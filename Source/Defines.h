#ifndef ___DEFINES___
#define ___DEFINES___

// Bits
#define BIN__N( x )                           ( x ) | x >> 3 | x >> 6 | x >> 9
#define BIN__B( x )                           ( x ) & 0xf | ( x ) >> 12 & 0xf0
#define BIN8( v )                             ( BIN__B( BIN__N( 0x ## v ) ) )
#define BIN16( bin16, bin8 )                  ( ( BIN8( bin16 ) << 8 ) | ( BIN8( bin8 ) ) )
#define BIN32( bin32, bin24, bin16, bin8 )    ( ( BIN8( bin32 ) << 24 ) | ( BIN8( bin24 ) << 16 ) | ( BIN8( bin16 ) << 8 ) | ( BIN8( bin8 ) ) )

// Flags
#define FLAG( x, y )                          ( ( ( x ) & ( y ) ) != 0 )
#define FLAGS( x, y )                         ( ( ( x ) & ( y ) ) == y )
#define SETFLAG( x, y )                       ( ( x ) = ( x ) | ( y ) )
#define UNSETFLAG( x, y )                     ( ( x ) = ( ( x ) | ( y ) ) ^ ( y ) )

// Limits
#define MAX_UCHAR                    ( 0xFF )
#define MAX_USHORT                   ( 0xFFFF )
#define MAX_UINT                     ( 0xFFFFFFFF )
#define MAX_INT                      ( 0x7FFFFFFF )
#define MIN_INT                      ( 0x80000000 )

// Other stuff
#define CLAMP( x, low, high )                 ( ( ( x ) > ( high ) ) ? ( high ) : ( ( ( x ) < ( low ) ) ? ( low ) : ( x ) ) )
#define CONVERT_GRAMM( x )                    ( ( x ) * 453 )
#define RAD( deg )                            ( ( deg ) * 3.141592654f / 180.0f )

// World dump versions
#define WORLD_SAVE_V1                ( 0x01AB0F01 )
#define WORLD_SAVE_V2                ( 0x01AB0F02 )
#define WORLD_SAVE_V3                ( 0x01AB0F03 )
#define WORLD_SAVE_V4                ( 0x01AB0F04 )
#define WORLD_SAVE_V5                ( 0x01AB0F05 )
#define WORLD_SAVE_V6                ( 0x01AB0F06 )
#define WORLD_SAVE_V7                ( 0x01AB0F07 )
#define WORLD_SAVE_V8                ( 0x01AB0F08 )
#define WORLD_SAVE_V9                ( 0x01AB0F09 )
#define WORLD_SAVE_V10               ( 0x01AB0F10 )
#define WORLD_SAVE_V11               ( 0x01AB0F11 )
#define WORLD_SAVE_V12               ( 0x01AB0F12 )
#define WORLD_SAVE_V13               ( 0x01AB0F13 )
#define WORLD_SAVE_LAST              WORLD_SAVE_V13
#define SINGLEPLAYER_SAVE_V1         ( 1 )
#define SINGLEPLAYER_SAVE_V2         ( 2 )
#define SINGLEPLAYER_SAVE_LAST       ( SINGLEPLAYER_SAVE_V2 )

// Client save
#define CLIENT_SAVE_V1               ( 1 ) // Corrupted
#define CLIENT_SAVE_V2               ( 2 )
#define CLIENT_SAVE_LAST             ( CLIENT_SAVE_V2 )
const char ClientSaveSignature[ 4 ] = { 'F', 'O', 0, CLIENT_SAVE_LAST };

// Generic
#define WORLD_START_TIME             "07:00 30:10:2246 x00"
#define MAX_FOPATH                   UTF8_BUF_SIZE( 1024 )
#define CRAFT_SEND_TIME              ( 60000 )
#define LEXEMS_SIZE                  ( 128 )
#define MAX_HOLO_INFO                ( 250 )
#define MAX_PARAMETERS_ARRAYS        ( 100 )
#define AMBIENT_SOUND_TIME           ( 60000 ) // Random(X/2,X);
#define EFFECT_TEXTURES              ( 10 )
#define EFFECT_SCRIPT_VALUES         ( 10 )
#define ABC_SIZE                     ( 26 )
#define DIRS_COUNT                   ( GameOpt.MapHexagonal ? 6 : 8 )
#define IS_DIR_CORNER( dir )                  ( ( ( dir ) & 1 ) != 0 ) // 1, 3, 5, 7
#define UTF8_BUF_SIZE( count )                ( ( count ) * 4 )

// Script pragma bindfield sizes
#define PROTO_ITEM_USER_DATA_SIZE    ( 500 )
#define CRITTER_USER_DATA_SIZE       ( 400 )

// Critters
#define GENDER_MALE                  ( 0 )
#define GENDER_FEMALE                ( 1 )
#define GENDER_IT                    ( 2 )
#define AGE_MAX                      ( 60 )
#define AGE_MIN                      ( 14 )
#define AGGRESSOR_TICK               ( 60000 )
#define MAX_ENEMY_STACK              ( 30 )
#define MAX_STORED_IP                ( 20 )

// Items
#define MAX_ADDED_NOGROUP_ITEMS      ( 30 )
#define ITEM_SLOT_BEGIN              ( 1000 )
#define ITEM_SLOT_END                ( 1099 )
#define ITEM_DEF_SLOT                ( 1000 )
#define ITEM_DEF_ARMOR               ( 1100 )
#define UNARMED_PUNCH                ( 1000 )
#define UNARMED_KICK                 ( 1020 )

// SceneryCl flags
#define SCEN_CAN_USE                 ( 0x01 )
#define SCEN_CAN_TALK                ( 0x02 )

// Maps
#define TIME_CAN_FOLLOW_GM           ( 20000 ) // Can less than Map timeout

// Critter find types
#define FIND_LIFE                    ( 0x01 )
#define FIND_KO                      ( 0x02 )
#define FIND_DEAD                    ( 0x04 )
#define FIND_ONLY_PLAYERS            ( 0x10 )
#define FIND_ONLY_NPC                ( 0x20 )
#define FIND_ALL                     ( 0x0F )

// Proto maps
#define MAP_PROTO_EXT                ".fomap"
#define MAX_PROTO_MAPS               ( 30000 )

// Type entires
#define ENTIRE_DEFAULT               ( 0 )
#define ENTIRE_LOG_OUT               ( 241 )

// Sendmap info
#define SENDMAP_TILES                BIN8( 00000001 )
#define SENDMAP_WALLS                BIN8( 00000010 )
#define SENDMAP_SCENERY              BIN8( 00000100 )

// Ping
enum PING_TYPES
{
	PING_PING = 0,
	PING_WAIT,
	PING_CLIENT,
	PING_UID_FAIL
};

// Say types
enum SAY_TYPES
{
	SAY_NORM = 1,
	SAY_NORM_ON_HEAD,
	SAY_SHOUT,
	SAY_SHOUT_ON_HEAD,
	SAY_EMOTE,
	SAY_EMOTE_ON_HEAD,
	SAY_WHISP,
	SAY_WHISP_ON_HEAD,
	SAY_SOCIAL,
	SAY_RADIO,
	SAY_NETMSG,
	SAY_DIALOG,
	SAY_APPEND,
	SAY_ENCOUNTER_ANY,
	SAY_ENCOUNTER_RT,
	SAY_ENCOUNTER_TB,
	SAY_FIX_RESULT,
	SAY_DIALOGBOX_TEXT,
	SAY_SAY_TITLE = 39,
	SAY_SAY_TEXT,
	SAY_FLASH_WINDOW
};
#define SAY_DIALOGBOX_BUTTON( b )             ( 19 + ( b ) ) // Max 20 buttons (0..19)

#define MAX_DLGBOX_BUTTONS           ( 20 )

// Transfer types
enum TRANSFER_TYPES
{
	TRANSFER_CLOSE = 0,
	TRANSFER_HEX_CONT_UP,
	TRANSFER_HEX_CONT_DOWN,
	TRANSFER_SELF_CONT,
	TRANSFER_CRIT_LOOT,
	TRANSFER_CRIT_STEAL,
	TRANSFER_CRIT_BARTER,
	TRANSFER_FAR_CONT,
	TRANSFER_FAR_CRIT
};

// Take flags
enum TAKE_FLAGS
{
	CONT_GET = 1,
	CONT_PUT,
	CONT_GETALL,
	CONT_PUTALL
};

// Target types
enum TARGET_TYPES
{
	TARGET_SELF = 0,
	TARGET_SELF_ITEM,
	TARGET_CRITTER,
	TARGET_ITEM,
	TARGET_SCENERY,
	TARGET_HEX			// Todo:
};

// Pick types
enum PICK_TYPES
{
	PICK_CRIT_LOOT = 0,
	PICK_CRIT_PUSH
};

// Craft results
enum CRAFT_RESULTS
{
	CRAFT_RESULT_NONE = 0,
	CRAFT_RESULT_SUCC,
	CRAFT_RESULT_FAIL,
	CRAFT_RESULT_TIMEOUT
};

// Critters
#define CRITTER_INV_VOLUME           ( 1000 )

// Locker
#define LOCKER_ISOPEN                ( 0x01 )
#define LOCKER_NOOPEN                ( 0x10 )

// Hit locations
enum HIT_LOCATION_TYPES
{
	HIT_LOCATION_NONE = 0,
	HIT_LOCATION_HEAD,
	HIT_LOCATION_LEFT_ARM,
	HIT_LOCATION_RIGHT_ARM,
	HIT_LOCATION_TORSO,
	HIT_LOCATION_RIGHT_LEG,
	HIT_LOCATION_LEFT_LEG,
	HIT_LOCATION_EYES,
	HIT_LOCATION_GROIN,
	HIT_LOCATION_UNCALLED,

	MAX_HIT_LOCATION
};

// Locations
#define MAX_PROTO_LOCATIONS          ( 30000 )

// Global map
#define GM_MAXX                      ( GameOpt.GlobalMapWidth * GameOpt.GlobalMapZoneLength )
#define GM_MAXY                      ( GameOpt.GlobalMapHeight * GameOpt.GlobalMapZoneLength )
#define GM_ZONE_LEN                  ( GameOpt.GlobalMapZoneLength ) // Can be multiple to GM_MAXX and GM_MAXY
#define GM__MAXZONEX                 ( 100 )
#define GM__MAXZONEY                 ( 100 )
#define GM_ZONES_FOG_SIZE            ( ( ( GM__MAXZONEX / 4 ) + ( ( GM__MAXZONEX % 4 ) ? 1 : 0 ) ) * GM__MAXZONEY )
enum GM_FOG_TYPES
{
	GM_FOG_FULL = 0,
	GM_FOG_HALF,
	GM_FOG_HALF_EX,
	GM_FOG_NONE
};
#define GM_MAX_GROUP_COUNT           ( GameOpt.GlobalMapMaxGroupCount )
#define GM_ANSWER_WAIT_TIME          ( 20000 )
#define GM_LIGHT_TIME                ( 5000 )
#define GM_ZONE( x )                          ( ( x ) / GM_ZONE_LEN )
#define GM_ENTRANCES_SEND_TIME       ( 60000 )
#define GM_TRACE_TIME                ( 1000 )

// Follow
#define FOLLOW_DIST                  ( 10 )
#define FOLLOW_FORCE                 ( 1 )
#define FOLLOW_PREP                  ( 2 )

// GM Info
#define GM_INFO_LOCATIONS            ( 0x01 )
#define GM_INFO_CRITTERS             ( 0x02 )
#define GM_INFO_GROUP_PARAM          ( 0x04 )
#define GM_INFO_ZONES_FOG            ( 0x08 )
#define GM_INFO_ALL                  ( 0x0F )
#define GM_INFO_FOG                  ( 0x10 )
#define GM_INFO_LOCATION             ( 0x20 )

// Global process types
enum GLOBAL_PROCESS_TYPES
{
	GLOBAL_PROCESS_MOVE = 0,
	GLOBAL_PROCESS_ENTER,
	GLOBAL_PROCESS_START_FAST,
	GLOBAL_PROCESS_START,
	GLOBAL_PROCESS_SET_MOVE,
	GLOBAL_PROCESS_STOPPED,
	GLOBAL_PROCESS_NPC_IDLE,
	GLOBAL_PROCESS_KICK
};

// GM Rule command
enum GM_CMD_TYPES
{
	GM_CMD_SETMOVE = 1,
	GM_CMD_STOP,
	GM_CMD_TOLOCAL,
	GM_CMD_KICKCRIT,
	GM_CMD_FOLLOW_CRIT,
	GM_CMD_FOLLOW,
	GM_CMD_GIVE_RULE,
	GM_CMD_ANSWER,
	GM_CMD_ENTRANCES,
	GM_CMD_VIEW_MAP
};

// GM Walk types
enum GM_WALK_TYPES
{
	GM_WALK_GROUND = 0,
	GM_WALK_FLY,
	GM_WALK_WATER
};

// Flags Hex
// Proto map
#define FH_BLOCK                     BIN8( 00000001 )
#define FH_NOTRAKE                   BIN8( 00000010 )
#define FH_WALL                      BIN8( 00000100 )
#define FH_SCEN                      BIN8( 00001000 )
#define FH_SCEN_GRID                 BIN8( 00010000 )
#define FH_TRIGGER                   BIN8( 00100000 )
// Map copy
#define FH_CRITTER                   BIN8( 00000001 )
#define FH_DEAD_CRITTER              BIN8( 00000010 )
#define FH_ITEM                      BIN8( 00000100 )
#define FH_DOOR                      BIN8( 00001000 )
#define FH_BLOCK_ITEM                BIN8( 00010000 )
#define FH_NRAKE_ITEM                BIN8( 00100000 )
#define FH_WALK_ITEM                 BIN8( 01000000 )
#define FH_GAG_ITEM                  BIN8( 10000000 )

#define FH_NOWAY                     BIN16( 00010001, 00000001 )
#define FH_NOSHOOT                   BIN16( 00100000, 00000010 )

// Client map
#define SERVER_MAP_EXT               ".map"
#define CLIENT_MAP_FORMAT_VER        ( 7 )

// Coordinates
#define MAXHEX_DEF                   ( 200 )
#define MAXHEX_MIN                   ( 10 )
#define MAXHEX_MAX                   ( 10000 )

// Client parameters
#define MAX_NAME                     ( 30 )
#define MIN_NAME                     ( 1 )
#define MAX_CHAT_MESSAGE             ( 75 )
#define MAX_SAY_NPC_TEXT             ( 25 )
#define MAX_SCENERY                  ( 5000 )
#define MAX_DIALOG_TEXT              ( MAX_FOTEXT )
#define MAX_DLG_LEN_IN_BYTES         ( 64 * 1024 )
#define MAX_DLG_LEXEMS_TEXT          ( 1000 )
#define MAX_BUF_LEN                  ( 4096 )
#define PASS_HASH_SIZE               ( 32 )

// Critters
#define MAX_CRIT_TYPES               ( 1000 )
#define NPC_START_ID                 ( 5000001 )
#define USERS_START_ID               ( 1 )
#define IS_USER_ID( id )                      ( ( id ) > 0 && ( id ) < NPC_START_ID )
#define IS_NPC_ID( id )                       ( ( id ) >= NPC_START_ID )
#define MAX_ANSWERS                  ( 100 )
#define PROCESS_TALK_TICK            ( 1000 )
#define DIALOGS_LST_NAME             "dialogs.lst"
#define MAX_SCRIPT_NAME              ( 64 )
#define SCRIPTS_LST                  "scripts.cfg"
#define MAX_START_SPECIAL            ( 40 )
#define TURN_BASED_TIMEOUT           ( 1000 )
#define MIN_VISIBLE_CRIT             ( 6 )
#define FADING_PERIOD                ( 1000 )

#define RESPOWN_TIME_PLAYER          ( 3 )
#define RESPOWN_TIME_NPC             ( 120 )
#define RESPOWN_TIME_INFINITY        ( 4 * 24 * 60 * 60000 )

// Combat modes
enum COMBAT_MODES
{
	COMBAT_MODE_ANY = 0,
	COMBAT_MODE_REAL_TIME,
	COMBAT_MODE_TURN_BASED
};

// Turn based
enum COMBAT_TB_ACTIONS
{
	COMBAT_TB_END_TURN = 0,
	COMBAT_TB_END_COMBAT
};

// Answer
#define ANSWER_BEGIN                 ( 0xF0 )
#define ANSWER_END                   ( 0xF1 )
#define ANSWER_BARTER                ( 0xF2 )

// Time AP
#define AP_DIVIDER                   ( 100 )

// Crit conditions
enum CRIT_CONDITIONS
{
	COND_LIFE = 1,
	COND_KNOCKOUT,
	COND_DEAD
};
// Run-time critters flags
#define FCRIT_PLAYER                 ( 0x00010000 ) // Player
#define FCRIT_NPC                    ( 0x00020000 ) // Npc
#define FCRIT_DISCONNECT             ( 0x00080000 ) // In offline
#define FCRIT_CHOSEN                 ( 0x00100000 ) // Chosen
#define FCRIT_RULEGROUP              ( 0x00200000 ) // Group rule

// Slots
enum SLOT_TYPES
{
	SLOT_INV = 0,
	SLOT_HAND1,
	SLOT_HAND2,
	SLOT_ARMOR,

	SLOT_GROUND = 255
};

// Players barter
#define BARTER_DIST                  ( 1 )
// Types
enum BARTER_TYPES
{
	BARTER_TRY = 0,					// opponentId, isHide
	BARTER_ACCEPTED,				// opponentId, isHide
	BARTER_BEGIN,
	BARTER_END,
	BARTER_SET_SELF,
	BARTER_SET_OPPONENT,
	BARTER_UNSET_SELF,
	BARTER_UNSET_OPPONENT,
	BARTER_OFFER,					// isSet, isOpponent
	BARTER_REFRESH
};

// Scores
#define SCORE_SPEAKER                ( 3 )
#define SCORE_TRADER                 ( 4 )
#define SCORES_MAX                   ( 50 )
#define SCORES_SEND_TIME             ( 60000 )
#define SCORE_NAME_LEN               ( 64 )

// Show screen modes
// Ouput: it is 'uint param' in Critter::ShowScreen.
// Input: I - integer value 'uint answerI', S - string value 'string& answerS' in 'answer_' function.
enum SHOW_SCREEN_MODES
{
	SHOW_SCREEN_CLOSE = 0,					// Close top window.
	SHOW_SCREEN_TIMER,						// Timer box. Output: picture index in INVEN.LST. Input I: time in game minutes (1..599).
	SHOW_SCREEN_DIALOGBOX,					// Dialog box. Output: buttons count - 0..20 (exit button added automatically). Input I: Choosed button - 0..19.
	SHOW_SCREEN_SKILLBOX,					// Skill box. Input I: selected skill.
	SHOW_SCREEN_BAG,						// Bag box. Input I: id of selected item.
	SHOW_SCREEN_SAY,						// Say box. Output: all symbols - 0 or only numbers - any other number. Input S: typed string.
	SHOW_ELEVATOR,							// Elevator. Output: look ELEVATOR_* macro. Input I: Choosed level button.
	SHOW_SCREEN_INVENTORY,					// Inventory.
	SHOW_SCREEN_CHARACTER,					// Character.
	SHOW_SCREEN_FIXBOY,						// Fix-boy.
	SHOW_SCREEN_PIPBOY,						// Pip-boy.
	SHOW_SCREEN_MINIMAP						// Mini-map.
};

// Parameters
#define MAX_PARAMS                   ( 1000 )
#define SKILL_OFFSET( skill )                 ( ( skill ) - ( GameOpt.AbsoluteOffsets ? 0 : SKILL_BEGIN ) )
#define PERK_OFFSET( perk )                   ( ( perk ) - ( GameOpt.AbsoluteOffsets ? 0 : PERK_BEGIN ) )

enum HARDCODE_PARAMS
{
	// Stats
	ST_STRENGTH = 0,
	ST_PERCEPTION,
	ST_ENDURANCE,
	ST_CHARISMA,
	ST_INTELLECT,
	ST_AGILITY,
	ST_LUCK,
	ST_MAX_LIFE,
	ST_ACTION_POINTS,
	ST_ARMOR_CLASS,
	ST_MELEE_DAMAGE,
	ST_CARRY_WEIGHT,
	ST_SEQUENCE,
	ST_HEALING_RATE,
	ST_CRITICAL_CHANCE,
	ST_NORMAL_RESIST = 23,
	ST_RADIATION_RESISTANCE = 30,
	ST_POISON_RESISTANCE,
	ST_AGE = 70,
	ST_GENDER,
	ST_CURRENT_HP,
	ST_POISONING_LEVEL,
	ST_RADIATION_LEVEL,
	ST_CURRENT_AP,
	ST_EXPERIENCE,
	ST_LEVEL,
	ST_UNSPENT_SKILL_POINTS,
	ST_UNSPENT_PERKS,
	ST_KARMA,
	ST_FOLLOW_CRIT,
	ST_REPLICATION_MONEY,
	ST_REPLICATION_COUNT,
	ST_REPLICATION_TIME,
	ST_REPLICATION_COST,
	ST_TURN_BASED_AC,
	ST_MAX_MOVE_AP,
	ST_MOVE_AP,
	ST_NPC_ROLE,
	ST_BONUS_LOOK = 101,
	ST_HANDS_ITEM_AND_MODE,
	ST_FREE_BARTER_PLAYER,
	ST_DIALOG_ID,
	ST_AI_ID,
	ST_TEAM_ID,
	ST_BAG_ID,
	ST_LAST_WEAPON_ID = 110,
	ST_BASE_CRTYPE = 112,
	ST_TALK_DISTANCE = 115,
	ST_SCALE_FACTOR,
	ST_WALK_TIME,
	ST_RUN_TIME,
	ST_MAX_TALKERS,
	// Skills
	SK_UNARMED = 203,
	SK_FIRST_AID = 206,
	SK_DOCTOR,
	SK_SNEAK,
	SK_LOCKPICK,
	SK_STEAL,
	SK_TRAPS,
	SK_SCIENCE,
	SK_REPAIR,
	SK_SPEECH,
	SK_BARTER,
	// Tag skills
	TAG_SKILL1 = 226,
	TAG_SKILL2,
	TAG_SKILL3,
	TAG_SKILL4,
	// Timeouts
	TO_SK_REPAIR = 232,
	TO_SK_SCIENCE,
	TO_BATTLE = 238,
	TO_TRANSFER,
	TO_REMOVE_FROM_GAME,
	TO_KARMA_VOTING = 242,
	// Perks
	PE_SILENT_RUNNING = 316,
	PE_MASTER_TRADER = 318,
	PE_QUICK_POCKETS = 349,
	PE_SMOOTH_TALKER,
	// Damages
	DAMAGE_POISONED = 500,
	DAMAGE_RADIATED,
	DAMAGE_RIGHT_ARM = 503,
	DAMAGE_LEFT_ARM,
	DAMAGE_RIGHT_LEG,
	DAMAGE_LEFT_LEG,
	// Modes
	MODE_HIDE = 510,
	MODE_NO_STEAL,
	MODE_NO_BARTER,
	MODE_NO_ENEMY_STACK,
	MODE_NO_PVP,
	MODE_END_COMBAT,
	MODE_DEFAULT_COMBAT,
	MODE_NO_HOME,
	MODE_GECK,
	MODE_NO_FAVORITE_ITEM,
	MODE_NO_ITEM_GARBAGER,
	MODE_DLG_SCRIPT_BARTER,
	MODE_UNLIMITED_AMMO,
	MODE_INVULNERABLE = 527,
	MODE_NO_FLATTEN,			 // Client
	MODE_RANGE_HTH = 530,
	MODE_NO_LOOT = 532,
	MODE_NO_PUSH = 536,
	MODE_NO_UNARMED,
	MODE_NO_AIM,
	MODE_NO_WALK,
	MODE_NO_RUN,
	MODE_NO_TALK,
	MODE_PROCESS_ON_GLOB = 544
};

#define ST_ANIM3D_LAYER_BEGIN        ( 150 )
#define ST_ANIM3D_LAYER_END			 ( 179 )

// Skills
#define SKILL_BEGIN                  ( GameOpt.SkillBegin )
#define SKILL_END                    ( GameOpt.SkillEnd )
#define SKILL_COUNT                  ( SKILL_END - SKILL_BEGIN + 1 )
#define MAX_SKILL_VAL                ( GameOpt.SkillMaxValue )

// Timeouts
#define TIMEOUT_BEGIN                ( GameOpt.TimeoutBegin )
#define TIMEOUT_END                  ( GameOpt.TimeoutEnd )

#define TB_BATTLE_TIMEOUT            ( 100000000 )
#define TB_BATTLE_TIMEOUT_CHECK( to )         ( ( to ) > 10000000 )

// Kills
#define KILL_BEGIN                   ( GameOpt.KillBegin )
#define KILL_END                     ( GameOpt.KillEnd )

// Perks
#define PERK_BEGIN                   ( GameOpt.PerkBegin )
#define PERK_END                     ( GameOpt.PerkEnd )
#define PERK_COUNT                   ( PERK_END - PERK_BEGIN + 1 )

// Addiction
#define ADDICTION_BEGIN              ( GameOpt.AddictionBegin )
#define ADDICTION_END                ( GameOpt.AddictionEnd )

// Karma
#define KARMA_BEGIN                  ( GameOpt.KarmaBegin )
#define KARMA_END                    ( GameOpt.KarmaEnd )

// Damages
#define DAMAGE_BEGIN                 ( GameOpt.DamageBegin )
#define DAMAGE_END                   ( GameOpt.DamageEnd )

// Traits
#define TRAIT_BEGIN                  ( GameOpt.TraitBegin )
#define TRAIT_END                    ( GameOpt.TraitEnd )
#define TRAIT_COUNT                  ( TRAIT_END - TRAIT_BEGIN + 1 )

// Reputation
#define REPUTATION_BEGIN             ( GameOpt.ReputationBegin )
#define REPUTATION_END               ( GameOpt.ReputationEnd )

// FORP AP stuff
#ifdef FORP_ENGINE
#define AP_DELTA					 ( 250 )
#define AP_REGEN_FORMULA( cr )		 ( cr->GetParam( ST_APREGEN ) * delta / 1000 )

#define ST_APREGEN                   ( 192 )
#define ST_AP_COST_RUN               ( 782 )			// Hardcoded! Used by client and server engine. 1 AP == 100, maximim 255. Used instead of RtApCostCritterRun.
#else
#define AP_DELTA					 ( 500 )
#define AP_REGEN_FORMULA( cr )		 ( max_ap * delta / GameOpt.ApRegeneration )
#endif

// Special send params
enum SPECIAL_SEND_PARAM_TYPES
{
	OTHER_BRECK_TIME = MAX_PARAMS,
	OTHER_WAIT_TIME,
	OTHER_FLAGS,
	OTHER_BASE_TYPE,
	OTHER_MULTIHEX,
	OTHER_YOU_TURE,
	OTHER_CLEAR_MAP,
	OTHER_TELEPORT
};

// Critter actions
// Flags for chosen:
// l - hardcoded local call
// s - hardcoded server call
// for all others critters actions call only server
//                                         flags    actionExt                                                      item
enum ACTION_TYPES
{
	ACTION_MOVE = 0,					// l
	ACTION_RUN,							// l
	ACTION_MOVE_ITEM,					// l s      from slot                                                      +
	ACTION_MOVE_ITEM_SWAP,				// l s      from slot                                                      +
	ACTION_USE_ITEM,					// l s                                                                     +
	ACTION_DROP_ITEM,					// l s      from slot                                                      +
	ACTION_USE_WEAPON,					// l        fail attack 8 bit, use index (0-2) 4-7 bits, aim 0-3 bits      +
	ACTION_RELOAD_WEAPON,				// l s                                                                     +
	ACTION_USE_SKILL,					// l s      skill index (see SK_*)
	ACTION_PICK_ITEM,					// l s                                                                     +
	ACTION_PICK_CRITTER,				// l        0 - loot, 1 - steal, 2 - push
	ACTION_OPERATE_CONTAINER,			// l s      transfer type * 10 + [0 - get, 1 - get all, 2 - put]           + (exclude get all)
	ACTION_BARTER,						//   s      0 - item taken, 1 - item given                                 +
	ACTION_DODGE,						//          0 - front, 1 - back
	ACTION_DAMAGE,						//          0 - front, 1 - back
	ACTION_DAMAGE_FORCE,				//          0 - front, 1 - back
	ACTION_KNOCKOUT,					//   s      0 - knockout anim2begin
	ACTION_STANDUP,						//   s      0 - knockout anim2end
	ACTION_FIDGET,						// l
	ACTION_DEAD,						//   s      dead type anim2 (see Anim2 in _animation.fos)
	ACTION_CONNECT,						//
	ACTION_DISCONNECT,					//
	ACTION_RESPAWN,						//   s
	ACTION_REFRESH						//   s
};

// Script defines
// Look checks
#define LOOK_CHECK_DIR               ( 0x01 )
#define LOOK_CHECK_SNEAK_DIR         ( 0x02 )
#define LOOK_CHECK_SNEAK_WEIGHT      ( 0x04 )
#define LOOK_CHECK_TRACE             ( 0x08 )
#define LOOK_CHECK_SCRIPT            ( 0x10 )
#define LOOK_CHECK_ITEM_SCRIPT       ( 0x20 )

// In SendMessage
enum AI_MESSAGE_TYPES
{
	MESSAGE_TO_VISIBLE_ME = 0,
	MESSAGE_TO_IAM_VISIBLE,
	MESSAGE_TO_ALL_ON_MAP
};

// Special skill values
enum SPECIAL_SKILL_TYPES
{
	SKILL_TALK = -7,
	SKILL_PUSH_CRITTER,
	SKILL_LOOT_CRITTER,
	SKILL_TAKE_ALL_CONT,
	SKILL_TAKE_CONT,
	SKILL_PUT_CONT,
	SKILL_PICK_ON_GROUND
};

// Anim1
#define ANIM1_UNARMED                ( 1 )
// Anim2
enum ANIM2_TYPES
{
	ANIM2_IDLE = 1,
	ANIM2_WALK = 3,
	ANIM2_LIMP,
	ANIM2_RUN,
	ANIM2_PANIC_RUN,
	ANIM2_SNEAK_WALK,
	ANIM2_SNEAK_RUN
};

#define ANIM2_IDLE_PRONE_FRONT       ( 86 )
#define ANIM2_IDLE_PRONE_BACK        ( 87 )
#define ANIM2_DEAD_FRONT             ( 102 )
#define ANIM2_DEAD_BACK              ( 103 )

// Move params
// 6 next steps (each 5 bit) + stop bit + run bit
// Step bits: 012 - dir, 3 - allow, 4 - disallow
#define MOVE_PARAM_STEP_COUNT        ( 6 )
#define MOVE_PARAM_STEP_BITS         ( 5 )
#define MOVE_PARAM_STEP_DIR          ( 0x7 )
#define MOVE_PARAM_STEP_ALLOW        ( 0x8 )
#define MOVE_PARAM_STEP_DISALLOW     ( 0x10 )
#define MOVE_PARAM_RUN               ( 0x80000000 )

// Holodisks
#define USER_HOLO_TEXTMSG_FILE       "FOHOLOEXT.MSG"
#define USER_HOLO_START_NUM          ( 100000 )
#define USER_HOLO_MAX_TITLE_LEN      ( 40 )
#define USER_HOLO_MAX_LEN            ( 2000 )

#endif // ___DEFINES___
