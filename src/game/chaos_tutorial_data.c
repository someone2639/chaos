#include "chaos_tutorial.h"

s32 gChaosTutorialSlideIndex = 0;
u8 *gChaosTutorialLoadedAddr = NULL;
ALIGNED16 u8 gChaosTutorialImgBuffer[CHAOS_TUTORIAL_IMG_SIZE];

struct ChaosTutorialSlide gChaosTutorialSlides[] = {
    {
        .imageAddress = chsTutDMA_00_introduction,
        .description = "In @00AFFF--Super Mario 64: Rogue Chaos Edition@--------, you will decide your own destiny as you activate and coordinate game-altering patches throughout your experience.",
    },
    {
        .imageAddress = chsTutDMA_01_select_patch,
        .description = "In this game, Mario will be sent to this patch selection menu any time he collects a star, key, or activates a cap switch. Here, you will need to select a @FF7F00--patch card@--------.",
    },
    {
        .imageAddress = chsTutDMA_02_selection_layout,
        .description = "You will be confronted with a few different patch cards to choose from, each bundled with a unique @05DF15--positive@-------- and @FF1525--negative@-------- effect.",
    },
    {
        .imageAddress = chsTutDMA_03_patch_card,
        .description = "@FF7F00--Patch Cards@-------- come with many different components, and it is important to understand what each symbol represents.",
    },
    {
        .imageAddress = chsTutDMA_04_positive_negative,
        .description = "Each card's @05DF15--positive@-------- effect will be indicated by the @05DF15--green@-------- text on the card, and @FF1525--negative@-------- effects are indicated in @FF1525--red@--------.",
    },
    {
        .imageAddress = chsTutDMA_05_patch_rank,
        .description = "Each card will also be assigned a @3F7FFF--Patch Rank@--------. The higher the listed rank, the more impactful the associated effects will tend to be on the gameplay!",
    },
    {
        .imageAddress = chsTutDMA_06_time_limit,
        .description = "Most effects come with an @FFFF00--Expiration Timer@--------, which specifies how many stars (blue stars included) may be collected until the effect expires. Some may even last forever!",
    },
    {
        .imageAddress = chsTutDMA_07_total_uses,
        .description = "Some special effects will come with a @1FBF1F--Use Counter@-------- instead, which indicates a specific number of times that effect may be activated. These may be saved for use in the perfect scenario!",
    },
    {
        .imageAddress = chsTutDMA_08_special_events,
        .description = "Occasionally, you will encounter a special event when visiting the patch selection menu. The three event types include @25CF65--Lucky@-------- events, @FF625A--Bad Luck@-------- events, and @464646--Chaos@-------- events.",
    },
    {
        .imageAddress = chsTutDMA_09_event_lucky,
        .description = "A @25CF65--Lucky@-------- event will attempt to display positive effects that are valued one rank higher than each patch card's listed rank. Affected cards will be given a @25CF65--green@-------- background.",
    },
    {
        .imageAddress = chsTutDMA_0A_event_bad_luck,
        .description = "Conversely, a @FF625A--Bad Luck@-------- event will attempt to display negative effects that are valued one rank higher than each patch card's listed rank. Affected cards will be given a @FF625A--red@-------- background.",
    },
    {
        .imageAddress = chsTutDMA_0B_event_chaos,
        .description = "Lastly, a @464646--Chaos@-------- event will completely eliminate the positive effect from the patch cards, leaving only negative effects to choose from. Affected cards will be given a @464646--dark gray@-------- background.",
    },
    {
        .imageAddress = chsTutDMA_0C_difficulty_selection,
        .description = "This hack comes with an @1FAF1F--Easy@--------, @7F7F7F--Normal@--------, and @9F1F1F--Hard@-------- difficulty, as well as a special @FFBF00--Challenge Mode@--------.",
    },
    {
        .imageAddress = chsTutDMA_0D_difficulty_easy,
        .description = "On @1FAF1F--Easy@-------- difficulty, you won't have to worry so much about the most intense negative effects appearing. Recommended for players brand new to Super Mario 64, or Mario games in general.",
    },
    {
        .imageAddress = chsTutDMA_0E_difficulty_hard,
        .description = "On @9F1F1F--Hard@-------- difficulty, you will need to endure brutal negative effects more frequently and for longer, with fewer offsetting benefits from positive effects. Recommended for experienced players only.",
    },
    {
        .imageAddress = chsTutDMA_0F_challenge_mode,
        .description = "If you're feeling confident in your abilities, take on @FFBF00--Challenge Mode@--------! Your entire save file is on the line, and you must work to complete the game without running out of lives!",
    },
    {
        .imageAddress = chsTutDMA_10_conclusion,
        .description = "Your goal is to defeat @6F4F2F--Bowser@-------- and rescue the @FFFF00--Grand Star@--------. What kinds of unique effect combinations will you experience during your adventure?",
    },
};

const s32 gChaosTutorialSlideCount = (s32) ARRAY_COUNT(gChaosTutorialSlides);
