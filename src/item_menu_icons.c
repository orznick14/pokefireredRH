#include "global.h"
#include "gflib.h"
#include "decompress.h"
#include "graphics.h"
#include "item_menu_icons.h"
#include "constants/items.h"

static EWRAM_DATA u8 sItemMenuIconSpriteIds[12] = {0};
static EWRAM_DATA void * sItemIconTilesBuffer = NULL;
static EWRAM_DATA void * sItemIconTilesBufferPadded = NULL;

static void sub_8098560(struct Sprite * sprite);
static void sub_80985BC(struct Sprite * sprite);

static const struct OamData sOamData_BagOrSatchel = {
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .shape = ST_OAM_SQUARE,
    .size = 3,
    .priority = 1,
    .paletteNum = 0
};

static const union AnimCmd sAnim_BagOrSatchel_OpenItemPocket[] = {
    ANIMCMD_FRAME(   0, 5),
    ANIMCMD_FRAME(0x40, 0),
    ANIMCMD_END
};

static const union AnimCmd sAnim_BagOrSatchel_OpenKeyItemPocket[] = {
    ANIMCMD_FRAME(   0, 5),
    ANIMCMD_FRAME(0x80, 0),
    ANIMCMD_END
};

static const union AnimCmd sAnim_BagOrSatchel_OpenPokeBallsPocket[] = {
    ANIMCMD_FRAME(   0, 5),
    ANIMCMD_FRAME(0xc0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_BagOrSatchel[] = {
    sAnim_BagOrSatchel_OpenKeyItemPocket,
    sAnim_BagOrSatchel_OpenPokeBallsPocket,
    sAnim_BagOrSatchel_OpenItemPocket
};

static const union AffineAnimCmd sAffineAnim_Idle[] = {
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Wobble[] = {
    AFFINEANIMCMD_FRAME(0, 0, -2, 2),
    AFFINEANIMCMD_FRAME(0, 0,  2, 4),
    AFFINEANIMCMD_FRAME(0, 0, -2, 4),
    AFFINEANIMCMD_FRAME(0, 0,  2, 2),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnimTable_BagOrSatchel[] = {
    sAffineAnim_Idle,
    sAffineAnim_Wobble
};

const struct CompressedSpriteSheet gSpriteSheet_Backpack = {
    gUnknown_8E8362C,
    0x2000,
    100
};

const struct CompressedSpriteSheet gSpriteSheet_Satchel = {
    gUnknown_8E83DBC,
    0x2000,
    100
};

const struct CompressedSpritePalette gSpritePalette_BagOrSatchel = {
    gUnknown_8E84560,
    100
};

static const struct SpriteTemplate sSpriteTemplate_BagOrSatchel = {
    100,
    100,
    &sOamData_BagOrSatchel,
    sAnimTable_BagOrSatchel,
    NULL,
    sAffineAnimTable_BagOrSatchel,
    SpriteCallbackDummy
};

static const struct OamData gUnknown_83D4214 = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .shape = ST_OAM_SQUARE,
    .size = 1,
    .priority = 1,
    .paletteNum = 1
};

static const union AnimCmd gUnknown_83D421C[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd gUnknown_83D4224[] = {
    ANIMCMD_FRAME(4, 0),
    ANIMCMD_END
};

static const union AnimCmd gUnknown_83D422C[] = {
    ANIMCMD_FRAME(0, 0, .hFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd *const gUnknown_83D4234[] = {
    gUnknown_83D421C,
    gUnknown_83D4224,
    gUnknown_83D422C
};

const struct CompressedSpriteSheet gBagSwapSpriteSheet = {
    gFile_graphics_interface_bag_swap_sheet,
    0x100,
    101
};

const struct CompressedSpritePalette gBagSwapSpritePalette = {
    gFile_graphics_interface_bag_swap_palette,
    101
};

static const struct SpriteTemplate gUnknown_83D4250 = {
    101,
    101,
    &gUnknown_83D4214,
    gUnknown_83D4234,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static const struct OamData sOamData_ItemIcon = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .shape = ST_OAM_SQUARE,
    .size = 2,
    .priority = 1,
    .paletteNum = 2
};

static const union AnimCmd sAnim_ItemIcon_0[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnimTable_ItemIcon[] = {
    sAnim_ItemIcon_0
};

static const struct SpriteTemplate sSpriteTemplate_ItemIcon = {
    102,
    102,
    &sOamData_ItemIcon,
    sAnimTable_ItemIcon,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static const void *const sItemIconGfxPtrs[][2] = {
    [ITEM_NONE] = {gItemIcon_QuestionMark, gItemIconPalette_QuestionMark},
    // Pokeballs
    [ITEM_MASTER_BALL] = {gItemIcon_MasterBall, gItemIconPalette_MasterBall},
    [ITEM_ULTRA_BALL] = {gItemIcon_UltraBall, gItemIconPalette_UltraBall},
    [ITEM_GREAT_BALL] = {gItemIcon_GreatBall, gItemIconPalette_GreatBall},
    [ITEM_POKE_BALL] = {gItemIcon_PokeBall, gItemIconPalette_PokeBall},
    [ITEM_SAFARI_BALL] = {gItemIcon_SafariBall, gItemIconPalette_SafariBall},
    [ITEM_NET_BALL] = {gItemIcon_NetBall, gItemIconPalette_NetBall},
    [ITEM_DIVE_BALL] = {gItemIcon_DiveBall, gItemIconPalette_DiveBall},
    [ITEM_NEST_BALL] = {gItemIcon_NestBall, gItemIconPalette_NestBall},
    [ITEM_REPEAT_BALL] = {gItemIcon_RepeatBall, gItemIconPalette_RepeatBall},
    [ITEM_TIMER_BALL] = {gItemIcon_TimerBall, gItemIconPalette_RepeatBall},
    [ITEM_LUXURY_BALL] = {gItemIcon_LuxuryBall, gItemIconPalette_LuxuryBall},
    [ITEM_PREMIER_BALL] = {gItemIcon_PremierBall, gItemIconPalette_LuxuryBall},
    [ITEM_LEVEL_BALL] = {gItemIcon_LevelBall, gItemIconPalette_LevelBall},
    [ITEM_LURE_BALL] = {gItemIcon_LureBall, gItemIconPalette_LureBall},
    [ITEM_MOON_BALL] = {gItemIcon_MoonBall, gItemIconPalette_MoonBall},
    [ITEM_FRIEND_BALL] = {gItemIcon_FriendBall, gItemIconPalette_FriendBall},
    [ITEM_LOVE_BALL] = {gItemIcon_LoveBall, gItemIconPalette_LoveBall},
    [ITEM_HEAVY_BALL] = {gItemIcon_HeavyBall, gItemIconPalette_HeavyBall},
    [ITEM_FAST_BALL] = {gItemIcon_FastBall, gItemIconPalette_FastBall},
    [ITEM_SPORT_BALL] = {gItemIcon_SportBall, gItemIconPalette_SportBall},
    [ITEM_DUSK_BALL] = {gItemIcon_DuskBall, gItemIconPalette_DuskBall},
    [ITEM_QUICK_BALL] = {gItemIcon_QuickBall, gItemIconPalette_QuickBall},
    [ITEM_HEAL_BALL] = {gItemIcon_HealBall, gItemIconPalette_HealBall},
    [ITEM_CHERISH_BALL] = {gItemIcon_CherishBall, gItemIconPalette_CherishBall},
    [ITEM_PARK_BALL] = {gItemIcon_ParkBall, gItemIconPalette_ParkBall},
    [ITEM_DREAM_BALL] = {gItemIcon_DreamBall, gItemIconPalette_DreamBall},
    [ITEM_BEAST_BALL] = {gItemIcon_BeastBall, gItemIconPalette_BeastBall},
    // Medicine
    [ITEM_POTION] = {gItemIcon_Potion, gItemIconPalette_Potion},
    [ITEM_ANTIDOTE] = {gItemIcon_Antidote, gItemIconPalette_Antidote},
    [ITEM_BURN_HEAL] = {gItemIcon_StatusHeal, gItemIconPalette_BurnHeal},
    [ITEM_ICE_HEAL] = {gItemIcon_StatusHeal, gItemIconPalette_IceHeal},
    [ITEM_AWAKENING] = {gItemIcon_StatusHeal, gItemIconPalette_Awakening},
    [ITEM_PARALYZE_HEAL] = {gItemIcon_StatusHeal, gItemIconPalette_ParalyzeHeal},
    [ITEM_FULL_RESTORE] = {gItemIcon_LargePotion, gItemIconPalette_FullRestore},
    [ITEM_MAX_POTION] = {gItemIcon_LargePotion, gItemIconPalette_MaxPotion},
    [ITEM_HYPER_POTION] = {gItemIcon_Potion, gItemIconPalette_HyperPotion},
    [ITEM_SUPER_POTION] = {gItemIcon_Potion, gItemIconPalette_SuperPotion},
    [ITEM_FULL_HEAL] = {gItemIcon_FullHeal, gItemIconPalette_FullHeal},
    [ITEM_REVIVE] = {gItemIcon_Revive, gItemIconPalette_Revive},
    [ITEM_MAX_REVIVE] = {gItemIcon_MaxRevive, gItemIconPalette_Revive},
    [ITEM_FRESH_WATER] = {gItemIcon_FreshWater, gItemIconPalette_FreshWater},
    [ITEM_SODA_POP] = {gItemIcon_SodaPop, gItemIconPalette_SodaPop},
    [ITEM_LEMONADE] = {gItemIcon_Lemonade, gItemIconPalette_Lemonade},
    [ITEM_MOOMOO_MILK] = {gItemIcon_MoomooMilk, gItemIconPalette_MoomooMilk},
    [ITEM_ENERGY_POWDER] = {gItemIcon_Powder, gItemIconPalette_EnergyPowder},
    [ITEM_ENERGY_ROOT] = {gItemIcon_EnergyRoot, gItemIconPalette_EnergyRoot},
    [ITEM_HEAL_POWDER] = {gItemIcon_Powder, gItemIconPalette_HealPowder},
    [ITEM_REVIVAL_HERB] = {gItemIcon_RevivalHerb, gItemIconPalette_RevivalHerb},
    [ITEM_ETHER] = {gItemIcon_Ether, gItemIconPalette_Ether},
    [ITEM_MAX_ETHER] = {gItemIcon_Ether, gItemIconPalette_MaxEther},
    [ITEM_ELIXIR] = {gItemIcon_Ether, gItemIconPalette_Elixir},
    [ITEM_MAX_ELIXIR] = {gItemIcon_Ether, gItemIconPalette_MaxElixir},
    [ITEM_LAVA_COOKIE] = {gItemIcon_LavaCookie, gItemIconPalette_LavaCookieAndLetter},
    [ITEM_BLUE_FLUTE] = {gItemIcon_Flute, gItemIconPalette_BlueFlute},
    [ITEM_YELLOW_FLUTE] = {gItemIcon_Flute, gItemIconPalette_YellowFlute},
    [ITEM_RED_FLUTE] = {gItemIcon_Flute, gItemIconPalette_RedFlute},
    [ITEM_BLACK_FLUTE] = {gItemIcon_Flute, gItemIconPalette_BlackFlute},
    [ITEM_WHITE_FLUTE] = {gItemIcon_Flute, gItemIconPalette_WhiteFlute},
    [ITEM_BERRY_JUICE] = {gItemIcon_BerryJuice, gItemIconPalette_BerryJuice},
    [ITEM_SACRED_ASH] = {gItemIcon_SacredAsh, gItemIconPalette_SacredAsh},
// Collectibles
    [ITEM_SHOAL_SALT] = {gItemIcon_Powder, gItemIconPalette_ShoalSalt},
    [ITEM_SHOAL_SHELL] = {gItemIcon_ShoalShell, gItemIconPalette_Shell},
    [ITEM_RED_SHARD] = {gItemIcon_Shard, gItemIconPalette_RedShard},
    [ITEM_BLUE_SHARD] = {gItemIcon_Shard, gItemIconPalette_BlueShard},
    [ITEM_YELLOW_SHARD] = {gItemIcon_Shard, gItemIconPalette_YellowShard},
    [ITEM_GREEN_SHARD] = {gItemIcon_Shard, gItemIconPalette_GreenShard},
// Vitamins
    [ITEM_HP_UP] = {gItemIcon_HPUp, gItemIconPalette_HPUp},
    [ITEM_PROTEIN] = {gItemIcon_Vitamin, gItemIconPalette_Protein},
    [ITEM_IRON] = {gItemIcon_Vitamin, gItemIconPalette_Iron},
    [ITEM_CARBOS] = {gItemIcon_Vitamin, gItemIconPalette_Carbos},
    [ITEM_CALCIUM] = {gItemIcon_Vitamin, gItemIconPalette_Calcium},
    [ITEM_RARE_CANDY] = {gItemIcon_RareCandy, gItemIconPalette_RareCandy},
    [ITEM_PP_UP] = {gItemIcon_PPUp, gItemIconPalette_PPUp},
    [ITEM_ZINC] = {gItemIcon_Vitamin, gItemIconPalette_Zinc},
    [ITEM_PP_MAX] = {gItemIcon_PPMax, gItemIconPalette_PPMax},
// Battle items
    [ITEM_GUARD_SPEC] = {gItemIcon_BattleStatItem, gItemIconPalette_GuardSpec},
    [ITEM_DIRE_HIT] = {gItemIcon_BattleStatItem, gItemIconPalette_DireHit},
    [ITEM_X_ATTACK] = {gItemIcon_BattleStatItem, gItemIconPalette_XAttack},
    [ITEM_X_DEFEND] = {gItemIcon_BattleStatItem, gItemIconPalette_XDefend},
    [ITEM_X_SPEED] = {gItemIcon_BattleStatItem, gItemIconPalette_XSpeed},
    [ITEM_X_ACCURACY] = {gItemIcon_BattleStatItem, gItemIconPalette_XAccuracy},
    [ITEM_X_SPECIAL] = {gItemIcon_BattleStatItem, gItemIconPalette_XSpecial},
    [ITEM_X_SPECIAL_DEFENSE] = {gItemIcon_XSpecialDefense, gItemIconPalette_XSpecialDefense},
    [ITEM_POKE_DOLL] = {gItemIcon_PokeDoll, gItemIconPalette_PokeDoll},
    [ITEM_FLUFFY_TAIL] = {gItemIcon_FluffyTail, gItemIconPalette_FluffyTail},
// Field items
    [ITEM_SUPER_REPEL] = {gItemIcon_Repel, gItemIconPalette_SuperRepel},
    [ITEM_MAX_REPEL] = {gItemIcon_Repel, gItemIconPalette_MaxRepel},
    [ITEM_ESCAPE_ROPE] = {gItemIcon_EscapeRope, gItemIconPalette_EscapeRope},
    [ITEM_REPEL] = {gItemIcon_Repel, gItemIconPalette_Repel},
// Other Food
    [ITEM_SWEET_HEART] = {gItemIcon_SweetHeart, gItemIconPalette_SweetHeart},
    [ITEM_BIG_MALASADA] = {gItemIcon_BigMalasada, gItemIconPalette_BigMalasada},
    [ITEM_CASTELIACONE] = {gItemIcon_Casteliacone, gItemIconPalette_Casteliacone},
    [ITEM_LUMIOSE_GALETTE] = {gItemIcon_LumioseGalette, gItemIconPalette_LumioseGalette},
    [ITEM_RAGE_CANDY_BAR] = {gItemIcon_RageCandyBar, gItemIconPalette_RageCandyBar},
    [ITEM_SHALOUR_SABLE] = {gItemIcon_ShalourSable, gItemIconPalette_ShalourSable},
    [ITEM_OLD_GATEAU] = {gItemIcon_OldGateau, gItemIconPalette_OldGateau},
// Evolution items
    [ITEM_SUN_STONE] = {gItemIcon_SunStone, gItemIconPalette_SunStone},
    [ITEM_MOON_STONE] = {gItemIcon_MoonStone, gItemIconPalette_MoonStone},
    [ITEM_FIRE_STONE] = {gItemIcon_FireStone, gItemIconPalette_FireStone},
    [ITEM_THUNDER_STONE] = {gItemIcon_ThunderStone, gItemIconPalette_ThunderStone},
    [ITEM_WATER_STONE] = {gItemIcon_WaterStone, gItemIconPalette_WaterStone},
    [ITEM_LEAF_STONE] = {gItemIcon_LeafStone, gItemIconPalette_LeafStone},
    [ITEM_OVAL_STONE] = {gItemIcon_OvalStone, gItemIconPalette_OvalStone},
    [ITEM_ICE_STONE] = {gItemIcon_IceStone, gItemIconPalette_IceStone},
    [ITEM_PROTECTOR] = {gItemIcon_Protector, gItemIconPalette_Protector},
    [ITEM_MAGMARIZER] = {gItemIcon_Magmarizer, gItemIconPalette_Magmarizer},
    [ITEM_PRISM_SCALE] = {gItemIcon_PrismScale, gItemIconPalette_PrismScale},
    [ITEM_SACHET] = {gItemIcon_Sachet, gItemIconPalette_Sachet},
    [ITEM_WHIPPED_DREAM] = {gItemIcon_WhippedDream, gItemIconPalette_WhippedDream},
    [ITEM_RAZOR_CLAW] = {gItemIcon_RazorClaw, gItemIconPalette_RazorClaw},
    [ITEM_RAZOR_FANG] = {gItemIcon_RazorFang, gItemIconPalette_RazorFang},
    [ITEM_REAPER_CLOTH] = {gItemIcon_ReaperCloth, gItemIconPalette_ReaperCloth},
    [ITEM_DUBIOUS_DISC] = {gItemIcon_DubiousDisc, gItemIconPalette_DubiousDisc},
    [ITEM_ELECTIRIZER] = {gItemIcon_Electirizer, gItemIconPalette_Electirizer},
// Wings
    [ITEM_HEALTH_WING] = {gItemIcon_HealthWing, gItemIconPalette_HealthWing},
    [ITEM_MUSCLE_WING] = {gItemIcon_MuscleWing, gItemIconPalette_MuscleWing},
    [ITEM_RESIST_WING] = {gItemIcon_ResistWing, gItemIconPalette_ResistWing},
    [ITEM_GENIUS_WING] = {gItemIcon_GeniusWing, gItemIconPalette_GeniusWing},
    [ITEM_CLEVER_WING] = {gItemIcon_CleverWing, gItemIconPalette_CleverWing},
    [ITEM_SWIFT_WING] = {gItemIcon_SwiftWing, gItemIconPalette_SwiftWing},
    [ITEM_PRETTY_WING] = {gItemIcon_PrettyWing, gItemIconPalette_PrettyWing},
//Valuable
    [ITEM_TINY_MUSHROOM] = {gItemIcon_TinyMushroom, gItemIconPalette_Mushroom},
    [ITEM_BIG_MUSHROOM] = {gItemIcon_BigMushroom, gItemIconPalette_Mushroom},
    [ITEM_PEARL] = {gItemIcon_Pearl, gItemIconPalette_Pearl},
    [ITEM_BIG_PEARL] = {gItemIcon_BigPearl, gItemIconPalette_Pearl},
    [ITEM_STARDUST] = {gItemIcon_Stardust, gItemIconPalette_Star},
    [ITEM_STAR_PIECE] = {gItemIcon_StarPiece, gItemIconPalette_Star},
    [ITEM_NUGGET] = {gItemIcon_Nugget, gItemIconPalette_Nugget},
    [ITEM_HEART_SCALE] = {gItemIcon_HeartScale, gItemIconPalette_HeartScale},
    [ITEM_RARE_BONE] = {gItemIcon_RareBone, gItemIconPalette_RareBone},
    [ITEM_PEARL_STRING] = {gItemIcon_PearlString, gItemIconPalette_PearlString},
    [ITEM_BIG_NUGGET] = {gItemIcon_BigNugget, gItemIconPalette_BigNugget},
    [ITEM_COMET_SHARD] = {gItemIcon_CometShard, gItemIconPalette_CometShard},
    [ITEM_BALM_MUSHROOM] = {gItemIcon_BalmMushroom, gItemIconPalette_BalmMushroom},
// Mail
    [ITEM_ORANGE_MAIL] = {gItemIcon_OrangeMail, gItemIconPalette_OrangeMail},
    [ITEM_HARBOR_MAIL] = {gItemIcon_HarborMail, gItemIconPalette_HarborMail},
    [ITEM_GLITTER_MAIL] = {gItemIcon_GlitterMail, gItemIconPalette_GlitterMail},
    [ITEM_MECH_MAIL] = {gItemIcon_MechMail, gItemIconPalette_MechMail},
    [ITEM_WOOD_MAIL] = {gItemIcon_WoodMail, gItemIconPalette_WoodMail},
    [ITEM_WAVE_MAIL] = {gItemIcon_WaveMail, gItemIconPalette_WaveMail},
    [ITEM_BEAD_MAIL] = {gItemIcon_BeadMail, gItemIconPalette_BeadMail},
    [ITEM_SHADOW_MAIL] = {gItemIcon_ShadowMail, gItemIconPalette_ShadowMail},
    [ITEM_TROPIC_MAIL] = {gItemIcon_TropicMail, gItemIconPalette_TropicMail},
    [ITEM_DREAM_MAIL] = {gItemIcon_DreamMail, gItemIconPalette_DreamMail},
    [ITEM_FAB_MAIL] = {gItemIcon_FabMail, gItemIconPalette_FabMail},
    [ITEM_RETRO_MAIL] = {gItemIcon_RetroMail, gItemIconPalette_RetroMail},
// Berries
    [ITEM_CHERI_BERRY] = {gItemIcon_CheriBerry, gItemIconPalette_CheriBerry},
    [ITEM_CHESTO_BERRY] = {gItemIcon_ChestoBerry, gItemIconPalette_ChestoBerry},
    [ITEM_PECHA_BERRY] = {gItemIcon_PechaBerry, gItemIconPalette_PechaBerry},
    [ITEM_RAWST_BERRY] = {gItemIcon_RawstBerry, gItemIconPalette_RawstBerry},
    [ITEM_ASPEAR_BERRY] = {gItemIcon_AspearBerry, gItemIconPalette_AspearBerry},
    [ITEM_LEPPA_BERRY] = {gItemIcon_LeppaBerry, gItemIconPalette_LeppaBerry},
    [ITEM_ORAN_BERRY] = {gItemIcon_OranBerry, gItemIconPalette_OranBerry},
    [ITEM_PERSIM_BERRY] = {gItemIcon_PersimBerry, gItemIconPalette_PersimBerry},
    [ITEM_LUM_BERRY] = {gItemIcon_LumBerry, gItemIconPalette_LumBerry},
    [ITEM_SITRUS_BERRY] = {gItemIcon_SitrusBerry, gItemIconPalette_SitrusBerry},
    [ITEM_FIGY_BERRY] = {gItemIcon_FigyBerry, gItemIconPalette_FigyBerry},
    [ITEM_WIKI_BERRY] = {gItemIcon_WikiBerry, gItemIconPalette_WikiBerry},
    [ITEM_MAGO_BERRY] = {gItemIcon_MagoBerry, gItemIconPalette_MagoBerry},
    [ITEM_AGUAV_BERRY] = {gItemIcon_AguavBerry, gItemIconPalette_AguavBerry},
    [ITEM_IAPAPA_BERRY] = {gItemIcon_IapapaBerry, gItemIconPalette_IapapaBerry},
    [ITEM_RAZZ_BERRY] = {gItemIcon_RazzBerry, gItemIconPalette_RazzBerry},
    [ITEM_BLUK_BERRY] = {gItemIcon_BlukBerry, gItemIconPalette_BlukBerry},
    [ITEM_NANAB_BERRY] = {gItemIcon_NanabBerry, gItemIconPalette_NanabBerry},
    [ITEM_WEPEAR_BERRY] = {gItemIcon_WepearBerry, gItemIconPalette_WepearBerry},
    [ITEM_PINAP_BERRY] = {gItemIcon_PinapBerry, gItemIconPalette_PinapBerry},
    [ITEM_POMEG_BERRY] = {gItemIcon_PomegBerry, gItemIconPalette_PomegBerry},
    [ITEM_KELPSY_BERRY] = {gItemIcon_KelpsyBerry, gItemIconPalette_KelpsyBerry},
    [ITEM_QUALOT_BERRY] = {gItemIcon_QualotBerry, gItemIconPalette_QualotBerry},
    [ITEM_HONDEW_BERRY] = {gItemIcon_HondewBerry, gItemIconPalette_HondewBerry},
    [ITEM_GREPA_BERRY] = {gItemIcon_GrepaBerry, gItemIconPalette_GrepaBerry},
    [ITEM_TAMATO_BERRY] = {gItemIcon_TamatoBerry, gItemIconPalette_TamatoBerry},
    [ITEM_CORNN_BERRY] = {gItemIcon_CornnBerry, gItemIconPalette_CornnBerry},
    [ITEM_MAGOST_BERRY] = {gItemIcon_MagostBerry, gItemIconPalette_MagostBerry},
    [ITEM_RABUTA_BERRY] = {gItemIcon_RabutaBerry, gItemIconPalette_RabutaBerry},
    [ITEM_NOMEL_BERRY] = {gItemIcon_NomelBerry, gItemIconPalette_NomelBerry},
    [ITEM_SPELON_BERRY] = {gItemIcon_SpelonBerry, gItemIconPalette_SpelonBerry},
    [ITEM_PAMTRE_BERRY] = {gItemIcon_PamtreBerry, gItemIconPalette_PamtreBerry},
    [ITEM_WATMEL_BERRY] = {gItemIcon_WatmelBerry, gItemIconPalette_WatmelBerry},
    [ITEM_DURIN_BERRY] = {gItemIcon_DurinBerry, gItemIconPalette_DurinBerry},
    [ITEM_BELUE_BERRY] = {gItemIcon_BelueBerry, gItemIconPalette_BelueBerry},
    [ITEM_LIECHI_BERRY] = {gItemIcon_LiechiBerry, gItemIconPalette_LiechiBerry},
    [ITEM_GANLON_BERRY] = {gItemIcon_GanlonBerry, gItemIconPalette_GanlonBerry},
    [ITEM_SALAC_BERRY] = {gItemIcon_SalacBerry, gItemIconPalette_SalacBerry},
    [ITEM_PETAYA_BERRY] = {gItemIcon_PetayaBerry, gItemIconPalette_PetayaBerry},
    [ITEM_APICOT_BERRY] = {gItemIcon_ApicotBerry, gItemIconPalette_ApicotBerry},
    [ITEM_LANSAT_BERRY] = {gItemIcon_LansatBerry, gItemIconPalette_LansatBerry},
    [ITEM_STARF_BERRY] = {gItemIcon_StarfBerry, gItemIconPalette_StarfBerry},
    [ITEM_OCCA_BERRY] = {gItemIcon_OccaBerry, gItemIconPalette_OccaBerry},
    [ITEM_PASSHO_BERRY] = {gItemIcon_PasshoBerry, gItemIconPalette_PasshoBerry},
    [ITEM_WACAN_BERRY] = {gItemIcon_WacanBerry, gItemIconPalette_WacanBerry},
    [ITEM_RINDO_BERRY] = {gItemIcon_RindoBerry, gItemIconPalette_RindoBerry},
    [ITEM_YACHE_BERRY] = {gItemIcon_YacheBerry, gItemIconPalette_YacheBerry},
    [ITEM_CHOPLE_BERRY] = {gItemIcon_ChopleBerry, gItemIconPalette_ChopleBerry},
    [ITEM_KEBIA_BERRY] = {gItemIcon_KebiaBerry, gItemIconPalette_KebiaBerry},
    [ITEM_SHUCA_BERRY] = {gItemIcon_ShucaBerry, gItemIconPalette_ShucaBerry},
    [ITEM_COBA_BERRY] = {gItemIcon_CobaBerry, gItemIconPalette_CobaBerry},
    [ITEM_PAYAPA_BERRY] = {gItemIcon_PayapaBerry, gItemIconPalette_PayapaBerry},
    [ITEM_TANGA_BERRY] = {gItemIcon_TangaBerry, gItemIconPalette_TangaBerry},
    [ITEM_CHARTI_BERRY] = {gItemIcon_ChartiBerry, gItemIconPalette_ChartiBerry},
    [ITEM_KASIB_BERRY] = {gItemIcon_KasibBerry, gItemIconPalette_KasibBerry},
    [ITEM_HABAN_BERRY] = {gItemIcon_HabanBerry, gItemIconPalette_HabanBerry},
    [ITEM_COLBUR_BERRY] = {gItemIcon_ColburBerry, gItemIconPalette_ColburBerry},
    [ITEM_BABIRI_BERRY] = {gItemIcon_BabiriBerry, gItemIconPalette_BabiriBerry},
    [ITEM_CHILAN_BERRY] = {gItemIcon_ChilanBerry, gItemIconPalette_ChilanBerry},
    [ITEM_MICLE_BERRY] = {gItemIcon_MicleBerry, gItemIconPalette_MicleBerry},
    [ITEM_CUSTAP_BERRY] = {gItemIcon_CustapBerry, gItemIconPalette_CustapBerry},
    [ITEM_JABOCA_BERRY] = {gItemIcon_JabocaBerry, gItemIconPalette_JabocaBerry},
    [ITEM_ROWAP_BERRY] = {gItemIcon_RowapBerry, gItemIconPalette_RowapBerry},
    [ITEM_ROSELI_BERRY] = {gItemIcon_RoseliBerry, gItemIconPalette_RoseliBerry},
    [ITEM_KEE_BERRY] = {gItemIcon_KeeBerry, gItemIconPalette_KeeBerry},
    [ITEM_MARANGA_BERRY] = {gItemIcon_MarangaBerry, gItemIconPalette_MarangaBerry},
    [ITEM_ENIGMA_BERRY] = {gItemIcon_EnigmaBerry, gItemIconPalette_EnigmaBerry},
// Battle
    [ITEM_BRIGHT_POWDER] = {gItemIcon_BrightPowder, gItemIconPalette_BrightPowder},
    [ITEM_WHITE_HERB] = {gItemIcon_InBattleHerb, gItemIconPalette_WhiteHerb},
    [ITEM_MACHO_BRACE] = {gItemIcon_MachoBrace, gItemIconPalette_MachoBrace},
    [ITEM_EXP_SHARE] = {gItemIcon_ExpShare, gItemIconPalette_ExpShare},
    [ITEM_QUICK_CLAW] = {gItemIcon_QuickClaw, gItemIconPalette_QuickClaw},
    [ITEM_SOOTHE_BELL] = {gItemIcon_SootheBell, gItemIconPalette_SootheBell},
    [ITEM_MENTAL_HERB] = {gItemIcon_InBattleHerb, gItemIconPalette_MentalHerb},
    [ITEM_CHOICE_BAND] = {gItemIcon_ChoiceBand, gItemIconPalette_ChoiceBand},
    [ITEM_KINGS_ROCK] = {gItemIcon_KingsRock, gItemIconPalette_KingsRock},
    [ITEM_SILVER_POWDER] = {gItemIcon_SilverPowder, gItemIconPalette_SilverPowder},
    [ITEM_AMULET_COIN] = {gItemIcon_AmuletCoin, gItemIconPalette_AmuletCoin},
    [ITEM_CLEANSE_TAG] = {gItemIcon_CleanseTag, gItemIconPalette_CleanseTag},
    [ITEM_SOUL_DEW] = {gItemIcon_SoulDew, gItemIconPalette_SoulDew},
    [ITEM_DEEP_SEA_TOOTH] = {gItemIcon_DeepSeaTooth, gItemIconPalette_DeepSeaTooth},
    [ITEM_DEEP_SEA_SCALE] = {gItemIcon_DeepSeaScale, gItemIconPalette_DeepSeaScale},
    [ITEM_SMOKE_BALL] = {gItemIcon_SmokeBall, gItemIconPalette_SmokeBall},
    [ITEM_EVERSTONE] = {gItemIcon_Everstone, gItemIconPalette_Everstone},
    [ITEM_FOCUS_BAND] = {gItemIcon_FocusBand, gItemIconPalette_FocusBand},
    [ITEM_LUCKY_EGG] = {gItemIcon_LuckyEgg, gItemIconPalette_LuckyEgg},
    [ITEM_SCOPE_LENS] = {gItemIcon_ScopeLens, gItemIconPalette_ScopeLens},
    [ITEM_METAL_COAT] = {gItemIcon_MetalCoat, gItemIconPalette_MetalCoat},
    [ITEM_LEFTOVERS] = {gItemIcon_Leftovers, gItemIconPalette_Leftovers},
    [ITEM_DRAGON_SCALE] = {gItemIcon_DragonScale, gItemIconPalette_DragonScale},
    [ITEM_LIGHT_BALL] = {gItemIcon_LightBall, gItemIconPalette_LightBall},
    [ITEM_SOFT_SAND] = {gItemIcon_SoftSand, gItemIconPalette_SoftSand},
    [ITEM_HARD_STONE] = {gItemIcon_HardStone, gItemIconPalette_HardStone},
    [ITEM_MIRACLE_SEED] = {gItemIcon_MiracleSeed, gItemIconPalette_MiracleSeed},
    [ITEM_BLACK_GLASSES] = {gItemIcon_BlackGlasses, gItemIconPalette_BlackTypeEnhancingItem},
    [ITEM_BLACK_BELT] = {gItemIcon_BlackBelt, gItemIconPalette_BlackTypeEnhancingItem},
    [ITEM_MAGNET] = {gItemIcon_Magnet, gItemIconPalette_Magnet},
    [ITEM_MYSTIC_WATER] = {gItemIcon_MysticWater, gItemIconPalette_MysticWater},
    [ITEM_SHARP_BEAK] = {gItemIcon_SharpBeak, gItemIconPalette_SharpBeak},
    [ITEM_POISON_BARB] = {gItemIcon_PoisonBarb, gItemIconPalette_PoisonBarb},
    [ITEM_NEVER_MELT_ICE] = {gItemIcon_NeverMeltIce, gItemIconPalette_NeverMeltIce},
    [ITEM_SPELL_TAG] = {gItemIcon_SpellTag, gItemIconPalette_SpellTag},
    [ITEM_TWISTED_SPOON] = {gItemIcon_TwistedSpoon, gItemIconPalette_TwistedSpoon},
    [ITEM_CHARCOAL] = {gItemIcon_Charcoal, gItemIconPalette_Charcoal},
    [ITEM_DRAGON_FANG] = {gItemIcon_DragonFang, gItemIconPalette_DragonFang},
    [ITEM_SILK_SCARF] = {gItemIcon_SilkScarf, gItemIconPalette_SilkScarf},
    [ITEM_UP_GRADE] = {gItemIcon_UpGrade, gItemIconPalette_UpGrade},
    [ITEM_SHELL_BELL] = {gItemIcon_ShellBell, gItemIconPalette_Shell},
    [ITEM_SEA_INCENSE] = {gItemIcon_SeaIncense, gItemIconPalette_SeaIncense},
    [ITEM_LAX_INCENSE] = {gItemIcon_LaxIncense, gItemIconPalette_LaxIncense},
    [ITEM_LUCKY_PUNCH] = {gItemIcon_LuckyPunch, gItemIconPalette_LuckyPunch},
    [ITEM_METAL_POWDER] = {gItemIcon_MetalPowder, gItemIconPalette_MetalPowder},
    [ITEM_THICK_CLUB] = {gItemIcon_ThickClub, gItemIconPalette_ThickClub},
    [ITEM_STICK] = {gItemIcon_Stick, gItemIconPalette_Stick},
    [ITEM_ADAMANT_ORB] = {gItemIcon_AdamantOrb, gItemIconPalette_AdamantOrb},
    [ITEM_LUSTROUS_ORB] = {gItemIcon_LustrousOrb, gItemIconPalette_LustrousOrb},
    [ITEM_GRISEOUS_ORB] = {gItemIcon_GriseousOrb, gItemIconPalette_GriseousOrb},
    [ITEM_EXPERT_BELT] = {gItemIcon_ExpertBelt, gItemIconPalette_ExpertBelt},
    [ITEM_POWER_HERB] = {gItemIcon_PowerHerb, gItemIconPalette_PowerHerb},
    [ITEM_WIDE_LENS] = {gItemIcon_WideLens, gItemIconPalette_WideLens},
    [ITEM_ZOOM_LENS] = {gItemIcon_ZoomLens, gItemIconPalette_ZoomLens},
    [ITEM_DESTINY_KNOT] = {gItemIcon_DestinyKnot, gItemIconPalette_DestinyKnot},
    [ITEM_SMOOTH_ROCK] = {gItemIcon_SmoothRock, gItemIconPalette_SmoothRock},
    [ITEM_DAMP_ROCK] = {gItemIcon_DampRock, gItemIconPalette_DampRock},
    [ITEM_HEAT_ROCK] = {gItemIcon_HeatRock, gItemIconPalette_HeatRock},
    [ITEM_ICY_ROCK] = {gItemIcon_IcyRock, gItemIconPalette_IcyRock},
    [ITEM_BIG_ROOT] = {gItemIcon_BigRoot, gItemIconPalette_BigRoot},
    [ITEM_LIGHT_CLAY] = {gItemIcon_LightClay, gItemIconPalette_LightClay},
    [ITEM_SAFETY_GOGGLES] = {gItemIcon_SafetyGoggles, gItemIconPalette_SafetyGoggles},
    [ITEM_ROCKY_HELMET] = {gItemIcon_RockyHelmet, gItemIconPalette_RockyHelmet},
    [ITEM_WEAKNESS_POLICY] = {gItemIcon_WeaknessPolicy, gItemIconPalette_WeaknessPolicy},
    [ITEM_ASSAULT_VEST] = {gItemIcon_AssaultVest, gItemIconPalette_AssaultVest},
    [ITEM_EVIOLITE] = {gItemIcon_Eviolite, gItemIconPalette_Eviolite},
    [ITEM_ABSORB_BULB] = {gItemIcon_AbsorbBulb, gItemIconPalette_AbsorbBulb},
    [ITEM_AIR_BALLOON] = {gItemIcon_AirBalloon, gItemIconPalette_AirBalloon},
    [ITEM_ADRENALINE_ORB] = {gItemIcon_AdrenalineOrb, gItemIconPalette_AdrenalineOrb},
    [ITEM_BINDING_BAND] = {gItemIcon_BindingBand, gItemIconPalette_BindingBand},
    [ITEM_CELL_BATTERY] = {gItemIcon_CellBattery, gItemIconPalette_CellBattery},
    [ITEM_EJECT_BUTTON] = {gItemIcon_EjectButton, gItemIconPalette_EjectButton},
    [ITEM_FLOAT_STONE] = {gItemIcon_FloatStone, gItemIconPalette_FloatStone},
    [ITEM_FOCUS_SASH] = {gItemIcon_FocusSash, gItemIconPalette_FocusSash},
    [ITEM_GRIP_CLAW] = {gItemIcon_GripClaw, gItemIconPalette_GripClaw},
    [ITEM_IRON_BALL] = {gItemIcon_IronBall, gItemIconPalette_IronBall},
    [ITEM_LAGGING_TAIL] = {gItemIcon_LaggingTail, gItemIconPalette_LaggingTail},
    [ITEM_LUMINOUS_MOSS] = {gItemIcon_LuminousMoss, gItemIconPalette_LuminousMoss},
    [ITEM_QUICK_POWDER] = {gItemIcon_QuickPowder, gItemIconPalette_QuickPowder},
    [ITEM_METRONOME] = {gItemIcon_Metronome, gItemIconPalette_Metronome},
    [ITEM_MUSCLE_BAND] = {gItemIcon_MuscleBand, gItemIconPalette_MuscleBand},
    [ITEM_PROTECTIVE_PADS] = {gItemIcon_ProtectivePads, gItemIconPalette_ProtectivePads},
    [ITEM_RED_CARD] = {gItemIcon_RedCard, gItemIconPalette_RedCard},
    [ITEM_RING_TARGET] = {gItemIcon_RingTarget, gItemIconPalette_RingTarget},
    [ITEM_SHED_SHELL] = {gItemIcon_ShedShell, gItemIconPalette_ShedShell},
    [ITEM_SNOWBALL] = {gItemIcon_Snowball, gItemIconPalette_Snowball},
    [ITEM_STICKY_BARB] = {gItemIcon_StickyBarb, gItemIconPalette_StickyBarb},
    [ITEM_TERRAIN_EXTENDER] = {gItemIcon_TerrainExtender, gItemIconPalette_TerrainExtender},
    [ITEM_WISE_GLASSES] = {gItemIcon_WiseGlasses, gItemIconPalette_WiseGlasses},
    [ITEM_ELECTRIC_SEED] = {gItemIcon_ElectricSeed, gItemIconPalette_ElectricSeed},
    [ITEM_GRASSY_SEED] = {gItemIcon_GrassySeed, gItemIconPalette_GrassySeed},
    [ITEM_MISTY_SEED] = {gItemIcon_MistySeed, gItemIconPalette_MistySeed},
    [ITEM_PSYCHIC_SEED] = {gItemIcon_PsychicSeed, gItemIconPalette_PsychicSeed},
    [ITEM_LIFE_ORB] = {gItemIcon_LifeOrb, gItemIconPalette_LifeOrb},
    [ITEM_TOXIC_ORB] = {gItemIcon_ToxicOrb, gItemIconPalette_ToxicOrb},
    [ITEM_FLAME_ORB] = {gItemIcon_FlameOrb, gItemIconPalette_FlameOrb},
    [ITEM_BLACK_SLUDGE] = {gItemIcon_BlackSludge, gItemIconPalette_BlackSludge},
    [ITEM_CHOICE_SPECS] = {gItemIcon_ChoiceSpecs, gItemIconPalette_ChoiceSpecs},
    [ITEM_CHOICE_SCARF] = {gItemIcon_ChoiceScarf, gItemIconPalette_ChoiceScarf},
    [ITEM_EJECT_PACK] = {gItemIcon_EjectPack, gItemIconPalette_EjectPack},
    [ITEM_ROOM_SERVICE] = {gItemIcon_RoomService, gItemIconPalette_RoomService},
    [ITEM_BLUNDER_POLICY] = {gItemIcon_BlunderPolicy, gItemIconPalette_BlunderPolicy},
    [ITEM_HEAVY_DUTY_BOOTS] = {gItemIcon_HeavyDutyBoots, gItemIconPalette_HeavyDutyBoots},
    [ITEM_UTILITY_UMBRELLA] = {gItemIcon_UtilityUmbrella, gItemIconPalette_UtilityUmbrella},
    [ITEM_THROAT_SPRAY] = {gItemIcon_ThroatSpray, gItemIconPalette_ThroatSpray},
// Contest hold items
    [ITEM_RED_SCARF] = {gItemIcon_Scarf, gItemIconPalette_RedScarf},
    [ITEM_BLUE_SCARF] = {gItemIcon_Scarf, gItemIconPalette_BlueScarf},
    [ITEM_PINK_SCARF] = {gItemIcon_Scarf, gItemIconPalette_PinkScarf},
    [ITEM_GREEN_SCARF] = {gItemIcon_Scarf, gItemIconPalette_GreenScarf},
    [ITEM_YELLOW_SCARF] = {gItemIcon_Scarf, gItemIconPalette_YellowScarf},
// Key items
    [ITEM_MACH_BIKE] = {gItemIcon_MachBike, gItemIconPalette_MachBike},
    [ITEM_COIN_CASE] = {gItemIcon_CoinCase, gItemIconPalette_CoinCase},
    [ITEM_ITEMFINDER] = {gItemIcon_Itemfinder, gItemIconPalette_Itemfinder},
    [ITEM_OLD_ROD] = {gItemIcon_OldRod, gItemIconPalette_OldRod},
    [ITEM_GOOD_ROD] = {gItemIcon_GoodRod, gItemIconPalette_GoodRod},
    [ITEM_SUPER_ROD] = {gItemIcon_SuperRod, gItemIconPalette_SuperRod},
    [ITEM_SS_TICKET] = {gItemIcon_SSTicket, gItemIconPalette_SSTicket},
    [ITEM_CONTEST_PASS] = {gItemIcon_ContestPass, gItemIconPalette_ContestPass},
    [ITEM_WAILMER_PAIL] = {gItemIcon_WailmerPail, gItemIconPalette_WailmerPail},
    [ITEM_DEVON_GOODS] = {gItemIcon_DevonGoods, gItemIconPalette_DevonGoods},
    [ITEM_SOOT_SACK] = {gItemIcon_SootSack, gItemIconPalette_SootSack},
    [ITEM_BASEMENT_KEY] = {gItemIcon_BasementKey, gItemIconPalette_OldKey},
    [ITEM_ACRO_BIKE] = {gItemIcon_AcroBike, gItemIconPalette_AcroBike},
    [ITEM_POKEBLOCK_CASE] = {gItemIcon_PokeblockCase, gItemIconPalette_PokeblockCase},
    [ITEM_LETTER] = {gItemIcon_Letter, gItemIconPalette_LavaCookieAndLetter},
    [ITEM_EON_TICKET] = {gItemIcon_EonTicket, gItemIconPalette_EonTicket},
    [ITEM_RED_ORB] = {gItemIcon_Orb, gItemIconPalette_RedOrb},
    [ITEM_BLUE_ORB] = {gItemIcon_Orb, gItemIconPalette_BlueOrb},
    [ITEM_SCANNER] = {gItemIcon_Scanner, gItemIconPalette_Scanner},
    [ITEM_GO_GOGGLES] = {gItemIcon_GoGoggles, gItemIconPalette_GoGoggles},
    [ITEM_METEORITE] = {gItemIcon_Meteorite, gItemIconPalette_Meteorite},
    [ITEM_ROOM_1_KEY] = {gItemIcon_Room1Key, gItemIconPalette_Key},
    [ITEM_ROOM_2_KEY] = {gItemIcon_Room2Key, gItemIconPalette_Key},
    [ITEM_ROOM_4_KEY] = {gItemIcon_Room4Key, gItemIconPalette_Key},
    [ITEM_ROOM_6_KEY] = {gItemIcon_Room6Key, gItemIconPalette_Key},
    [ITEM_STORAGE_KEY] = {gItemIcon_StorageKey, gItemIconPalette_OldKey},
    [ITEM_ROOT_FOSSIL] = {gItemIcon_RootFossil, gItemIconPalette_HoennFossil},
    [ITEM_CLAW_FOSSIL] = {gItemIcon_ClawFossil, gItemIconPalette_HoennFossil},
    [ITEM_DEVON_SCOPE] = {gItemIcon_DevonScope, gItemIconPalette_DevonScope},
//Tm/Hm
    [ITEM_TM01] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM02] = {gItemIcon_TM, gItemIconPalette_DragonTMHM},
    [ITEM_TM03] = {gItemIcon_TM, gItemIconPalette_PsychicTMHM},
    [ITEM_TM04] = {gItemIcon_TM, gItemIconPalette_PsychicTMHM},
    [ITEM_TM05] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM06] = {gItemIcon_TM, gItemIconPalette_PoisonTMHM},
    [ITEM_TM07] = {gItemIcon_TM, gItemIconPalette_IceTMHM},
    [ITEM_TM08] = {gItemIcon_TM, gItemIconPalette_FightingTMHM},
    [ITEM_TM09] = {gItemIcon_TM, gItemIconPalette_PoisonTMHM},
    [ITEM_TM10] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM11] = {gItemIcon_TM, gItemIconPalette_FireTMHM},
    [ITEM_TM12] = {gItemIcon_TM, gItemIconPalette_DarkTMHM},
    [ITEM_TM13] = {gItemIcon_TM, gItemIconPalette_IceTMHM},
    [ITEM_TM14] = {gItemIcon_TM, gItemIconPalette_IceTMHM},
    [ITEM_TM15] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM16] = {gItemIcon_TM, gItemIconPalette_PsychicTMHM},
    [ITEM_TM17] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM18] = {gItemIcon_TM, gItemIconPalette_WaterTMHM},
    [ITEM_TM19] = {gItemIcon_TM, gItemIconPalette_FlyingTMHM},
    [ITEM_TM20] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM21] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM22] = {gItemIcon_TM, gItemIconPalette_GrassTMHM},
    [ITEM_TM23] = {gItemIcon_TM, gItemIconPalette_RockTMHM},
    [ITEM_TM24] = {gItemIcon_TM, gItemIconPalette_ElectricTMHM},
    [ITEM_TM25] = {gItemIcon_TM, gItemIconPalette_ElectricTMHM},
    [ITEM_TM26] = {gItemIcon_TM, gItemIconPalette_GroundTMHM},
    [ITEM_TM27] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM28] = {gItemIcon_TM, gItemIconPalette_BugTMHM},
    [ITEM_TM29] = {gItemIcon_TM, gItemIconPalette_PsychicTMHM},
    [ITEM_TM30] = {gItemIcon_TM, gItemIconPalette_GhostTMHM},
    [ITEM_TM31] = {gItemIcon_TM, gItemIconPalette_FightingTMHM},
    [ITEM_TM32] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM33] = {gItemIcon_TM, gItemIconPalette_PsychicTMHM},
    [ITEM_TM34] = {gItemIcon_TM, gItemIconPalette_PoisonTMHM},
    [ITEM_TM35] = {gItemIcon_TM, gItemIconPalette_FireTMHM},
    [ITEM_TM36] = {gItemIcon_TM, gItemIconPalette_PoisonTMHM},
    [ITEM_TM37] = {gItemIcon_TM, gItemIconPalette_RockTMHM},
    [ITEM_TM38] = {gItemIcon_TM, gItemIconPalette_FireTMHM},
    [ITEM_TM39] = {gItemIcon_TM, gItemIconPalette_RockTMHM},
    [ITEM_TM40] = {gItemIcon_TM, gItemIconPalette_FlyingTMHM},
    [ITEM_TM41] = {gItemIcon_TM, gItemIconPalette_DarkTMHM},
    [ITEM_TM42] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM43] = {gItemIcon_TM, gItemIconPalette_FireTMHM},
    [ITEM_TM44] = {gItemIcon_TM, gItemIconPalette_PsychicTMHM},
    [ITEM_TM45] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM46] = {gItemIcon_TM, gItemIconPalette_DarkTMHM},
    [ITEM_TM47] = {gItemIcon_TM, gItemIconPalette_FightingTMHM},
    [ITEM_TM48] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM49] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM50] = {gItemIcon_TM, gItemIconPalette_FireTMHM},
    [ITEM_TM51] = {gItemIcon_TM, gItemIconPalette_SteelTMHM},
    [ITEM_TM52] = {gItemIcon_TM, gItemIconPalette_FightingTMHM},
    [ITEM_TM53] = {gItemIcon_TM, gItemIconPalette_GrassTMHM},
    [ITEM_TM54] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM55] = {gItemIcon_TM, gItemIconPalette_WaterTMHM},
    [ITEM_TM56] = {gItemIcon_TM, gItemIconPalette_DarkTMHM},
    [ITEM_TM57] = {gItemIcon_TM, gItemIconPalette_ElectricTMHM},
    [ITEM_TM58] = {gItemIcon_TM, gItemIconPalette_FlyingTMHM},
    [ITEM_TM59] = {gItemIcon_TM, gItemIconPalette_DarkTMHM},
    [ITEM_TM60] = {gItemIcon_TM, gItemIconPalette_DarkTMHM},
    [ITEM_TM61] = {gItemIcon_TM, gItemIconPalette_FireTMHM},
    [ITEM_TM62] = {gItemIcon_TM, gItemIconPalette_FlyingTMHM},
    [ITEM_TM63] = {gItemIcon_TM, gItemIconPalette_DarkTMHM},
    [ITEM_TM64] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM65] = {gItemIcon_TM, gItemIconPalette_GhostTMHM},
    [ITEM_TM66] = {gItemIcon_TM, gItemIconPalette_DarkTMHM},
    [ITEM_TM67] = {gItemIcon_TM, gItemIconPalette_SteelTMHM},
    [ITEM_TM68] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM69] = {gItemIcon_TM, gItemIconPalette_RockTMHM},
    [ITEM_TM70] = {gItemIcon_TM, gItemIconPalette_IceTMHM},
    [ITEM_TM71] = {gItemIcon_TM, gItemIconPalette_RockTMHM},
    [ITEM_TM72] = {gItemIcon_TM, gItemIconPalette_ElectricTMHM},
    [ITEM_TM73] = {gItemIcon_TM, gItemIconPalette_ElectricTMHM},
    [ITEM_TM74] = {gItemIcon_TM, gItemIconPalette_SteelTMHM},
    [ITEM_TM75] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM76] = {gItemIcon_TM, gItemIconPalette_FlyingTMHM},
    [ITEM_TM77] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM78] = {gItemIcon_TM, gItemIconPalette_GroundTMHM},
    [ITEM_TM79] = {gItemIcon_TM, gItemIconPalette_IceTMHM},
    [ITEM_TM80] = {gItemIcon_TM, gItemIconPalette_RockTMHM},
    [ITEM_TM81] = {gItemIcon_TM, gItemIconPalette_BugTMHM},
    [ITEM_TM82] = {gItemIcon_TM, gItemIconPalette_DragonTMHM},
    [ITEM_TM83] = {gItemIcon_TM, gItemIconPalette_BugTMHM},
    [ITEM_TM84] = {gItemIcon_TM, gItemIconPalette_PoisonTMHM},
    [ITEM_TM85] = {gItemIcon_TM, gItemIconPalette_PsychicTMHM},
    [ITEM_TM86] = {gItemIcon_TM, gItemIconPalette_GrassTMHM},
    [ITEM_TM87] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM88] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM89] = {gItemIcon_TM, gItemIconPalette_BugTMHM},
    [ITEM_TM90] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM91] = {gItemIcon_TM, gItemIconPalette_SteelTMHM},
    [ITEM_TM92] = {gItemIcon_TM, gItemIconPalette_PsychicTMHM},
    [ITEM_TM93] = {gItemIcon_TM, gItemIconPalette_ElectricTMHM},
    [ITEM_TM94] = {gItemIcon_TM, gItemIconPalette_WaterTMHM},
    [ITEM_TM95] = {gItemIcon_TM, gItemIconPalette_DarkTMHM},
    [ITEM_TM96] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_TM97] = {gItemIcon_TM, gItemIconPalette_DarkTMHM},
    [ITEM_TM98] = {gItemIcon_TM, gItemIconPalette_WaterTMHM},
    [ITEM_TM99] = {gItemIcon_TM, gItemIconPalette_FairyTMHM},
    [ITEM_TM100] = {gItemIcon_TM, gItemIconPalette_NormalTMHM},
    [ITEM_HM01] = {gItemIcon_HM, gItemIconPalette_NormalTMHM},
    [ITEM_HM02] = {gItemIcon_HM, gItemIconPalette_FlyingTMHM},
    [ITEM_HM03] = {gItemIcon_HM, gItemIconPalette_WaterTMHM},
    [ITEM_HM04] = {gItemIcon_HM, gItemIconPalette_NormalTMHM},
    [ITEM_HM05] = {gItemIcon_HM, gItemIconPalette_NormalTMHM},
    [ITEM_HM06] = {gItemIcon_HM, gItemIconPalette_FightingTMHM},
    [ITEM_HM07] = {gItemIcon_HM, gItemIconPalette_WaterTMHM},
    [ITEM_HM08] = {gItemIcon_HM, gItemIconPalette_WaterTMHM},
//FRLG Key Items
    [ITEM_OAKS_PARCEL] = {gItemIcon_OaksParcel, gItemIconPalette_OaksParcel},
    [ITEM_POKE_FLUTE] = {gItemIcon_PokeFlute, gItemIconPalette_PokeFlute},
    [ITEM_SECRET_KEY] = {gItemIcon_SecretKey, gItemIconPalette_SecretKey},
    [ITEM_BIKE_VOUCHER] = {gItemIcon_BikeVoucher, gItemIconPalette_BikeVoucher},
    [ITEM_GOLD_TEETH] = {gItemIcon_GoldTeeth, gItemIconPalette_GoldTeeth},
    [ITEM_OLD_AMBER] = {gItemIcon_OldAmber, gItemIconPalette_OldAmber},
    [ITEM_CARD_KEY] = {gItemIcon_CardKey, gItemIconPalette_CardKey},
    [ITEM_LIFT_KEY] = {gItemIcon_LiftKey, gItemIconPalette_Key},
    [ITEM_HELIX_FOSSIL] = {gItemIcon_HelixFossil, gItemIconPalette_KantoFossil},
    [ITEM_DOME_FOSSIL] = {gItemIcon_DomeFossil, gItemIconPalette_KantoFossil},
    [ITEM_SILPH_SCOPE] = {gItemIcon_SilphScope, gItemIconPalette_SilphScope},
    [ITEM_BICYCLE] = {gItemIcon_Bicycle, gItemIconPalette_Bicycle},
    [ITEM_TOWN_MAP] = {gItemIcon_TownMap, gItemIconPalette_TownMap},
    [ITEM_VS_SEEKER] = {gItemIcon_VSSeeker, gItemIconPalette_VSSeeker},
    [ITEM_FAME_CHECKER] = {gItemIcon_FameChecker, gItemIconPalette_FameChecker},
    [ITEM_TM_CASE] = {gItemIcon_TMCase, gItemIconPalette_TMCase},
    [ITEM_BERRY_POUCH] = {gItemIcon_BerryPouch, gItemIconPalette_BerryPouch},
    [ITEM_TEACHY_TV] = {gItemIcon_TeachyTV, gItemIconPalette_TeachyTV},
    [ITEM_TRI_PASS] = {gItemIcon_TriPass, gItemIconPalette_TriPass},
    [ITEM_RAINBOW_PASS] = {gItemIcon_RainbowPass, gItemIconPalette_RainbowPass},
    [ITEM_TEA] = {gItemIcon_Tea, gItemIconPalette_Tea},
    [ITEM_MYSTIC_TICKET] = {gItemIcon_MysticTicket, gItemIconPalette_MysticTicket},
    [ITEM_AURORA_TICKET] = {gItemIcon_AuroraTicket, gItemIconPalette_AuroraTicket},
    [ITEM_POWDER_JAR] = {gItemIcon_PowderJar, gItemIconPalette_PowderJar},
    [ITEM_RUBY] = {gItemIcon_Gem, gItemIconPalette_Ruby},
    [ITEM_SAPPHIRE] = {gItemIcon_Gem, gItemIconPalette_Sapphire},
// Emerald-only key items
    //[ITEM_MAGMA_EMBLEM] = {gItemIcon_MagmaEmblem, gItemIconPalette_MagmaEmblem},
    //[ITEM_OLD_SEA_MAP] = {gItemIcon_OldSeaMap, gItemIconPalette_OldSeaMap},
// Return to field arrow
    [ITEM_N_A] = {gItemIcon_ReturnToFieldArrow, gItemIconPalette_ReturnToFieldArrow},
};

void ResetItemMenuIconState(void)
{
    u16 i;

    for (i = 0; i < NELEMS(sItemMenuIconSpriteIds); i++)
        sItemMenuIconSpriteIds[i] = 0xFF;
}

void CreateBagOrSatchelSprite(u8 animNum)
{
    sItemMenuIconSpriteIds[0] = CreateSprite(&sSpriteTemplate_BagOrSatchel, 40, 68, 0);
    sub_8098528(animNum);
}

void sub_8098528(u8 animNum)
{
    struct Sprite * sprite = &gSprites[sItemMenuIconSpriteIds[0]];
    sprite->pos2.y = -5;
    sprite->callback = sub_8098560;
    StartSpriteAnim(sprite, animNum);
}

static void sub_8098560(struct Sprite * sprite)
{
    if (sprite->pos2.y != 0)
        sprite->pos2.y++;
    else
        sprite->callback = SpriteCallbackDummy;
}

void sub_8098580(void)
{
    struct Sprite * sprite = &gSprites[sItemMenuIconSpriteIds[0]];
    if (sprite->affineAnimEnded)
    {
        StartSpriteAffineAnim(sprite, 1);
        sprite->callback = sub_80985BC;
    }
}

static void sub_80985BC(struct Sprite * sprite)
{
    if (sprite->affineAnimEnded)
    {
        StartSpriteAffineAnim(sprite, 0);
        sprite->callback = SpriteCallbackDummy;
    }
}

void ItemMenuIcons_CreateInsertIndicatorBarHidden(void)
{
    u8 i;
    u8 * ptr = &sItemMenuIconSpriteIds[1];

    for (i = 0; i < 9; i++)
    {
        ptr[i] = CreateSprite(&gUnknown_83D4250, i * 16 + 0x60, 7, 0);
        switch (i)
        {
        case 0:
            break;
        case 8:
            StartSpriteAnim(&gSprites[ptr[i]], 2);
            break;
        default:
            StartSpriteAnim(&gSprites[ptr[i]], 1);
            break;
        }
        gSprites[ptr[i]].invisible = TRUE;
    }
}

void ItemMenuIcons_ToggleInsertIndicatorBarVisibility(bool8 invisible)
{
    u8 i;
    u8 * ptr = &sItemMenuIconSpriteIds[1];

    for (i = 0; i < 9; i++)
    {
        gSprites[ptr[i]].invisible = invisible;
    }
}

void ItemMenuIcons_MoveInsertIndicatorBar(s16 x, u16 y)
{
    u8 i;
    u8 * ptr = &sItemMenuIconSpriteIds[1];

    for (i = 0; i < 9; i++)
    {
        gSprites[ptr[i]].pos2.x = x;
        gSprites[ptr[i]].pos1.y = y + 7;
    }
}

static bool8 TryAllocItemIconTilesBuffers(void)
{
    void ** ptr1, ** ptr2;

    ptr1 = &sItemIconTilesBuffer;
    *ptr1 = Alloc(0x120);
    if (*ptr1 == NULL)
        return FALSE;
    ptr2 = &sItemIconTilesBufferPadded;
    *ptr2 = AllocZeroed(0x200);
    if (*ptr2 == NULL)
    {
        Free(*ptr1);
        return FALSE;
    }
    return TRUE;
}

void CopyItemIconPicTo4x4Buffer(const void * src, void * dest)
{
    u8 i;

    for (i = 0; i < 3; i++)
    {
        CpuCopy16(src + 0x60 * i, dest + 0x80 * i, 0x60);
    }
}

u8 AddItemIconObject(u16 tilesTag, u16 paletteTag, u16 itemId)
{
    struct SpriteTemplate template;
    struct SpriteSheet spriteSheet;
    struct CompressedSpritePalette spritePalette;
    u8 spriteId;

    if (!TryAllocItemIconTilesBuffers())
        return MAX_SPRITES;

    LZDecompressWram(GetItemIconGfxPtr(itemId, 0), sItemIconTilesBuffer);
    CopyItemIconPicTo4x4Buffer(sItemIconTilesBuffer, sItemIconTilesBufferPadded);
    spriteSheet.data = sItemIconTilesBufferPadded;
    spriteSheet.size = 0x200;
    spriteSheet.tag = tilesTag;
    LoadSpriteSheet(&spriteSheet);

    spritePalette.data = GetItemIconGfxPtr(itemId, 1);
    spritePalette.tag = paletteTag;
    LoadCompressedSpritePalette(&spritePalette);

    CpuCopy16(&sSpriteTemplate_ItemIcon, &template, sizeof(struct SpriteTemplate));
    template.tileTag = tilesTag;
    template.paletteTag = paletteTag;
    spriteId = CreateSprite(&template, 0, 0, 0);

    Free(sItemIconTilesBuffer);
    Free(sItemIconTilesBufferPadded);
    return spriteId;
}

u8 AddItemIconObjectWithCustomObjectTemplate(const struct SpriteTemplate * origTemplate, u16 tilesTag, u16 paletteTag, u16 itemId)
{
    struct SpriteTemplate template;
    struct SpriteSheet spriteSheet;
    struct CompressedSpritePalette spritePalette;
    u8 spriteId;

    if (!TryAllocItemIconTilesBuffers())
        return MAX_SPRITES;

    LZDecompressWram(GetItemIconGfxPtr(itemId, 0), sItemIconTilesBuffer);
    CopyItemIconPicTo4x4Buffer(sItemIconTilesBuffer, sItemIconTilesBufferPadded);
    spriteSheet.data = sItemIconTilesBufferPadded;
    spriteSheet.size = 0x200;
    spriteSheet.tag = tilesTag;
    LoadSpriteSheet(&spriteSheet);

    spritePalette.data = GetItemIconGfxPtr(itemId, 1);
    spritePalette.tag = paletteTag;
    LoadCompressedSpritePalette(&spritePalette);

    CpuCopy16(origTemplate, &template, sizeof(struct SpriteTemplate));
    template.tileTag = tilesTag;
    template.paletteTag = paletteTag;
    spriteId = CreateSprite(&template, 0, 0, 0);

    Free(sItemIconTilesBuffer);
    Free(sItemIconTilesBufferPadded);
    return spriteId;
}

void CreateItemMenuIcon(u16 itemId, u8 idx)
{
    u8 * ptr = &sItemMenuIconSpriteIds[10];
    u8 spriteId;

    if (ptr[idx] == 0xFF)
    {
        FreeSpriteTilesByTag(102 + idx);
        FreeSpritePaletteByTag(102 + idx);
        spriteId = AddItemIconObject(102 + idx, 102 + idx, itemId);
        if (spriteId != MAX_SPRITES)
        {
            ptr[idx] = spriteId;
            gSprites[spriteId].pos2.x = 24;
            gSprites[spriteId].pos2.y = 140;
        }
    }
}

void DestroyItemMenuIcon(u8 idx)
{
    u8 * ptr = &sItemMenuIconSpriteIds[10];

    if (ptr[idx] != 0xFF)
    {
        DestroySpriteAndFreeResources(&gSprites[ptr[idx]]);
        ptr[idx] = 0xFF;
    }
}

const void * GetItemIconGfxPtr(u16 itemId, u8 attrId)
{
    if (itemId > ITEM_N_A)
        itemId = ITEM_NONE;
    return sItemIconGfxPtrs[itemId][attrId];
}

void sub_80989A0(u16 itemId, u8 idx)
{
    u8 * ptr = &sItemMenuIconSpriteIds[10];
    u8 spriteId;

    if (ptr[idx] == 0xFF)
    {
        FreeSpriteTilesByTag(102 + idx);
        FreeSpritePaletteByTag(102 + idx);
        spriteId = AddItemIconObject(102 + idx, 102 + idx, itemId);
        if (spriteId != MAX_SPRITES)
        {
            ptr[idx] = spriteId;
            gSprites[spriteId].pos2.x = 24;
            gSprites[spriteId].pos2.y = 147;
        }
    }
}
