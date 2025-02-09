#ifndef SAVE_FILE_H
#define SAVE_FILE_H

#include <PR/ultratypes.h>

#include "chaos/chaos.h"
#include "types.h"
#include "area.h"

#include "course_table.h"

#if defined(SRAM)
    #define EEPROM_SIZE 0x8000
#elif defined(EEP16K)
    #define EEPROM_SIZE 0x800
#else
    #define EEPROM_SIZE 0x200
#endif

#define NUM_SAVE_FILES 4

struct SaveBlockSignature {
    u16 magic;
    u16 chksum;
};

struct SaveFile {
    // Location of lost cap.
    // Note: the coordinates get set, but are never actually used, since the
    // cap can always be found in a fixed spot within the course
    union {
        struct {
            u8 capLevel;
            u8 capArea;
            Vec3s capPos;
        };
        u64 FORCED_ALIGNMENT;
    };

    u32 flags;

    // Star flags for each course.
    // The most significant bit of the byte *following* each course is set if the
    // cannon is open.
    u8 courseStars[COURSE_COUNT];

    u8 courseCoinScores[COURSE_STAGES_COUNT];

    s8 lives;
    u8 chaosDifficulty;
    u8 chaosChallengeMode;

    s32 chaosEntryCount;
    struct ChaosActiveEntry chaosEntries[CHAOS_PATCH_ENTRIES];

    u16 blueStars;
    u16 deaths;
    u16 gameLoads;
    u32 playTime;
    u16 totalPatches;

    struct SaveBlockSignature signature;
};

enum SaveFileChallenge {
    CHALLENGE_MODE_OFF,
    CHALLENGE_MODE_ON,
};

enum SaveFileIndex {
    SAVE_FILE_A,
    SAVE_FILE_B,
    SAVE_FILE_C,
    SAVE_FILE_D
};

struct MainMenuSaveData {
    // Each save file has a 2 bit "age" for each course. The higher this value,
    // the older the high score is. This is used for tie-breaking when displaying
    // on the high score screen.
    u32 coinScoreAges[NUM_SAVE_FILES];
    u8 soundMode: 2;
    u8 disableBGMusic: 1;
#ifdef WIDE
    u8 wideMode: 1;
#endif

    struct SaveBlockSignature signature;
};

struct SaveBuffer {
    // Each of the four save files has two copies. If one is bad, the other is used as a backup.
    struct SaveFile files[NUM_SAVE_FILES];
    // The main menu data has two copies. If one is bad, the other is used as a backup.
    struct MainMenuSaveData menuData;
};

STATIC_ASSERT(sizeof(struct SaveFile) % 8 == 0, "ERROR: SaveFile struct must be multiple of 8!");
STATIC_ASSERT(sizeof(struct MainMenuSaveData) % 8 == 0, "ERROR: MainMenuSaveData struct must be multiple of 8!");
STATIC_ASSERT(sizeof(struct SaveBuffer) % 8 == 0, "ERROR: SaveBuffer should be multiple of 8!");
STATIC_ASSERT(sizeof(struct SaveBuffer) <= EEPROM_SIZE, "ERROR: Save file too large!");

extern u8 gLastCompletedCourseNum;
extern u8 gLastCompletedStarNum;
extern s8 sUnusedGotGlobalCoinHiScore;
extern u8 gGotFileCoinHiScore;
extern u8 gCurrCourseStarFlags;
extern u8 gSpecialTripleJump;
extern s8 gLevelToCourseNumTable[];

// game progress flags
#define SAVE_FLAG_FILE_EXISTS            /* 0x00000001 */ (1 << 0)
#define SAVE_FLAG_HAVE_WING_CAP          /* 0x00000002 */ (1 << 1)
#define SAVE_FLAG_HAVE_METAL_CAP         /* 0x00000004 */ (1 << 2)
#define SAVE_FLAG_HAVE_VANISH_CAP        /* 0x00000008 */ (1 << 3)
#define SAVE_FLAG_HAVE_KEY_1             /* 0x00000010 */ (1 << 4)
#define SAVE_FLAG_HAVE_KEY_2             /* 0x00000020 */ (1 << 5)
#define SAVE_FLAG_UNLOCKED_BASEMENT_DOOR /* 0x00000040 */ (1 << 6)
#define SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR /* 0x00000080 */ (1 << 7)
#define SAVE_FLAG_DDD_MOVED_BACK         /* 0x00000100 */ (1 << 8)
#define SAVE_FLAG_MOAT_DRAINED           /* 0x00000200 */ (1 << 9)
#define SAVE_FLAG_UNLOCKED_PSS_DOOR      /* 0x00000400 */ (1 << 10)
#define SAVE_FLAG_UNLOCKED_WF_DOOR       /* 0x00000800 */ (1 << 11)
#define SAVE_FLAG_UNLOCKED_CCM_DOOR      /* 0x00001000 */ (1 << 12)
#define SAVE_FLAG_UNLOCKED_JRB_DOOR      /* 0x00002000 */ (1 << 13)
#define SAVE_FLAG_UNLOCKED_BITDW_DOOR    /* 0x00004000 */ (1 << 14)
#define SAVE_FLAG_UNLOCKED_BITFS_DOOR    /* 0x00008000 */ (1 << 15)
#define SAVE_FLAG_CAP_ON_GROUND          /* 0x00010000 */ (1 << 16)
#define SAVE_FLAG_CAP_ON_KLEPTO          /* 0x00020000 */ (1 << 17)
#define SAVE_FLAG_CAP_ON_UKIKI           /* 0x00040000 */ (1 << 18)
#define SAVE_FLAG_CAP_ON_MR_BLIZZARD     /* 0x00080000 */ (1 << 19)
#define SAVE_FLAG_UNLOCKED_50_STAR_DOOR  /* 0x00100000 */ (1 << 20)
#define SAVE_FLAG_COLLECTED_TOAD_STAR_1  /* 0x01000000 */ (1 << 24)
#define SAVE_FLAG_COLLECTED_TOAD_STAR_2  /* 0x02000000 */ (1 << 25)
#define SAVE_FLAG_COLLECTED_TOAD_STAR_3  /* 0x04000000 */ (1 << 26)
#define SAVE_FLAG_COLLECTED_MIPS_STAR_1  /* 0x08000000 */ (1 << 27)
#define SAVE_FLAG_COLLECTED_MIPS_STAR_2  /* 0x10000000 */ (1 << 28)

#define SAVE_FLAG_TO_STAR_FLAG(cmd) (((cmd) >> 24) & 0x7F)
#define STAR_FLAG_TO_SAVE_FLAG(cmd) ((cmd) << 24)

// Variable for setting a warp checkpoint.

// possibly a WarpDest struct where arg is a union. TODO: Check?
struct WarpCheckpoint {
    /*0x00*/ u8 actNum;
    /*0x01*/ u8 courseNum;
    /*0x02*/ u8 levelID;
    /*0x03*/ u8 areaNum;
    /*0x04*/ u8 warpNode;
};

extern struct WarpCheckpoint gWarpCheckpoint;

extern s8 gMainMenuDataModified;
extern s8 gSaveFileModified;

void save_file_do_save(s32 fileIndex);
void save_file_erase(s32 fileIndex);
BAD_RETURN(s32) save_file_copy(s32 srcFileIndex, s32 destFileIndex);
void save_file_load_all(void);
void save_file_reload(void);
void save_file_collect_star_or_key(s16 coinScore, s16 starIndex);
s32 save_file_exists(s32 fileIndex);
u32 save_file_get_max_coin_score(s32 courseIndex);
s32 save_file_get_course_star_count(s32 fileIndex, s32 courseIndex);
s32 save_file_get_total_star_count(s32 fileIndex, s32 minCourse, s32 maxCourse);
void save_file_set_flags(u32 flags);
void save_file_clear_flags(u32 flags);
u32 save_file_get_flags(void);
u32 save_file_get_star_flags(s32 fileIndex, s32 courseIndex);
void save_file_set_star_flags(s32 fileIndex, s32 courseIndex, u32 starFlags);
void save_file_remove_star_flags(s32 fileIndex, s32 courseIndex, u32 starFlags);
s32 save_file_get_course_coin_score(s32 fileIndex, s32 courseIndex);
s32 save_file_is_cannon_unlocked(void);
void save_file_set_cannon_unlocked(void);
s8 save_file_get_life_count(s32 fileIndex);
void save_file_set_life_count(s32 fileIndex, s8 lives);
void save_file_set_cap_pos(s16 x, s16 y, s16 z);
s32 save_file_get_cap_pos(Vec3s capPos);
void save_file_set_sound_mode(u16 mode);
u16 save_file_get_sound_mode(void);
void save_file_move_cap_to_default_location(void);
void save_file_get_chaos_data(struct ChaosActiveEntry **entryData, s32 **currentEntryCount, enum ChaosDifficulty *gChaosDifficulty, u8 *gChaosLivesEnabled);

void save_file_add_blue_star();
u16 save_file_get_blue_stars();
void save_file_add_death_count();
u16 save_file_get_death_count();
void save_file_add_game_load();
u16 save_file_get_game_loads();
void save_file_update_play_time();
u32 save_file_get_play_time();
void save_file_update_total_patches();
u16 save_file_get_total_patches();

void disable_warp_checkpoint(void);
void check_if_should_set_warp_checkpoint(struct WarpNode *warpNode);
s32 check_warp_checkpoint(struct WarpNode *warpNode);

u32 save_file_get_bg_music_disabled(void);
void save_file_set_bg_music(u8 shouldDisable);

#ifdef WIDE
u32 save_file_get_widescreen_mode(void);
void save_file_set_widescreen_mode(u8 mode);
#endif

#ifdef VERSION_EU
enum EuLanguages {
    LANGUAGE_ENGLISH,
    LANGUAGE_FRENCH,
    LANGUAGE_GERMAN
};

void eu_set_language(u16 language);
u16 eu_get_language(void);
#endif

s32 save_file_get_difficulty(s32 fileIndex);
s32 save_file_get_challenge_mode(s32 fileIndex);
void save_file_set_gamemode(s32 fileIndex, s32 difficulty, s32 challenge);

#endif // SAVE_FILE_H
