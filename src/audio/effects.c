#include <ultra64.h>

#include "effects.h"
#include "load.h"
#include "data.h"
#include "external.h"
#include "seqplayer.h"
#include "external.h"
#include "game/game_init.h"
#include "game/main.h"
#include "engine/math_util.h"

#if defined(VERSION_EU) || defined(VERSION_SH)
void sequence_channel_process_sound(struct SequenceChannel *seqChannel, s32 recalculateVolume) {
    f32 channelVolume;
    s32 i;

    if (seqChannel->changes.as_bitfields.volume || recalculateVolume) {
        channelVolume = seqChannel->volume * seqChannel->volumeScale * seqChannel->seqPlayer->appliedFadeVolume;
        if (seqChannel->seqPlayer->muted && (seqChannel->muteBehavior & MUTE_BEHAVIOR_SOFTEN) != 0) {
            channelVolume = seqChannel->seqPlayer->muteVolumeScale * channelVolume;
        }
#ifdef VERSION_SH
        seqChannel->appliedVolume = channelVolume * channelVolume;
#else
        seqChannel->appliedVolume = channelVolume;
#endif
    }

    if (seqChannel->changes.as_bitfields.pan) {
        seqChannel->pan = seqChannel->newPan * seqChannel->panChannelWeight;
    }

    for (i = 0; i < 4; ++i) {
        struct SequenceChannelLayer *layer = seqChannel->layers[i];
        if (layer != NULL && layer->enabled && layer->note != NULL) {
            if (layer->notePropertiesNeedInit) {
                layer->noteFreqScale = layer->freqScale * seqChannel->freqScale;
                layer->noteVelocity = layer->velocitySquare * seqChannel->appliedVolume;
                layer->notePan = (seqChannel->pan + layer->pan * (0x80 - seqChannel->panChannelWeight)) >> 7;
                layer->notePropertiesNeedInit = FALSE;
            } else {
                if (seqChannel->changes.as_bitfields.freqScale) {
                    layer->noteFreqScale = layer->freqScale * seqChannel->freqScale;
                }
                if (seqChannel->changes.as_bitfields.volume || recalculateVolume) {
                    layer->noteVelocity = layer->velocitySquare * seqChannel->appliedVolume;
                }
                if (seqChannel->changes.as_bitfields.pan) {
                    layer->notePan = (seqChannel->pan + layer->pan * (0x80 - seqChannel->panChannelWeight)) >> 7;
                }
            }
        }
    }
    seqChannel->changes.as_u8 = 0;
}
#else
static void sequence_channel_process_sound(struct SequenceChannel *seqChannel) {
    s32 hasProcessedChannel = FALSE;
    f32 channelVolume;
    f32 panFromChannel;
    f32 panLayerWeight;

    for (s32 i = 0; i < LAYERS_MAX; i++) {
        struct SequenceChannelLayer *layer = seqChannel->layers[i];
        if (layer != NULL && layer->enabled && layer->note != NULL) {
            if (!hasProcessedChannel) {
                hasProcessedChannel = TRUE;

                if (gConfig.disableBGMusic &&
                    (seqChannel->seqPlayer == &gSequencePlayers[SEQ_PLAYER_LEVEL]
                    || seqChannel->seqPlayer == &gSequencePlayers[SEQ_PLAYER_ENV])) {
                    channelVolume = 0;
                } else {
                    channelVolume = seqChannel->volume * seqChannel->volumeScale * seqChannel->seqPlayer->fadeVolume;
                    if (seqChannel->seqPlayer->muted && (seqChannel->muteBehavior & MUTE_BEHAVIOR_SOFTEN) != 0) {
                        channelVolume *= seqChannel->seqPlayer->muteVolumeScale;
                    }
                }

                panFromChannel = seqChannel->pan * seqChannel->panChannelWeight;
                panLayerWeight = 1.0f - seqChannel->panChannelWeight;
            }

            layer->noteFreqScale = layer->freqScale * seqChannel->freqScale;
            layer->noteVelocity = layer->velocitySquare * channelVolume;
            layer->notePan = (layer->pan * panLayerWeight) + panFromChannel;
        }
    }
}
#endif

void sequence_player_process_sound(struct SequencePlayer *seqPlayer) {
    s32 i;

    if (seqPlayer->fadeRemainingFrames != 0) {
        seqPlayer->fadeVolume += seqPlayer->fadeVelocity;
#if defined(VERSION_EU) || defined(VERSION_SH)
        seqPlayer->recalculateVolume = TRUE;
#endif
        seqPlayer->fadeVolume = CLAMP(seqPlayer->fadeVolume, 0, 1);

        if (--seqPlayer->fadeRemainingFrames == 0) {
#if defined(VERSION_EU) || defined(VERSION_SH)
            if (seqPlayer->state == 2) {
                sequence_player_disable(seqPlayer);
                return;
            }
#else
            switch (seqPlayer->state) {
                case SEQUENCE_PLAYER_STATE_FADE_OUT:
                    sequence_player_disable(seqPlayer);
                    return;

                case SEQUENCE_PLAYER_STATE_2:
                case SEQUENCE_PLAYER_STATE_3:
                    seqPlayer->state = SEQUENCE_PLAYER_STATE_0;
                    break;

                case SEQUENCE_PLAYER_STATE_4:
                    break;
            }
#endif
        }
    }

#if defined(VERSION_EU) || defined(VERSION_SH)
    if (seqPlayer->recalculateVolume) {
        seqPlayer->appliedFadeVolume = seqPlayer->fadeVolume * seqPlayer->fadeVolumeScale;
    }
#endif

    // Process channels
    for (i = 0; i < CHANNELS_MAX; i++) {
        if (IS_SEQUENCE_CHANNEL_VALID(seqPlayer->channels[i]) == TRUE
            && seqPlayer->channels[i]->enabled == TRUE) {
#if defined(VERSION_EU) || defined(VERSION_SH)
            sequence_channel_process_sound(seqPlayer->channels[i], seqPlayer->recalculateVolume);
#else
            sequence_channel_process_sound(seqPlayer->channels[i]);
#endif
        }
    }

#if defined(VERSION_EU) || defined(VERSION_SH)
    seqPlayer->recalculateVolume = FALSE;
#endif
}

f32 get_portamento_freq_scale(struct Portamento *p) {
#if defined(VERSION_JP) || defined(VERSION_US)
    if (p->mode == 0) {
        return 1.0f;
    }
#endif

    p->cur += p->speed;
    u32 v0 = (u32) p->cur;

    if (v0 > 127) {
        v0 = 127;
    }

    return (1.0f + (p->extent * (gPitchBendFrequencyScale[v0 + 128] - 1.0f)));
}

s32 get_vibrato_pitch_change(struct VibratoState *vib) {
#if defined(VERSION_EU) || defined(VERSION_SH)
    vib->time += (s32) vib->rate;
    s32 index = (vib->time >> 10) & 0x3F;
    return vib->curve[index] >> 8;
}
#else
    vib->time += vib->rate;

    s32 index = (vib->time >> 10) & 0x3F;

    switch (index & 0x30) {
        case 0x10:
            index = 31 - index;
            // fallthrough

        case 0x00:
            return vib->curve[index];

        case 0x20:
            index -= 0x20;
            break;

        case 0x30:
            index = 63 - index;
            break;
    }

    return -vib->curve[index];
}
#endif

f32 get_vibrato_freq_scale(struct VibratoState *vib) {
    s32 vibratoExtentTarget;

    if (vib->delay != 0) {
        vib->delay--;
        return 1;
    }

    // This needs to be set locally because changing the original value to 0 overrides the whole channel,
    // effectively negating the ability to disable vibrato and bypass this function in the first place.
    // This function isn't huge but it otherwise would get called many thousands of times per second.
    vibratoExtentTarget = vib->seqChannel->vibratoExtentTarget;
    if (vibratoExtentTarget >= VIBRATO_DISABLED_VALUE) {
        vibratoExtentTarget = 0;
    }

    if (vib->extentChangeTimer) {
        if (vib->extentChangeTimer == 1) {
            vib->extent = vibratoExtentTarget;
        } else {
            vib->extent +=
                (vibratoExtentTarget - vib->extent) / (s32) vib->extentChangeTimer;
        }

        vib->extentChangeTimer--;
    } else if (vibratoExtentTarget != (s32) vib->extent) {
        if ((vib->extentChangeTimer = vib->seqChannel->vibratoExtentChangeDelay) == 0) {
            vib->extent = vibratoExtentTarget;
        }
    }

    if (vib->rateChangeTimer) {
        if (vib->rateChangeTimer == 1) {
            vib->rate = (s32) vib->seqChannel->vibratoRateTarget;
        } else {
            vib->rate += ((s32) vib->seqChannel->vibratoRateTarget - vib->rate) / (s32) vib->rateChangeTimer;
        }

        vib->rateChangeTimer--;
    } else if (vib->seqChannel->vibratoRateTarget != (s32) vib->rate) {
        if ((vib->rateChangeTimer = vib->seqChannel->vibratoRateChangeDelay) == 0) {
            vib->rate = (s32) vib->seqChannel->vibratoRateTarget;
        }
    }

    if (vib->extent == 0) {
        return 1.0f;
    }

    s32 pitchChange = get_vibrato_pitch_change(vib);
    f32 extent = (f32) vib->extent / 4096.0f;

    return 1.0f + extent * (gPitchBendFrequencyScale[pitchChange + 128] - 1.0f);
}

void note_vibrato_update(struct Note *note) {
#if defined(VERSION_EU) || defined(VERSION_SH)
    if (note->portamento.mode != 0) {
        note->portamentoFreqScale = get_portamento_freq_scale(&note->portamento);
    }
    if (note->vibratoState.active && note->parentLayer != NO_LAYER) {
        note->vibratoFreqScale = get_vibrato_freq_scale(&note->vibratoState);
    }
#else
    if (note->vibratoState.activeFlags & VIBMODE_PORTAMENTO) {
        note->portamentoFreqScale = get_portamento_freq_scale(&note->portamento);
    }
    if ((note->vibratoState.activeFlags & VIBMODE_VIBRATO) && note->parentLayer != NO_LAYER) {
        note->vibratoFreqScale = get_vibrato_freq_scale(&note->vibratoState);
    }
#endif
}

void note_vibrato_init(struct Note *note) {
#if defined(VERSION_EU) || defined(VERSION_SH)
    struct NotePlaybackState *seqPlayerState = (struct NotePlaybackState *) &note->priority;
#endif

    note->vibratoFreqScale = 1.0f;
    note->portamentoFreqScale = 1.0f;

    struct VibratoState *vib = &note->vibratoState;

#if defined(VERSION_JP) || defined(VERSION_US)
    vib->activeFlags = VIBMODE_NONE;
    if (note->parentLayer->portamento.mode != 0) {
        vib->activeFlags |= VIBMODE_PORTAMENTO;
        note->portamento = note->parentLayer->portamento;
    }

    if (!(note->parentLayer->seqChannel->vibratoExtentStart == 0
        && note->parentLayer->seqChannel->vibratoExtentTarget >= VIBRATO_DISABLED_VALUE)) {
        vib->activeFlags |= VIBMODE_VIBRATO;
    } else {
        return;
    }
#else
    vib->active = TRUE;
#endif

    vib->time = 0;

#if defined(VERSION_EU) || defined(VERSION_SH)
    vib->curve = gWaveSamples[2];
    vib->seqChannel = note->parentLayer->seqChannel;
    if ((vib->extentChangeTimer = vib->seqChannel->vibratoExtentChangeDelay) == 0) {
        vib->extent = FLOAT_CAST(vib->seqChannel->vibratoExtentTarget);
    } else {
        vib->extent = FLOAT_CAST(vib->seqChannel->vibratoExtentStart);
    }

    if ((vib->rateChangeTimer = vib->seqChannel->vibratoRateChangeDelay) == 0) {
        vib->rate = FLOAT_CAST(vib->seqChannel->vibratoRateTarget);
    } else {
        vib->rate = FLOAT_CAST(vib->seqChannel->vibratoRateStart);
    }
    vib->delay = vib->seqChannel->vibratoDelay;

    seqPlayerState->portamento = seqPlayerState->parentLayer->portamento;
#else
    vib->curve = gVibratoCurve;
    vib->seqChannel = note->parentLayer->seqChannel;
    struct SequenceChannel *seqChannel = vib->seqChannel;

    if ((vib->extentChangeTimer = seqChannel->vibratoExtentChangeDelay) == 0) {
        vib->extent = seqChannel->vibratoExtentTarget;
    } else {
        vib->extent = seqChannel->vibratoExtentStart;
    }

    if ((vib->rateChangeTimer = seqChannel->vibratoRateChangeDelay) == 0) {
        vib->rate = seqChannel->vibratoRateTarget;
    } else {
        vib->rate = seqChannel->vibratoRateStart;
    }
    vib->delay = seqChannel->vibratoDelay;
#endif
}

void adsr_init(struct AdsrState *adsr, struct AdsrEnvelope *envelope, UNUSED s16 *volOut) {
    adsr->action = 0;
    adsr->state = ADSR_STATE_DISABLED;
#if defined(VERSION_EU) || defined(VERSION_SH)
    adsr->delay = 0;
    adsr->envelope = envelope;
#ifdef VERSION_SH
    adsr->sustain = 0.0f;
#endif
    adsr->current = 0.0f;
#else
    adsr->initial = 0;
    adsr->delay = 0;
    adsr->velocity = 0;
    adsr->envelope = envelope;
    adsr->volOut = volOut;
#endif
}

#if defined(VERSION_EU) || defined(VERSION_SH)
f32 adsr_update(struct AdsrState *adsr) {
#else
s32 adsr_update(struct Note *note) {
    struct AdsrState *adsr = &note->adsr;
    u8 isInit = FALSE;
#endif
    u8 action = adsr->action;
    u8 state = adsr->state;
    switch (state) {
        case ADSR_STATE_DISABLED:
            return 0;

        case ADSR_STATE_INITIAL:
            isInit = TRUE;
            // fallthrough

        case ADSR_STATE_RESTART:
#if defined(VERSION_JP) || defined(VERSION_US)
            adsr->current = adsr->initial;
            adsr->target = adsr->initial;
#endif
            if (action & ADSR_ACTION_HANG) {
                adsr->state = ADSR_STATE_HANG;
                break;
            }
            // fallthrough

        case ADSR_STATE_START_LOOP:
            adsr->envIndex = 0;
#if defined(VERSION_JP) || defined(VERSION_US)
            adsr->currentHiRes = (adsr->current << 0x10);
#endif
            adsr->state = ADSR_STATE_LOOP;
#ifdef VERSION_SH
            // fallthrough
            restart:
#endif
            // fallthrough

        case ADSR_STATE_LOOP:
            adsr->delay = BSWAP16(adsr->envelope[adsr->envIndex].delay);
            switch (adsr->delay) {
                case ADSR_DISABLE:
                    adsr->state = ADSR_STATE_DISABLED;
                    break;
                case ADSR_HANG:
                    adsr->state = ADSR_STATE_HANG;
                    break;
                case ADSR_GOTO:
                    adsr->envIndex = BSWAP16(adsr->envelope[adsr->envIndex].arg);
#ifdef VERSION_SH
                    goto restart;
#else
                    break;
#endif
                case ADSR_RESTART:
                    adsr->state = ADSR_STATE_RESTART;
                    break;

                default:
#if defined(VERSION_EU) || defined(VERSION_SH)
                    if (adsr->delay >= 4) {
                        adsr->delay = adsr->delay * gAudioBufferParameters.updatesPerFrame
 #ifdef VERSION_SH
                        / gAudioBufferParameters.presetUnk4
 #endif
                        / 4;
                    }
 #ifdef VERSION_SH
                    if (adsr->delay == 0) {
                        adsr->delay = 1;
                    }
 #endif
                    adsr->target = (f32) BSWAP16(adsr->envelope[adsr->envIndex].arg) / 32767.0f;
                    adsr->target = adsr->target * adsr->target;
                    adsr->velocity = (adsr->target - adsr->current) / adsr->delay;
#else // !(VERSION_EU || VERSION_SH)
                    if (adsr->delay <= 0) {
                        adsr->delay = 1;
                        note->initFullVelocity = isInit;
                    }

                    adsr->target = BSWAP16(adsr->envelope[adsr->envIndex].arg);
                    adsr->velocity = ((adsr->target - adsr->current) << 0x10) / adsr->delay;
#endif // !(VERSION_EU || VERSION_SH)
                    adsr->state = ADSR_STATE_FADE;
                    adsr->envIndex++;
                    break;
            }
            if (adsr->state != ADSR_STATE_FADE) {
                break;
            }
            // fallthrough

        case ADSR_STATE_FADE:
#if defined(VERSION_EU) || defined(VERSION_SH)
            adsr->current += adsr->velocity;
#else
            adsr->currentHiRes += adsr->velocity;
            adsr->current = adsr->currentHiRes >> 0x10;
#endif
            if (--adsr->delay <= 0) {
                adsr->state = ADSR_STATE_LOOP;
            }
            // fallthrough

        case ADSR_STATE_HANG:
            break;

        case ADSR_STATE_DECAY:
        case ADSR_STATE_RELEASE: {
            adsr->current -= adsr->fadeOutVel;
#if defined(VERSION_EU) || defined(VERSION_SH)
            if (adsr->sustain != 0.0f && state == ADSR_STATE_DECAY) {
#else
            if (adsr->sustain != 0 && adsr->state == ADSR_STATE_DECAY) {
#endif
                if (adsr->current < adsr->sustain) {
                    adsr->current = adsr->sustain;
#if defined(VERSION_EU) || defined(VERSION_SH)
                    adsr->delay = 128;
#else
                    adsr->delay = adsr->sustain / 16;
#endif
                    adsr->state = ADSR_STATE_SUSTAIN;
                }
                break;
            }

#if defined(VERSION_SH)
            if (adsr->current < 0.00001f) {
                adsr->current = 0.0f;
                adsr->state = ADSR_STATE_DISABLED;
            }
#elif defined(VERSION_EU)
            if (adsr->current < 0) {
                adsr->current = 0.0f;
                adsr->state = ADSR_STATE_DISABLED;
            }
#else
            if (adsr->current < 100) {
                adsr->current = 0;
                adsr->state = ADSR_STATE_DISABLED;
            }
#endif
            break;
        }

        case ADSR_STATE_SUSTAIN:
            adsr->delay -= 1;
            if (adsr->delay == 0) {
                adsr->state = ADSR_STATE_RELEASE;
            }
            break;
    }

    if ((action & ADSR_ACTION_DECAY)) {
        adsr->state = ADSR_STATE_DECAY;
        adsr->action = action & ~ADSR_ACTION_DECAY;
    }

    if ((action & ADSR_ACTION_RELEASE)) {
        adsr->state = ADSR_STATE_RELEASE;
#if defined(VERSION_EU) || defined(VERSION_SH)
        adsr->action = action & ~ADSR_ACTION_RELEASE;
#else
        adsr->action = action & ~(ADSR_ACTION_RELEASE | ADSR_ACTION_DECAY);
#endif
    }

#if defined(VERSION_EU) || defined(VERSION_SH)
    if (adsr->current < 0.0f) {
        return 0.0f;
    }
    if (adsr->current > 1.0f) {
        eu_stubbed_printf_1("Audio:Envp: overflow  %f\n", adsr->current);
        return 1.0f;
    }
    return adsr->current;
#else
    *adsr->volOut = adsr->current;
    return 0;
#endif
}
