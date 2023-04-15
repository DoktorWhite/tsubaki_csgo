#if defined _tsubaki_game_core
	#endinput
#endif
#define _tsubaki_game_core

/**
 * CopyVector 要改曬所有順序
 */

#pragma semicolon 1

#include <tsubaki_v3/tsubaki_common>
#include <tsubaki_v3/tsubaki_menu>
#include <tsubaki_v3/tsubaki_exception>

#include <tsubaki_v3/tsubaki_util>

#include <tsubaki_v3/tsubaki_bullet>
#include <tsubaki_v3/tsubaki_damage_area>
#include <tsubaki_v3/tsubaki_trap>

/*  For Debug  */
	#if defined DEBUG
		#define TOTAL_DEBUG_COLOR	7
		stock DEBUG_COLORS[TOTAL_DEBUG_COLOR][4] = {
			{ 255, 51, 51,255}
			, { 255, 153, 51,255}
			, { 255, 255, 51,255}
			, { 51, 255, 51,255}
			, { 51, 255, 255,255}
			, { 51, 51, 255,255}
			, { 153, 51, 255,255}
		};
		stock DEBUG_COLOR_COUNTER[MAXPLAYERS];
	#endif

/*  Map Variables  */

	#define MAX_MAP_LOAD	9
	#define MAX_ITEM_ORIGIN_LOAD	128
	
	stock int MAP_LOADED;
	stock char LOADED_MAP_NAME[MAX_MAP_LOAD][64];
	stock int LOADED_MAP_GAMEMODE[MAX_MAP_LOAD];
	stock int giPlyMapChoice[MAXPLAYERS];

	stock int CURRENT_MAP_ID;
	stock int CURRENT_MAP_DIFFICULTY;
	stock int CURRENT_MAP_ITEM_ORIGIN_AMOUNT;
	stock float CURRENT_MAP_ITEM_ORIGIN[MAX_ITEM_ORIGIN_LOAD][3];

	#define MAP_DISPLAY_RANK	5
	#define MAP_SCORE_PER_LEVEL	7500.0
	stock float gfPlyGameScore[MAXPLAYERS];

/*  Game mode  */
	#define TOTAL_GAME_MODE 1+5			//0 is used for empty
		stock String:NAME_OF_GAMEMODE[TOTAL_GAME_MODE][32] =
		{
			"EMPTY",
			"人類VS喪屍",
			"大逃殺BR",
			"VS BOSS",
			"Citymall",
			"HUMAN MUST DIE"
		};
		#define GAMEMODE_HVZ			1
		#define GAMEMODE_BR				2
		#define GAMEMODE_VB				3
		#define GAMEMODE_CITYMALL		4
			#if defined DEBUG
				#define CITYMALL_DEFAULT_TIME 9999.0
			#else
				#define CITYMALL_DEFAULT_TIME	240.0
			#endif
			#define CITYMALL_EXTEND_TIME	15.0
		#define GAMEMODE_HMD	5			//HUMAN MUST DIE
		
		stock GAMEMODE_BOT_AMOUNT[TOTAL_GAME_MODE] = 
		{
			0,
			12,
			4,
			3,
			0,
			12
		};

	stock giRoundCounter;
	stock ConVar g_CvarCurRoundMode;
	stock giCurRoundMode;
		#define AllowIdentity(identity)	(GAMEMODE_POSSIBLE_IDENTITY[giCurRoundMode] & (1<<identity))
	stock gbsGameStatus = (1<<0);
		#define GAME_STATUS_PREPARE				(1<<0)
		#define GAME_STATUS_LACK_OF_PLAYER		(1<<1)
		#define GAME_STATUS_START				(1<<2)
		#define GAME_STATUS_END					(1<<3)
		#define GAME_STATUS_BR_END				(1<<4)
		#define GAME_STATUS_NO_KILL_LOG			(1<<7)
		#define GAME_STATUS_LAST_HUMAN			(1<<10)
		#define GAME_STATUS_NO_RESPAWN			(1<<12)
		#define GAME_STATUS_TEAM_ATK			(1<<14)
		#define GAME_STATUS_KICK_BOT			(1<<15)
		#define GAME_STATUS_EMERGENCY			(1<<18)
		#define GAME_STATUS_PROGRESS_COMPLETE	(1<<19)
		#define GAME_STATUS_BOSS				(1<<20)
		#define GAME_STATUS_BOSS_PASSIVE		(1<<22)
		#define GAME_STATUS_BOSS_FREEZE			(1<<23)
		#define GAME_STATUS_BOSS_GODMODE		(1<<24)
		#define GAME_STATUS_BOSS_ABILITY_END	(1<<26)
		#define GAME_STATUS_BOSS_FINAL			(1<<27)
		#define GAME_STATUS_FINAL_FINISHED		(1<<28)

		#define GAME_PREPARE				(gbsGameStatus & GAME_STATUS_PREPARE)
		#define GAME_LACK_OF_PLAYER			(gbsGameStatus & GAME_STATUS_LACK_OF_PLAYER)
		#define GAME_START					(gbsGameStatus & GAME_STATUS_START)
		#define GAME_END					(gbsGameStatus & GAME_STATUS_END)
		#define GAME_BR_END					(gbsGameStatus & GAME_STATUS_BR_END)
		#define GAME_NO_KILL_LOG			(gbsGameStatus & GAME_STATUS_NO_KILL_LOG)
		#define GAME_LAST_HUMAN				(gbsGameStatus & GAME_STATUS_LAST_HUMAN)
		#define GAME_NO_RESPAWN				(gbsGameStatus & GAME_STATUS_NO_RESPAWN)
		#define GAME_TEAM_ATK_ON			(gbsGameStatus & GAME_STATUS_TEAM_ATK)
		#define GAME_KICK_BOT				(gbsGameStatus & GAME_STATUS_KICK_BOT)
		#define GAME_EMERGENCY				(gbsGameStatus & GAME_STATUS_EMERGENCY)
		#define GAME_PROGRESS_COMPLETE		(gbsGameStatus & GAME_STATUS_PROGRESS_COMPLETE)
		#define GAME_BOSS					(gbsGameStatus & GAME_STATUS_BOSS)
		#define GAME_BOSS_PASSIVE			(gbsGameStatus & GAME_STATUS_BOSS_PASSIVE)
		#define GAME_BOSS_FREEZE			(gbsGameStatus & GAME_STATUS_BOSS_FREEZE)
		#define GAME_BOSS_GODMODE			(gbsGameStatus & GAME_STATUS_BOSS_GODMODE)
		#define GAME_BOSS_ABILITY_END		(gbsGameStatus & GAME_STATUS_BOSS_ABILITY_END)	
		#define GAME_BOSS_FINAL				(gbsGameStatus & GAME_STATUS_BOSS_FINAL)
		#define GAME_FINAL_FINISHED			(gbsGameStatus & GAME_STATUS_FINAL_FINISHED)	//Boss Ability Calculated?
	
	#define HUD_CHN_GAME_STATUS 1

	#define ENDING_HUMAN_WIN		1
	#define ENDING_HUMAN_LOSE		2
	#define ENDING_DRAW				3
	#define ENDING_BR_GAMEOVER		4
	#define ENDING_BR_TIMEUP		5

	stock Float:gfGameStartTime;
	stock Float:gfGameProgress;

	stock Handle g_hTimerForPlayerCount;
	stock Handle g_hTimerForGameStatus;

	stock bool gbGamemodeLoadBPWH[TOTAL_GAME_MODE] = {
		false,
		true,
		false,
		true,
		true,
		false
	};

/*  Ability Variables  */
	#define TOTAL_ABILITY 12+1
		#define ABILITY_HP_LIMIT 	1	//a		
		#define ABILITY_ATK			2	//b
		#define ABILITY_DEF			3	//c
		#define ABILITY_GRAV		4	//d
			#define PLAYER_BASE_GRAVITY 800.0
		#define ABILITY_SPD			5	//e
			#define PLAYER_BASE_SPEED	320.0
		#define ABILITY_ITEM_EFFECT	6	//f	//薬物効果
		#define ABILITY_BUFF_RATE	7	//g	//バフ率
		#define ABILITY_DEBUFF_RATE	8	//h	//デバフ防御率
		#define ABILITY_WEAPON_EXP	9	//i
		#define ABILITY_DROP_RATE	10	//j	//ドロップ率
		#define ABILITY_SUPPLY_TIME	11  //k 補給時間
		#define ABILITY_CRITICAL	12  //l 爆擊機率
	#define CRITICAL_DMG_AMPIFIER	2.0
	stock String:CRITICAL_DMG_MSG[] = "爆撃";

	stock String:NAME_OF_ABILITY[TOTAL_ABILITY][] =
	{
		"", 
		"生命値上限", 			//a
		"攻擊", 				//b
		"防御", 				//c
		"重力軽減", 			//d
		"速度", 				//e
		"薬物効果", 			//f
		"BUFF機率", 			//g
		"DEBUFF防御機率", 		//h
		"武器経験値増幅率", 	//i		
		"物品掉落率",			//j
		"補給時間",				//k
		"爆擊機率"				//l
	};

	stock Float:gfPlyCurAbility[MAXPLAYERS][TOTAL_ABILITY]; //現在玩家實際能力值
	stock Float:gfPlyGamePauseAbility[TOTAL_ABILITY];   //暫停時玩家能力值
	//stock Float:gfPlyDefaultAbility[MAXPLAYERS][TOTAL_IDENTITY][TOTAL_ABILITY]; //玩家基本能力值(遊戲時)
	//stock Float:gfPlyGameProgressAbility[MAXPLAYERS][TOTAL_IDENTITY][TOTAL_ABILITY];    //玩家遊戲進度能力值
	//stock Float:gfPlyIdentityAmountAbility[MAXPLAYERS][TOTAL_IDENTITY][TOTAL_ABILITY];	//玩家數對能力影響(身份)
	stock Float:gfPlyFreeAbility[MAXPLAYERS][TOTAL_ABILITY];    //玩家自由能力值
	stock Float:gfPlyEquipAbility[MAXPLAYERS][TOTAL_ABILITY];   //
	stock Float:gfPlySkillAbility[MAXPLAYERS][TOTAL_ABILITY];   //
	stock Float:gfPlyAbnormalAbility[MAXPLAYERS][TOTAL_ABILITY];    //玩家異常能力值

	//stock Float:IDENTITY_ABILITY_LOW_LIMIT[TOTAL_IDENTITY][TOTAL_ABILITY];      //身分能力值下限
	//stock Float:IDENTITY_ABILITY_HIGH_LIMIT[TOTAL_IDENTITY][TOTAL_ABILITY];     //身分能力值上限

	stock bool:gbPlyGodMode[MAXPLAYERS];    //玩家無敵
	stock bool:gbPlyFreeze[MAXPLAYERS];     //玩家静止
	
	stock Handle g_hCalculateFinalAbilityTask[MAXPLAYERS];
	#define DEFAULT_FINAL_ABILITY_TASK_DELAY	5.0
	#define CreateCalculateFinalAbilityTask(%0) g_hCalculateFinalAbilityTask[%0] = CreateTimer(DEFAULT_FINAL_ABILITY_TASK_DELAY, TaskCalculateFinalAbility, client, TIMER_FLAG_NO_MAPCHANGE)
	#define RemoveCalculateFinalAbilityTask(%0) if(g_hCalculateFinalAbilityTask[%0]!=INVALID_HANDLE) { KillTimer(g_hCalculateFinalAbilityTask[%0]); } g_hCalculateFinalAbilityTask[%0]=INVALID_HANDLE

	// stock Float:LEADER_ABILITY_AMPIFIER[TOTAL_ABILITY] = {
	// 	0.0,
	// 	0.1,
	// 	0.05,
	// 	0.05,
	// 	0.0,
	// 	0.1,
	// 	0.0,
	// 	0.0,
	// 	0.0,
	// 	0.1,
	// 	0.1,
	// 	0.0,
	// 	0.0
	// };

	// stock bool:BOT_AMPIFIER_ON_IDENTITY[TOTAL_IDENTITY] = {
	// 	false,
	// 	false,		//HUMAN
	// 	false,		//LEADER
	// 	true,		//ZOMBIE
	// 	false		//BOSS
	// };
	stock Float:BOT_ABILITY_AMPIFIER[TOTAL_ABILITY] = {
		0.0,
		0.5,
		-0.2,
		0.0,
		0.95,
		0.2,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0,
		0.0
	};

/*  SP Variables  */

	#define DEFAULT_SP 5
	#define SECOND_PER_LEVEL	1800
	
	stock giPlyLevel[MAXPLAYERS];
	stock giPlyAvailableSP[MAXPLAYERS];
	stock giPlySP[MAXPLAYERS][TOTAL_ABILITY];

	stock MAX_SP[TOTAL_ABILITY] = {
		0,
		60,
		0,
		35,
		35,
		35,
		0,
		20,
		20,
		35,
		35,
		35,
		35
	};

	stock Float:SP_ABILITY_FACTOR[TOTAL_ABILITY];		//Retrieve From Database of Gamemode
	// float SP_ABILITY_FACTOR[TOTAL_ABILITY] = {
	// 	0.0,
	// 	0.7,			//HP
	// 	0.01,			//ATK
	// 	0.005,			//DEF
	// 	-3.0,			//GRAV
	// 	1.5,			//SPD
	// 	0.005,			//ITEM EFFECT
	// 	0.005,			//BUFF
	// 	0.005,			//DEBUFF
	// 	0.02,			//WEAPON EXP
	// 	0.0025,			//DROP
	// 	0.0075,
	// 	0.0025
	// };
	
/*  Identity Variables  */

	//Identity Constant see tsubaki_common.sp

	stock GAMEMODE_POSSIBLE_IDENTITY[TOTAL_GAME_MODE] = 
	{
		0,
		(1<<IDENTITY_HUMAN)|(1<<IDENTITY_LEADER)|(1<<IDENTITY_ZOMBIE)|(1<<IDENTITY_BOSS),		//HVZ
		(1<<IDENTITY_HUMAN),																	//BR
		(1<<IDENTITY_HUMAN)|(1<<IDENTITY_ZOMBIE)|(1<<IDENTITY_BOSS),								//VB
		(1<<IDENTITY_HUMAN),																	//Citymall
		(1<<IDENTITY_HUMAN)|(1<<IDENTITY_ZOMBIE)												//HUMAN MUST DIE
	};

	#define ALLOW_IDENTITY(%0) (GAMEMODE_POSSIBLE_IDENTITY[giCurRoundMode] & (1<<%0))


	stock LEADER_ID;

	#define ChangePlayerIdentity(client, identity) (giPlyIdentity[client] = identity)

	stock ALIVE_IDENTITY_COUNTER[TOTAL_IDENTITY];
	stock IDENTITY_COUNTER[TOTAL_IDENTITY];

	#define ALIVE_HUMAN_COUNTER			(ALIVE_IDENTITY_COUNTER[IDENTITY_HUMAN] + ALIVE_IDENTITY_COUNTER[IDENTITY_LEADER])
	#define ALIVE_ZOMBIE_COUNTER		(ALIVE_IDENTITY_COUNTER[IDENTITY_ZOMBIE])
	#define ZOMBIE_COUNTER				(IDENTITY_COUNTER[IDENTITY_ZOMBIE])

	#define GetPlayers_IncludeHumans			(1<<IDENTITY_HUMAN)
	#define GetPlayers_IncludeLeader			(1<<IDENTITY_LEADER)
	#define GetPlayers_IncludeZombies			(1<<IDENTITY_ZOMBIE)
	#define GetPlayers_IncludeBoss				(1<<IDENTITY_BOSS)
	#define GetPlayers_IncludeAllIdentity		(1<<TOTAL_IDENTITY)
	#define GetPlayers_IncludeBots				(1<<24)
	#define GetPlayers_IncludeAlive				(1<<25)
	#define GetPlayers_IncludeDead				(1<<26)
	#define GetPlayers_IncludeSpectator			(1<<28)

	stock Float:gfPlyDefaultAbility[MAXPLAYERS][TOTAL_IDENTITY][TOTAL_ABILITY]; //玩家基本能力值(遊戲時)
	stock Float:gfPlyGameProgressAbility[MAXPLAYERS][TOTAL_IDENTITY][TOTAL_ABILITY];    //玩家遊戲進度能力值
	stock Float:gfPlyIdentityAmountAbility[MAXPLAYERS][TOTAL_IDENTITY][TOTAL_ABILITY];	//玩家數對能力影響(身份)

	stock Float:DEFAULT_IDENTITY_ABIILTY[TOTAL_IDENTITY][TOTAL_ABILITY];    
	stock Float:IDENTITY_ABILITY_LOW_LIMIT[TOTAL_IDENTITY][TOTAL_ABILITY];      //身分能力值下限
	stock Float:IDENTITY_ABILITY_HIGH_LIMIT[TOTAL_IDENTITY][TOTAL_ABILITY];     //身分能力值上限

	stock Float:LEADER_ABILITY_AMPIFIER[TOTAL_ABILITY] = {
		0.0,
		0.1,
		0.05,
		0.05,
		0.0,
		0.1,
		0.0,
		0.0,
		0.0,
		0.1,
		0.1,
		0.0,
		0.0
	};

	stock bool:BOT_AMPIFIER_ON_IDENTITY[TOTAL_IDENTITY] = {
		false,
		false,		//HUMAN
		false,		//LEADER
		true,		//ZOMBIE
		false		//BOSS
	};



/*  Item Variables  */

	#define TOTAL_ITEM 64
	#define TOP_ITEM_RARE 8

	#define ITEM_DATA 11
		#define ITEM_DATA_TYPE					0
			#define IT_REMOVABLE			(1<<1) 		//a
			#define IT_ETERNAL				(1<<2) 		//b
			#define IT_CONTINUOUS_EFFECT	(1<<3) 		//c
			#define IT_EQUIP				(1<<4) 		//d
			#define IT_INSTANTLY			(1<<5)		//e		一次性
			#define IT_PERIOD				(1<<6)		//f
			#define IT_GAMBLE				(1<<12)		//l
			#define IT_GREAT_GAMBLE			(1<<14)		//n
			#define IT_HEAL					(1<<16)		//p
			#define IT_ONLY_HEALING_ITEM	(1<<17)		//q
			#define IT_GAME_PAUSE			(1<<20)		//t
			#define IT_TRIGGER_WHILE_DEAD	(1<<22)		//v
			#define IT_MATERIAL				(1<<26)		//z
		#define ITEM_DATA_IDENTITY				1
		#define ITEM_DATA_DURABILITY			2
		#define ITEM_DATA_RARE					3
		#define ITEM_DATA_LAST_TIME				4
		#define ITEM_DATA_CONTINUOUS_RATE		5
		#define ITEM_DATA_AVAILABLE_GAMEMODE	6
		#define ITEM_DATA_MAX_LEVEL				7
		#define ITEM_DATA_INSTANT_HEAL			8
		#define ITEM_DATA_CONTINUOUS_HEAL		9
		#define ITEM_DATA_SELL_PRICE			10

	#define ITEM_HUD_WITHOUT_TIME (IT_EQUIP|IT_ETERNAL)
	#define ITEM_DISPLAY_TIME(item) (~ITEM_HUD_WITHOUT_TIME & ITEM_DEFAULT_DATA[item][ITEM_TYPE])

	stock int LOADED_ITEM_AMOUNT;
	stock char NAME_OF_ITEM[TOTAL_ITEM][32];
	stock char DESCRIPTION_OF_ITEM[TOTAL_ITEM][64];
	stock int ITEM_DEFAULT_DATA[TOTAL_ITEM][ITEM_DATA];
	stock int ITEM_ABILITY_LIST[TOTAL_ITEM][TOTAL_ABILITY];
	stock float ITEM_ABILITY_VALUE[TOTAL_ITEM][TOTAL_ABILITY];
	stock int ITEM_DISPLAY_SORT[TOTAL_ITEM];

	stock int GAMBLE_ITEM_AMOUNT;
	stock int GAMBLE_ITEM[TOTAL_ITEM/2];
	stock int GREAT_GAMBLE_ITEM_AMOUNT;
	stock int GREAT_GAMBLE_ITEM[TOTAL_ITEM/2];

	#define MAX_APPEAR_ITEM_RARE	8
	stock int RANDOM_ITEM_AMOUNT[TOTAL_IDENTITY];
	stock int RANDOM_ITEM_LIST[TOTAL_IDENTITY][TOTAL_ITEM*MAX_APPEAR_ITEM_RARE];

	stock int RANDOM_ITEM_AMOUNT_AFTER_KILL[TOTAL_IDENTITY];
	stock int RANDOM_ITEM_LIST_AFTER_KILL[TOTAL_IDENTITY][TOTAL_ITEM*MAX_APPEAR_ITEM_RARE];

	stock int RANDOM_ITEM_AMOUNT_IN_STORE;
	stock int RANDOM_ITEM_IN_STORE[TOTAL_ITEM];

	stock int giPlyTriggeredItem[MAXPLAYERS][TOTAL_ITEM];
	stock int giPlyTriggeredItemLevel[MAXPLAYERS][TOTAL_ITEM];
	stock float gfPlyItemTime[MAXPLAYERS][TOTAL_ITEM];
	stock bool gbCanPlayerGetItem[MAXPLAYERS];

	stock Handle g_hItemRemoveTimer[MAXPLAYERS][TOTAL_ITEM];
	stock Handle g_hItemEffectTimer[MAXPLAYERS][TOTAL_ITEM];

	#define MENUID_ITEM_DICT_ADMIN       111
	#define MENUID_ITEM_DICT_DISPLAY_ALL 112
	#define MENUID_ITEM_DICT             113

	// stock int GetItemDisplayId(int item_id)
	// {
	// 	for(int i=0; i<TOTAL_ITEM && ITEM_DISPLAY_SORT[i]!=0; i++)
	// 		if(ITEM_DISPLAY_SORT[i] == item_id)
	// 			return i;

	// 	return -1;
	// }

	// //Identity flags
	// // a~o	attacker identity
	// // p~z	victim identity

/*  Item Entity Variable  */

	stock String:ITEM_ENTITY[] = "Item_Entity";
	stock String:DEFAULT_ITEM_MODEL[256];
	stock int DEFAULT_ITEM_LASER_SPRITE;
	#define ITEM_ENTITY_THINKTIME	0.5
	#define ITEM_ENTITY_SEARCH_RANGE 30.0
	#define DEFAULT_ITEM_SOLID        FSOLID_TRIGGER
	
	stock float ITEM_ENTITY_MIN_SIZE[3] = {-5.0,-5.0,-5.0};
	stock float ITEM_ENTITY_MAX_SIZE[3] = { 5.0, 5.0, 5.0};

/*  Abnormal Variables  */
	#define TOTAL_ABNORMAL	64

	#define ABNORMAL_DATA 5
		#define ABNORMAL_DATA_TYPE						0
			#define AT_REMOVABLE			(1<<1)		//a
			#define AT_ETERNAL				(1<<2)		//b
			#define AT_CONTINUOUS_EFFECT	(1<<3)		//c
			#define AT_EQUIP				(1<<4)		//d
			#define AT_GAMBLE_BUFF			(1<<12)		//l
			#define AT_GAMBLE_DEBUFF		(1<<13)		//m
			#define AT_INSURANCE_TARGET		(1<<14)		//n		醫療保險對象
			#define AT_GAME_PAUSE			(1<<20)		//t
			#define AT_TRIGGER_WHILE_DEAD	(1<<22)		//v
			#define AT_DONT_DISPLAY			(1<<23)		//w
			#define AT_BUFF					(1<<24)		//x
			#define AT_DEBUFF				(1<<25)		//y
		#define ABNORMAL_DATA_IDENTITY					1
		#define ABNORMAL_DATA_MAX_LEVEL					2
		#define ABNORMAL_DATA_AVAILABLE_GAMEMODE		3
		#define ABNORMAL_DATA_CONTINUOUS_EFFECT_RATE	4

	#define ABNORMAL_HUD_WITHOUT_TIME (AT_EQUIP|AT_ETERNAL)
	#define ABNORMAL_DONT_DISPLAY(abnormal) (AT_DONT_DISPLAY & ABNORMAL[abnormal][ABNORMAL_TYPE])
	#define ABNORMAL_DISPLAY_TIME(abnormal)	(~ABNORMAL_HUD_WITHOUT_TIME & ABNORMAL[abnormal][ABNORMAL_TYPE])

	stock LOADED_ABNORMAL;
	stock String:NAME_OF_ABNORMAL[TOTAL_ABNORMAL][32];
	stock ABNORMAL_DEFAULT_DATA[TOTAL_ABNORMAL][ABNORMAL_DATA];
	stock ABNORMAL_ABILITY_LIST[TOTAL_ABNORMAL][TOTAL_ABILITY];
	stock Float:ABNORMAL_ABILITY_VALUE[TOTAL_ABNORMAL][TOTAL_ABILITY];

	stock GAMBLE_BUFF_AMOUNT;
	stock GAMBLE_BUFF[TOTAL_ABNORMAL/3];
	stock GAMBLE_DEBUFF_AMOUNT;
	stock GAMBLE_DEBUFF[TOTAL_ABNORMAL/3];

	stock giPlyTriggerAbnormal[MAXPLAYERS][TOTAL_ABNORMAL];
	stock giPlyTriggerAbnormalLevel[MAXPLAYERS][TOTAL_ABNORMAL];
	stock giPlyAbnormalTriggeredPlayer[MAXPLAYERS][TOTAL_ABNORMAL];
	stock Float:gfPlyAbnormalTime[MAXPLAYERS][TOTAL_ABNORMAL];

	Handle g_hAbnormalEffectTimer[MAXPLAYERS][TOTAL_ABNORMAL];
	Handle g_hAbnormalRemoveTimer[MAXPLAYERS][TOTAL_ABNORMAL];

	#define MENUID_ABNORMAL_DICT_ADMIN		121

	//TO-DO EMIT_GODMODE_SOUND(%0,%1,%2) To Customize Sound
	// char GODMODE_SOUND[] = "tsubaki/effects/temp_barrier.mp3";
	// #define EMIT_GODMODE_SOUND(%0,%1) EmitSoundToClient(%0, GODMODE_SOUND, %1, SNDCHAN_ITEM, SNDLEVEL_NONE, SND_NOFLAGS, 0.45, SNDPITCH_NORMAL, %1)

/*  Backpack and Warehouse Variables  */

	#define DEFAULT_BP_SPACE 5
	#define DEFAULT_WH_SPACE 20

	#define MAX_BP_SPACE 10
	#define MAX_WH_SPACE 100

	#define TOTAL_PLAYER_ITEM_INFO	2
		#define PLAYER_ITEM_ID			0		//ID
		#define PLAYER_ITEM_DURABILITY	1		//耐久度

	stock int giPlyBPSpace[MAXPLAYERS];
	stock int giPlyWHSpace[MAXPLAYERS];

	stock int giPlyBPItem[MAXPLAYERS][MAX_BP_SPACE][TOTAL_PLAYER_ITEM_INFO];
	stock int giPlyWHItem[MAXPLAYERS][MAX_WH_SPACE][TOTAL_PLAYER_ITEM_INFO];

	#define BP_CASH_LIMIT	50000
	stock int giPlyBPCash[MAXPLAYERS];
	stock int giPlyWHCash[MAXPLAYERS];
	stock String:m_iAccount[] = "m_iAccount";

	#define SetPlayerMoney(%0,%1) SetEntProp(%0,Prop_Send,m_iAccount,%1)
	#define GetPlayerMoney(%0) GetEntProp(%0,Prop_Send,m_iAccount)

	stock void ClearBPSpace(client, pos, int &info=0) {
		for(; info<TOTAL_PLAYER_ITEM_INFO; info++)
			giPlyBPItem[client][pos][info]=0;
	}

	stock void ClearWHSpace(client, pos, int &info=0) 
	{
		for(; info<TOTAL_PLAYER_ITEM_INFO; info++) 
			giPlyWHItem[client][pos][info]=0;
	}

	stock int GetPlayerBackpackEmptySpace(int& client, int& i=0)
	{
		for(; i<giPlyBPSpace[client]; i++)
		{
			if(giPlyBPItem[client][i][PLAYER_ITEM_ID] == 0)
				return i;
		}

		return -1;
	}

	stock int GetPlayerWarehouseEmptySpace(int& client)
	{
		for(int i=0; i<giPlyWHSpace[client]; i++)
		{
			if(giPlyWHItem[client][i][PLAYER_ITEM_ID] == 0)
				return i;
		}

		return -1;
	}

	stock void GivePlayerCash(int client, int amount)
	{
		if( (giPlyBPCash[client] += amount) > BP_CASH_LIMIT )
		{
			PrintToChat(client, " \x04已存入$%d入銀行", giPlyBPCash[client]);
			
			giPlyWHCash[client] += giPlyBPCash[client];
			giPlyBPCash[client] = 0;
		}
		
		SetPlayerMoney(client, giPlyBPCash[client]);
	} 

	#define MENUID_BACKPACK_USE		8
	#define MENUID_BACKPACK_THROW	9
	#define MENUID_BP_TO_WH			12
	#define MENUID_WH_TO_BP			14
	#define MENUID_BP_TRADE			18
	#define MENUID_WH_SELL			19


/*  Equip Variables  */
	#define TOTAL_EQUIP		64
	#define EQUIP_DISPLAY_RARE	5
		#define DISPLAY_EQUIP(%0)	(EQUIP_DEFAULT_DATA[%0][EQUIP_DATA_RARE] <= EQUIP_DISPLAY_RARE)
	#define MENUID_EQUIP	16
	#define MENUID_EQUIP_CHOOSE	17

	#define EQUIP_DATA		9
		#define EQUIP_DATA_TYPE					0
			#define TOTAL_EQUIP_TYPE				4			
			#define TOTAL_EQUIP_TYPE_ALL_IDENTITY	TOTAL_EQUIP_TYPE+1		//1 is for leader
		#define EQUIP_DATA_RARE					1
		#define EQUIP_DATA_USE_LIMIT			2
		#define EQUIP_DATA_COOLDOWN				3
		#define EQUIP_DATA_ROUND_COOLDOWN		4
		#define EQUIP_DATA_IS_ENABLE			5
			#define EQUIP_DATA_DISABLE				-1
		#define EQUIP_DATA_AVAILABLE_GAMEMODE	6
		#define EQUIP_DATA_AUTO_TRIGGER_MSG		7
		#define EQUIP_DATA_SKILL_ID				8

	stock char NAME_OF_EQUIP_SLOT[TOTAL_EQUIP_TYPE_ALL_IDENTITY+1] = "ABCDL";
	#define GetEquipSlot(%0)		(EQUIP_DEFAULT_DATA[%0][EQUIP_DATA_TYPE]-1)
	
	stock int LOADED_EQUIP;
	stock char NAME_OF_EQUIP[TOTAL_EQUIP][32];
	stock char EQUIP_DESCRIPTION[TOTAL_EQUIP][128];
	stock char EQUIP_UNLOCK_CONDITION[TOTAL_EQUIP][128];
	stock int EQUIP_DISPLAY_LIST[TOTAL_EQUIP_TYPE_ALL_IDENTITY][TOTAL_EQUIP/2];
	stock int LOADED_EQUIP_EACH_SLOT[TOTAL_EQUIP_TYPE_ALL_IDENTITY];

	stock int EQUIP_DEFAULT_DATA[TOTAL_EQUIP][EQUIP_DATA];
	stock int EQUIP_ABILITY_LIST[TOTAL_EQUIP][TOTAL_ABILITY];
	stock float EQUIP_ABILITY_VALUE[TOTAL_EQUIP][TOTAL_ABILITY];

	stock int giPlyEquipLastUsedRound[MAXPLAYERS][TOTAL_EQUIP];
	stock int gbsPlyEquipUnlocked[MAXPLAYERS][1+(TOTAL_EQUIP/32)];
		bool IsPlayerEquipUnlocked(int& client, int equip_id) { return (gbsPlyEquipUnlocked[client][equip_id/32])&(1<<(equip_id%32)) != 0; }
		void UnlockPlayerEquip(int client, int equip_id, bool dontBroadcast=false) 
		{ 
			if(!IsClientValid(client) || IsPlayerEquipUnlocked(client, equip_id)) return;

			gbsPlyEquipUnlocked[client][equip_id/32] |= (1<<(equip_id%32));
			if(!dontBroadcast)
				PrintToChat(client, " \x04已解鎖裝備%s", NAME_OF_EQUIP[equip_id]);

			if(!IsPlayerEquipUnlocked(client, 15))
			{
				int counter = 0, i=0;
				for(;i<LOADED_EQUIP; i++)
					if(IsPlayerEquipUnlocked(client, i))
						counter++;

				if(counter >= 15)
					UnlockPlayerEquip(client, 15, dontBroadcast);
			}
		}
	stock int giPlyCurrentEquip[MAXPLAYERS][TOTAL_EQUIP_TYPE_ALL_IDENTITY];
	//stock int giPlyCurrentEquipRemain[MAXPLAYERS][TOTAL_EQUIP_TYPE_ALL_IDENTITY];
	//stock float gfPlyCurrentEquipCooldown[MAXPLAYERS][TOTAL_EQUIP_TYPE_ALL_IDENTITY];
	//stock int giEquipParam[MAXPLAYERS][TOTAL_EQUIP_TYPE_ALL_IDENTITY];

	//裝置局數Cooldown
	#define GetEquipCooldownRound(%0,%1) Clamp(giPlyEquipLastUsedRound[%0][%1]+EQUIP_DEFAULT_DATA[%1][EQUIP_DATA_ROUND_COOLDOWN]+1-giRoundCounter, 0, 99999)
	#define IsEquipInCooldown(%0,%1) (GetEquipCooldownRound(%0,%1)>0)
	
	//裝備是否使用Cooldown中
	// #define IsEquipSlotInCooldown(const int& client, const int slot) (gfPlyCurrentEquipCooldown[client][slot]>GetGameTime())

	stock Handle g_hEquipEffectTimer[MAXPLAYERS][TOTAL_EQUIP];

	#define TSUBAKI_ALLOW_EQUIP_COMMAND "tsubaki_allow_equip"
	ConVar g_CvarAllowEquip;

/*  Player Weapon Variables  */

		#define MAX_WEAPON_LEVEL 5
		#define TOTAL_PROCESS_WEAPON_SLOT		5
		stock float WEAPON_EXP[] = {300.0, 800.0, 2500.0, 5000.0, 11000.0, 0.0};
		stock float DEFAULT_SUPPLY_TIME[] = {20.0, 40.0, 80.0, 150.0, 220.0, 250.0};
		
		stock int WEAPON_AMOUNT_PER_LEVEL[MAX_WEAPON_LEVEL+1] = {4, 7, 4, 5, 7, 5};
		stock int AVAILIABLE_WEAPON_ID[MAX_WEAPON_LEVEL+1][] = 
		{
			{2, 8, 10, 11},
			{6, 9, 7, 1, 4, 3, 5},
			{12, 13, 14, 25},
			{15, 21, 22, 23, 24},
			{18, 19, 17, 32, 33, 16, 20},
			{28, 30, 29, 31, 46}
		};

		stock int giPlyChosenWeaponList[MAXPLAYERS][MAX_WEAPON_LEVEL+1];		//Used for saving
		stock int giPlyCurrentWeaponList[MAXPLAYERS][MAX_WEAPON_LEVEL+1];
		#define WEAPON_ID_DONT_GIVE		-1
		#define DEFAULT_WEAPON_LEVEL_CHECK_TIME		4.0


		#define DEFAULT_WEAPON_INFO 7
			#define WEAPON_INFO_SLOT			0
			#define WEAPON_INFO_AMMO			1
			#define WEAPON_INFO_RESERVED		2
			#define WEAPON_INFO_ATTACK_AFFECT	3
			#define WEAPON_INFO_SEMI_FIRE		4
			#define WEAPON_INFO_SUPPLY_AMMO		5
			#define WEAPON_INFO_RESERVED_AMMO	6

		#define TOTAL_WEAPON	48
		//[0]:Weapon Slot		[1]:Weapon Clip		[2]:ReserveAmmo
		stock int WEAPON_LIST_INFO[TOTAL_WEAPON][DEFAULT_WEAPON_INFO] = 
		{
			{2, 255,  -1,   0, 1,   0,   0},				// 0:knife
			{1,  12,  12,   0, 0,  12,  12},				// 1:cz75a
			{1,   7,  35,   0, 1,   7,  35},				// 2:deagle
			{1,  30, 120,   0, 1,  30, 120},				// 3:elite
			{1,  20, 100,   0, 1,  20, 100},				// 4:fiveseven
			{1,  20, 120,   0, 1,  20, 120},				// 5:glock
			{1,  13,  52,   0, 1,  13,  52},				// 6:hkp2000
			{1,  13,  26,   0, 1,  13,  26},				// 7:p250
			{1,   8,   8,   0, 1,   8,   8},				// 8:revolver
			{1,  18,  90,   0, 1,  18,  90},				// 9:tec9
			{1,  12,  24,  -1, 1,  12,  24},				//10:usp_silencer
			{0,  10,  90,  10, 1,  30,   0},				//11:ssg08
			{0,   5,  32,   0, 0,  25,   0},				//12:mag7
			{0,   8,  32,   0, 0,  32,   0},				//13:nova
			{0,   7,  32,   0, 0,  32,   0},				//14:sawedoff
			{0,   7,  32,  -5, 0,  20,   0},				//15:xm1014
			{0,  30,  90,  -5, 0, 250,   0},				//16:aug
			{0,  25,  90,   0, 0, 215,   0},				//17:famas
			{0,  35,  90,  -5, 0, 215,   0},				//18:galilar
			{0,  20,  80,   0, 0, 215,   0},				//19:m4a1_silencer
			{0,  30,  90,  -5, 0, 250,   0},				//20:sg556
			{0,  30, 100,   5, 0, 200,   0},				//21:mac10
			{0,  30, 120,   5, 0, 200,   0},				//22:mp5sd
			{0,  30, 120,   5, 0, 200,   0},				//23:mp7
			{0,  30, 120,   5, 0, 200,   0},				//24:mp9
			{0,  25, 100,  10, 0, 125,   0},				//25:ump45
			{0, 100, 200, -10, 0, 250,   0},				//26:m249
			{0, 150, 300,  -5, 0, 250,   0},				//27:negev
			{0,  30,  90, -10, 0, 250,   0},				//28:ak47
			{0,  20,  90,   0, 0, 135,   0},				//29:g3sg1
			{0,  30,  90, -10, 0, 250,   0},				//30:m4a1
			{0,  20,  90,   0, 0, 135,   0},				//31:scar20
			{0,  64, 120,   0, 0, 215,   0},				//32:bizon
			{0,  50, 100,   0, 0, 215,   0},				//33:p90
			{3, 255,   0,   0, 1,   0,   0},				//34:decoy
			{3, 255,   0,   0, 1,   0,   0},				//35:flashbang
			{3, 255,   0,   0, 1,   0,   0},				//36:hegrenade
			{3, 255,   0,   0, 1,   0,   0},				//37:smokegrenade
			{3, 255,   0,   0, 1,   0,   0},				//38:incgrenade
			{3, 255,   0,   0, 1,   0,   0},				//39:tagrenade
			{3, 255,   0,   0, 1,   0,   0},				//40:molotov
			{2,   1,   0,   0, 1,   0,   0},				//41:taser
			{9,  -1,  -1, -10, 0,   0,   0},				//42:shield
			{4,   0,   0,   0, 0,   0,   0},				//43:bumpmine
			{4,   0,   0,   0, 0,   0,   0},				//44:breachcharge
			{9,   0,   0,   0, 0,   0,   0},				//45:healthshot
			{0,  15,   0,  30, 1,  30,   0},				//46:awp
			{4, 255,   0,   0, 0,   0,   0}					//47:c4
		};

		stock char WEAPON_KNIFE[] = "weapon_knife";

		//Weapon Name List
		stock char WEAPON_LIST[TOTAL_WEAPON][] = 
		{
			"knife",	//	0
			"cz75a",	//	1
			"deagle",	//	2
			"elite",	//	3
			"fiveseven",	//	4
			"glock",	//	5
			"hkp2000",	//	6
			"p250",	//	7
			"revolver",	//	8
			"tec9",	//	9
			"usp_silencer",	//	10
			"ssg08",	//	11
			"mag7",	//	12
			"nova",	//	13
			"sawedoff",	//	14
			"xm1014",	//	15
			"aug",	//	16
			"famas",	//	17
			"galilar",	//	18
			"m4a1_silencer",	//	19
			"sg556",	//	20
			"mac10",	//	21
			"mp5sd",	//	22
			"mp7",	//	23
			"mp9",	//	24
			"ump45",	//	25
			"m249",	//	26
			"negev",	//	27
			"ak47",	//	28
			"g3sg1",	//	29
			"m4a1",	//	30
			"scar20",	//	31
			"bizon",	//	32
			"p90",	//	33
			"decoy",	//	34
			"flashbang",	//	35
			"hegrenade",	//	36
			"smokegrenade",	//	37
			"incgrenade",	//	38
			"tagrenade",	//	39
			"molotov",	//	40
			"taser",	//	41
			"shield",	//	42
			"bumpmine",	//	43			no dmg, force player jump
			"breachcharge",	//	44		detonate c4
			"healthshot",	//	45
			"awp",	//	46
			"c4"	//	47

		};

		stock int GetW_IdByWeaponNameWithHeader(String:weapon_clsname[]) {
			//PrintToServer("Checking %s", weapon_clsname);
			static i;
			for(i=0; i<TOTAL_WEAPON; i++) {
				if( (strcmp(weapon_clsname, WEAPON_LIST[i], .caseSensitive=false))==0 ) {
					return i;
				}
			}

			return -1;
		}

		//char PROJECTILE_HEADER[] = "_projectile";
		stock char INFERNO[] = "inferno";

		stock char HEGRENADE_PROJECTILE[] = "hegrenade_projectile";
		stock char SMOKEGRENADE_PROJECTILE[] = "smokegrenade_projectile";
		stock char FLASHBANG_PROJECTILE[] = "flashbang_projectile";
		stock char DECOY_PROJECTILE[] = "decoy_projectile";
		stock char MOLOTOV_PROJECTILE[] = "molotov_projectile";
		stock char TAGRENADE_PROJECTILE[] = "tagrenade_projectile";
		
		stock int giPlyWeaponLevel[MAXPLAYERS];
		stock float gfPlyWeaponExp[MAXPLAYERS];
		stock int gbsPlyReceivedWeapon[MAXPLAYERS];
		stock Handle g_hPlyWeaponCheck[MAXPLAYERS];
		stock int giPlyKnifeKillCounter[MAXPLAYERS];
		stock int giPlyHeadshotCounter[MAXPLAYERS];
		stock char headshot[] = "headshot";

		stock float gfPlyNextSupplyTime[MAXPLAYERS];
			stock bool WILL_SLOT_BE_SUPPLIED[TOTAL_PROCESS_WEAPON_SLOT] = {true, false, false, false, false};		//會補給SLOT嗎?
			stock bool IS_SLOT_INFINITY_AMMO[TOTAL_PROCESS_WEAPON_SLOT] = {false, true, false, false, false};
		stock Handle g_hPlyWeaponSupply[MAXPLAYERS];

		stock ConVar g_CvarCanPlayerCustomizeWeapon;
		#define TSUBAKI_PLAYER_CUSTOMIZE_WEAPON "tsubaki_player_customize_weapon"
			//C : Customize
			#define C_WEAPON_GAME_START	3
			#define C_WEAPON_ALLOW	1
			#define C_WEAPON_BLOCKED_DUE_TO_GAMEMODE	-1
			#define C_WEAPON_BLOCKED_DUE_TO_DIFFICULT	-2

			#define CanPlayerCustomizeWeapon() (g_CvarCanPlayerCustomizeWeapon.IntValue==C_WEAPON_ALLOW)

		#define WEAPON_BAG_INFO 4
			#define WB_WEAPON_ID		0
			#define WB_AMMO				1
			#define WB_RESERVE_AMMO		2
			#define WB_BACKUP			3

		stock int giPlyWeaponBeforeSleep[MAXPLAYERS][2][WEAPON_BAG_INFO];
		#define MENUID_WEAPON_PANEL  21

/*  Player Skills Variables  */

	#define TOTAL_SKILL	64
	stock char NAME_OF_SKILL[TOTAL_SKILL][32];
	stock char DESCRIPTION_OF_SKILL[TOTAL_SKILL][64];
	#define SKILL_DATA	5
		#define SKILL_DATA_TYPE		0
			#define ST_AUTO_TRIGGER	(1<<11)				//l
			#define ST_HAS_DURATION	(1<<12)				//m
			#define ST_HAS_CONTINUOUS_EFFECT	(1<<13)	//n
			#define ST_TRIGGER_AFTER_GAME_START	(1<<15) //p
			#define ST_TRIGGER_AFTER_RESPAWN	(1<<16) //q
			#define ST_GAME_PAUSE	(1<<20)				//t
			#define ST_TRIGGER_WHILE_DEAD	(1<<22)		//v
			#define ST_ACTIVE		(1<<25)				//x
			#define ST_PASSIVE		(1<<26)				//y
		#define SKILL_DATA_DEFAULT_CD	1
		#define SKILL_DATA_DEFAULT_AMOUNT	2
		#define SKILL_DATA_DEFAULT_DURATION	3		//Duration*100
			#define IsSkillHasDuration(%0) (DEFAULT_SKILL_DATA[%0][SKILL_DATA_DEFAULT_DURATION]!=0)
		#define SKILL_DATA_IDENTITY			4
	stock int DEFAULT_SKILL_DATA[TOTAL_SKILL][SKILL_DATA];
	#define IsSkillActive(%0)	(DEFAULT_SKILL_DATA[%0][SKILL_DATA_TYPE]&ST_ACTIVE)
	#define IsSkillPassive(%0)	(DEFAULT_SKILL_DATA[%0][SKILL_DATA_TYPE]&ST_PASSIVE)
	
	#define MAX_ACTIVE_SKILL_SLOT	4
	#define MAX_PASSIVE_SKILL_SLOT	9
	#define PLY_SKILL_INFO		2
		#define PLY_SKILL_INFO_ID		0
		#define PLY_SKILL_INFO_REMAIN	1
			#define SKILL_REMAIN_INFINITY	-999

	stock int giPlyActiveSkill[MAXPLAYERS][MAX_ACTIVE_SKILL_SLOT][PLY_SKILL_INFO];
	stock float gfPlyActiveSkillCooldown[MAXPLAYERS][MAX_ACTIVE_SKILL_SLOT];
	stock int giPlyPassiveSkill[MAXPLAYERS][MAX_PASSIVE_SKILL_SLOT][PLY_SKILL_INFO];
	stock float gfPlyPassiveSkillCooldown[MAXPLAYERS][MAX_PASSIVE_SKILL_SLOT];

	stock int giPlyTriggeredSkill[MAXPLAYERS][TOTAL_SKILL];
	stock float gfPlySkillTime[MAXPLAYERS][TOTAL_SKILL];

	stock Handle g_hSkillEffectTimer[MAXPLAYERS][TOTAL_SKILL];
	stock Handle g_hSkillContinuousEffectTimer[MAXPLAYERS][TOTAL_SKILL];
	stock Handle g_hBotAutoActiveSkill[MAXPLAYERS];	

	//Mid Boss

	//Boss
	#define TOTAL_BOSS_PASSIVE_SKILL_FOR_VB	8
	#define TOTAL_BOSS_PASSIVE_SKILL		TOTAL_BOSS_PASSIVE_SKILL_FOR_VB+2
	
	#if defined DEBUG
		#define MAX_BOSS_SKILL	5
	#else
		#define MAX_BOSS_SKILL	25
	#endif
	stock int BOSS_SKILL_COUNTER;
	#define BOSS_TOXIC_ID	999

	#define BIG_BANG_RANGE 1250.0



/*  Ability  */

	public Action TaskCalculatFinalAbility(Handle timer, int client)
	{
		if(IsClientValid(client) && IsPlayerAlive(client))
			CalculateFinalAbility(client);

		g_hCalculateFinalAbilityTask[client] = INVALID_HANDLE;
		return Plugin_Stop;
	}

	stock void CalculateFinalAbility(const &client, identity_override=-1, bool:spawn=false) {
		static ability, identity;
		static Float:ability_before[TOTAL_ABILITY];
		static char m_flLaggedMovementValue[] = "m_flLaggedMovementValue";

		identity = (identity_override==-1)?PLY_IDENTITY(client):identity_override;
		for(ability=1; ability<TOTAL_ABILITY; ability++) ability_before[ability] = gfPlyCurAbility[client][ability];

		if(GAME_PREPARE) {
			for(ability=1; ability<TOTAL_ABILITY; ability++) {
				gfPlyCurAbility[client][ability] = gfPlyGamePauseAbility[ability] + gfPlyFreeAbility[client][ability];
			}
		} else if(GAME_START) {
			switch(identity) {
				case IDENTITY_HUMAN: {
					for(ability=1; ability<TOTAL_ABILITY; ability++) {
						gfPlyCurAbility[client][ability] = FloatClamp(
							gfPlyDefaultAbility[client][identity][ability] 
								+ gfPlyGameProgressAbility[client][identity][ability] * gfGameProgress
								+ gfPlyFreeAbility[client][ability] 
								+ gfPlyEquipAbility[client][ability] 
								+ gfPlySkillAbility[client][ability] 
								+ gfPlyAbnormalAbility[client][ability] 
								+ giPlySP[client][ability]*SP_ABILITY_FACTOR[ability]
								+ gfPlyIdentityAmountAbility[client][identity][ability]
							, IDENTITY_ABILITY_LOW_LIMIT[identity][ability]
							, IDENTITY_ABILITY_HIGH_LIMIT[identity][ability]
						);
					}
				}
				case IDENTITY_LEADER: {
					for(ability=1; ability<TOTAL_ABILITY; ability++) {
						gfPlyCurAbility[client][ability] = FloatClamp(
							(gfPlyDefaultAbility[client][identity][ability] 
								+ gfPlyGameProgressAbility[client][identity][ability] * gfGameProgress
								+ gfPlyFreeAbility[client][ability] 
								+ gfPlyEquipAbility[client][ability] 
								+ gfPlySkillAbility[client][ability] 
								+ gfPlyAbnormalAbility[client][ability] 
								+ giPlySP[client][ability]*SP_ABILITY_FACTOR[ability]
								+ gfPlyIdentityAmountAbility[client][identity][ability]
							) * (1.0+LEADER_ABILITY_AMPIFIER[ability])
							, IDENTITY_ABILITY_LOW_LIMIT[identity][ability]
							, IDENTITY_ABILITY_HIGH_LIMIT[identity][ability]
						);
					}
				}
				case IDENTITY_ZOMBIE: {
					for(ability=1; ability<TOTAL_ABILITY; ability++) {
						gfPlyCurAbility[client][ability] = FloatClamp(
							gfPlyDefaultAbility[client][identity][ability] 
								+ gfPlyGameProgressAbility[client][identity][ability] * gfGameProgress 
								+ gfPlyFreeAbility[client][ability] 
								+ gfPlySkillAbility[client][ability] 
								+ gfPlyAbnormalAbility[client][ability] 
								+ gfPlyIdentityAmountAbility[client][identity][ability]
							 
							, IDENTITY_ABILITY_LOW_LIMIT[identity][ability]
							, IDENTITY_ABILITY_HIGH_LIMIT[identity][ability]
						);
					}
				}
				case IDENTITY_BOSS: {
					for(ability=1; ability<TOTAL_ABILITY; ability++) {
						gfPlyCurAbility[client][ability] = FloatClamp(
							gfPlyDefaultAbility[client][identity][ability] 
								+ gfPlyGameProgressAbility[client][identity][ability] * gfGameProgress 
								+ gfPlyFreeAbility[client][ability] 
								+ gfPlySkillAbility[client][ability] 
								+ gfPlyAbnormalAbility[client][ability] 
								+ gfPlyIdentityAmountAbility[client][identity][ability]
							 
							, IDENTITY_ABILITY_LOW_LIMIT[identity][ability]
							, IDENTITY_ABILITY_HIGH_LIMIT[identity][ability]
						);
					}
				}
			}

			if(IsFakeClient(client) && BOT_AMPIFIER_ON_IDENTITY[identity]) {
				for(ability=1; ability<TOTAL_ABILITY; ability++) {
					gfPlyCurAbility[client][ability] = FloatClamp(
						gfPlyCurAbility[client][ability] * (1.0+BOT_ABILITY_AMPIFIER[ability])
						, IDENTITY_ABILITY_LOW_LIMIT[identity][ability]
						, IDENTITY_ABILITY_HIGH_LIMIT[identity][ability]
					);
				}
			}
		}
		
		if(IsPlayerAlive(client)) {
			if(spawn)
			{
				SetEntityHealth(client, RoundToCeil(gfPlyCurAbility[client][ABILITY_HP_LIMIT]));
			}
			else if(gfPlyCurAbility[client][ABILITY_HP_LIMIT] != ability_before[ABILITY_HP_LIMIT])
			{
				if(gfPlyCurAbility[client][ABILITY_HP_LIMIT] < 1.0)
					gfPlyCurAbility[client][ABILITY_HP_LIMIT] = 1.0;

				new Float:health = GetClientHealth(client) * gfPlyCurAbility[client][ABILITY_HP_LIMIT] / ability_before[ABILITY_HP_LIMIT];
				SetEntityHealth (client, RoundToCeil(health));
			}

			if( gfPlyCurAbility[client][ABILITY_SPD] <= 0.0 || gbPlyFreeze[client] || GAME_END)
				SetEntPropFloat(client, Prop_Data, m_flLaggedMovementValue, 0.0);
			else
				SetEntPropFloat(client, Prop_Data, m_flLaggedMovementValue, gfPlyCurAbility[client][ABILITY_SPD]/PLAYER_BASE_SPEED);

			SetEntityGravity(client, gfPlyCurAbility[client][ABILITY_GRAV]/PLAYER_BASE_GRAVITY);
		}
	}

	stock int ParseAbilityFlags(String:flags[], Float:values[TOTAL_ABILITY])
	{
		char explode[TOTAL_ABILITY][16], split[2][16];
		int pos=0, ability, counter = ExplodeString(flags, ",", explode, TOTAL_ABILITY, 16);

		for(; pos<counter; pos++)
		{
			if(explode[pos][0] == 0)
				break;

			ExplodeString(explode[pos], "_", split, 2, 16);
			ability = GetFlagNumber(split[0][0], 1);
			values[ability] = StringToFloat(split[1]);

		}

		return counter;
	}
	
	int ParseAbilityFlagsData(char[] flags, int abilities[TOTAL_ABILITY], float values[TOTAL_ABILITY])
	{
		char explode[TOTAL_ABILITY][16], split[2][16];
		int pos=0, counter = ExplodeString(flags, ",", explode, TOTAL_ABILITY, 16);

		for(;pos<counter; pos++)
		{
			ExplodeString(explode[pos], "_", split, 2, 16);
			abilities[pos] = GetFlagNumber(split[0][0], 1);
			values[pos] = StringToFloat(split[1]);
		}

		if(pos<TOTAL_ABILITY-1)
			abilities[pos+1] = 0;

		return counter;
	}

		stock void RetrieveGamemodeAbilityData(Database db, gamemode) {
			new String:query[512];
			DBResultSet rs;
			FormatEx(query, sizeof(query), "SELECT identity_flag, type_flag, ability_values FROM Server_Default_Ability WHERE gamemode_id=%d;", gamemode);
			
			if( (rs=SQL_Query(db, query)) == null)
			{
				SetTsubakiFailState("TSUBAKI SERVER ERROR\n0xD2EE789C8E4EB119");
				SetFailState("Failed to retrieve Human Data");
				return;
			}

			new i, identity, String:identity_flag[8], String:type_flag[8], String:abilities[256];
			for(i=0; i<TOTAL_ABILITY; i++) {
				gfPlyGamePauseAbility[i] = SP_ABILITY_FACTOR[i] = 0.0;
				for(identity=0; identity<TOTAL_IDENTITY; identity++) {
					DEFAULT_IDENTITY_ABIILTY[identity][i] = IDENTITY_ABILITY_LOW_LIMIT[identity][i] = IDENTITY_ABILITY_HIGH_LIMIT[identity][i] = 0.0;
				}
			}

			i = identity = 0;
			while(rs.FetchRow())
			{
				rs.FetchString(0, identity_flag, sizeof(identity_flag));
				rs.FetchString(1, type_flag, sizeof(type_flag));
				rs.FetchString(2, abilities, sizeof(abilities));

				while(identity_flag[i] != 0) {
					identity = identity_flag[i] - 'a' + 1;

					switch(type_flag[0]) {
						case 'a':ParseAbilityFlags(abilities, gfPlyGamePauseAbility);
						case 'b':ParseAbilityFlags(abilities, DEFAULT_IDENTITY_ABIILTY[identity]);
						case 'c':ParseAbilityFlags(abilities, IDENTITY_ABILITY_LOW_LIMIT[identity]);
						case 'd':ParseAbilityFlags(abilities, IDENTITY_ABILITY_HIGH_LIMIT[identity]);
						case 'e':ParseAbilityFlags(abilities, SP_ABILITY_FACTOR);
					}

					i++;
				}
				
				i = identity_flag[0] = type_flag[0] = abilities[0] = 0;
			}

			for(i=1; i<MAXPLAYERS; i++) {
				for(identity=1; identity<TOTAL_IDENTITY; identity++) {
					for(int ability=1; ability<TOTAL_ABILITY; ability++) {
						gfPlyDefaultAbility[i][identity][ability] = DEFAULT_IDENTITY_ABIILTY[identity][ability];
					}
				}
			}

			delete rs;
		}
		


	#if defined DEBUG

		public Action DebugDisplayAbility(int client, int args) {
			ResetPlayerMenu(client);
			
			int target = client;
			target = GetPlayerAimingTarget(client);
			if(!IsClientValid(target))
			{
				PLY_MENU_TARGET(client) = target = client;
			}

			DebugDisplayAbilityMenu(client);

			return Plugin_Handled;
		}

			void DebugDisplayAbilityMenu(int client) {
				Menu menu = new Menu(DebugDisplayAbilityHandler, MenuAction_Select);

				new String:menu_item[256];
				FormatEx(menu_item, sizeof(menu_item), "【DEBUG】能力值 \n%N\n身分：%s", PLY_MENU_TARGET(client), NAME_OF_IDENTITY[PLY_IDENTITY(PLY_MENU_TARGET(client))]);
				menu.SetTitle(menu_item);

				for(int i=1; i<TOTAL_ABILITY; i++) {
					menu_item[0] = 0;
					FormatEx(menu_item, sizeof(menu_item), "%s:%.4f", NAME_OF_ABILITY[i], gfPlyCurAbility[PLY_MENU_TARGET(client)][i]);
					menu.AddItem(EMPTY, menu_item);
				}

				menu.DisplayAt(client, (PLY_MENU_PAGE(client)/MAX_MENU_ITEM)*MAX_MENU_ITEM, MENU_TIME_FOREVER);
			}
			

			public int DebugDisplayAbilityHandler(Menu menu, MenuAction action, int client, int pos) {
				if(action == MenuAction_Select)
				{
					PLY_MENU_PAGE(client) = pos;
					DebugDisplayAbilityDetail(client);
				}
				else if(action == MenuAction_End)
				{
					delete menu;
				}

				return 0;
			}

			void DebugDisplayAbilityDetail(int client) {
				int ability=PLY_MENU_PAGE(client)+1, target=PLY_MENU_TARGET(client), identity=PLY_IDENTITY(target);
				char menu_item[256];

				Panel panel = new Panel();
				FormatEx(menu_item, sizeof(menu_item), "【DEBUG】能力%s \n%N\n身分：%s", NAME_OF_ABILITY[ability], target, NAME_OF_IDENTITY[PLY_IDENTITY(target)]);
				panel.SetTitle(menu_item);
				menu_item[0] = 0;


				FormatEx(menu_item, sizeof(menu_item), "實際能力值:%.4f", gfPlyCurAbility[target][ability]);
				panel.DrawText(menu_item);
				menu_item[0] = 0;

				FormatEx(menu_item, sizeof(menu_item), "暫停時玩家能力值:%.4f", gfPlyGamePauseAbility[ability]);
				panel.DrawText(menu_item);
				menu_item[0] = 0;

				FormatEx(menu_item, sizeof(menu_item), "身分基本能力值:%.4f", gfPlyDefaultAbility[target][identity][ability] );
				panel.DrawText(menu_item);
				menu_item[0] = 0;

				FormatEx(menu_item, sizeof(menu_item), "遊戲進度能力值:%.4f", gfPlyGameProgressAbility[target][identity][ability]*gfGameProgress );
				panel.DrawText(menu_item);
				menu_item[0] = 0;

				FormatEx(menu_item, sizeof(menu_item), "玩家自由能力值:%.4f", gfPlyFreeAbility[target][ability]);
				panel.DrawText(menu_item);
				menu_item[0] = 0;

				FormatEx(menu_item, sizeof(menu_item), "玩家裝備能力值:%.4f", gfPlyEquipAbility[target][ability]);
				panel.DrawText(menu_item);
				menu_item[0] = 0;

				FormatEx(menu_item, sizeof(menu_item), "玩家異常能力值:%.4f", gfPlyAbnormalAbility[target][ability]);
				panel.DrawText(menu_item);
				menu_item[0] = 0;

				FormatEx(menu_item, sizeof(menu_item), "SP能力值:%.4f", giPlySP[target][ability]*SP_ABILITY_FACTOR[ability]);
				panel.DrawText(menu_item);
				menu_item[0] = 0;

				FormatEx(menu_item, sizeof(menu_item), "玩家數對能力影響:%.4f", gfPlyIdentityAmountAbility[client][identity][ability]);
				panel.DrawText(menu_item);
				menu_item[0] = 0;

				FormatEx(menu_item, sizeof(menu_item), "身分能力值下限:%.4f", IDENTITY_ABILITY_LOW_LIMIT[identity][ability]);
				panel.DrawText(menu_item);
				menu_item[0] = 0;

				FormatEx(menu_item, sizeof(menu_item), "身分能力值上限:%.4f", IDENTITY_ABILITY_HIGH_LIMIT[identity][ability]);
				panel.DrawText(menu_item);
				menu_item[0] = 0;

				panel.DrawItem("上一頁");
				panel.DrawItem("OK");
				panel.SetKeys(MENU_KEYS_ALL);
				panel.Send(client, DebugDisplayAbilityDetailHandler, MENU_TIME_FOREVER);

				delete panel;
			}

			public int DebugDisplayAbilityDetailHandler(Menu menu, MenuAction action, int client, int pos) {
				if(action == MenuAction_Select) {
					switch(pos) {
						case 1:{
							DebugDisplayAbilityMenu(client);
						}
					}
				}

				return 1;
			}

		// public Action DebugDisplayAbility(int client, int args) {
		
		// 	int target = client;
		// 	target = GetPlayerAimingTarget(client);
		// 	if(!IsClientValid(target))
		// 	{
		// 		target=client;
		// 	}

		// 	Panel panel = GetPanelForDisplayOnly("能力値");
		// 	char msg[256];
		// 	FormatEx(msg, sizeof(msg), "%N\n身分：%s", target, NAME_OF_IDENTITY[PLY_IDENTITY(target)]);
		// 	panel.DrawText(msg);

		// 	for(int i=1; i<TOTAL_ABILITY; i++)
		// 	{
		// 		msg[0] = 0;
		// 		FormatEx(msg, sizeof(msg), "%s:%.4f", NAME_OF_ABILITY[i], gfPlyCurAbility[target][i]);
		// 		panel.DrawText(msg);
		// 	}

		// 	msg[0]=0;
		// 	FormatEx(msg, sizeof(msg), "無敵:%s", gbPlyGodMode[client]?"True":"False");
		// 	panel.DrawText(msg);
		// 	panel.DrawItem("取消");
		// 	panel.Send(client, DummyMenuHandler, MENU_TIME_FOREVER);

		// 	return Plugin_Handled; 

		// }


	#endif

/*  Item  */

	stock bool PlayerItemTrigger(int client, bool onoff, int item_id, int level, float time)
	{
		#if defined DEBUG
			PrintToChat(client, "You try to trigger %s %s", (onoff==ON)?"ON":"OFF", NAME_OF_ITEM[item_id]);
		#endif

		if(!IsClientValid(client) || item_id <= 0 || item_id >= TOTAL_ITEM || ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE] == 0)
			return false;

		if(onoff == OFF)
		{
			ProcessPlayerItem(client, onoff, item_id, level, time);
			return true;
		}
		else if(CanPlayerTriggerItem(client, item_id))
		{
			if( (ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE]&IT_CONTINUOUS_EFFECT) && (ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE]&IT_ONLY_HEALING_ITEM) )
			{
				if( (time*=gfPlyCurAbility[client][ABILITY_ITEM_EFFECT]) > ITEM_DEFAULT_DATA[item_id][ITEM_DATA_CONTINUOUS_RATE]/100.0)
				{
					ProcessPlayerItem(client, onoff, item_id, level, time);
				}
			}
			else
			{
				ProcessPlayerItem(client, onoff, item_id, level, time);
			}

			gfPlyWeaponExp[client] += ITEM_DEFAULT_DATA[item_id][ITEM_DATA_RARE] * 20.0;
			gfPlyGameScore[client] += ITEM_DEFAULT_DATA[item_id][ITEM_DATA_RARE] * 100.0;

			/* Equip */
			if(giPlyCurrentEquip[client][GetEquipSlot(28)]==28)		//#真C5能量轉換裝置
				SetEntityHealth(client, Clamp(GetClientHealth(client)+40, 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])));		//紅藥水, +15hp instantly
			else if(giPlyCurrentEquip[client][GetEquipSlot(27)]==27)	//#C5能量增幅器
				SetEntityHealth(client, Clamp(GetClientHealth(client)+5, 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])));		//紅藥水, +15hp instantly

			return true;
		}
		
		return false;
	}

		stock bool CanPlayerTriggerItem(int client, int item_id)
		{
			if(ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE] & IT_MATERIAL)
				return false;
			if(ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE] & IT_EQUIP)
				return true;
			
			if(~ITEM_DEFAULT_DATA[item_id][ITEM_DATA_AVAILABLE_GAMEMODE] & (1<<giCurRoundMode))
				return false;
			
			if(!IsPlayerAlive(client) && (~ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE] & IT_TRIGGER_WHILE_DEAD))
				return false;

			if(~ITEM_DEFAULT_DATA[item_id][ITEM_DATA_IDENTITY] & (1<<PLY_IDENTITY(client)))
				return false;

			if(!GAME_START && (~ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE] & IT_GAME_PAUSE))
				return false;

			//Equip
			if(giPlyCurrentEquip[client][GetEquipSlot(28)]==28 && (ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE] & IT_HEAL))
				return false;

			//EQUIP
			//BACKPACK & WAREHOUSE
			switch(item_id)
			{
				case 3:		//BP拡張
				{
					if(giPlyBPSpace[client] >= MAX_BP_SPACE)
					{
						PrintToChat(client, " \x07不能再增加更多背包位置了");
						return false;
					}
				}
				case 6:		//WH拡張
				{
					if(giPlyWHSpace[client] >= MAX_WH_SPACE)
					{
						PrintToChat(client, " \x07不能再增加更多倉庫位置了");
						return false;
					}
				}
				case 18:			//王の証
				{
					if(giPlyTriggeredItemLevel[client][19]>0)
					{
						PrintToChat(client, " \x09姫の証正在發動中");
						return false;
					}
				}
				case 19:			//姫の証
				{
					if(giPlyTriggeredItemLevel[client][18]>0)
					{
						PrintToChat(client, " \x09王の証正在發動中");
						return false;		//王の証
					}
				}
				case 23:			//銀色子彈
				{
					if(giPlyTriggeredItemLevel[client][24]>0)
					{
						PrintToChat(client, " \x09金色子彈正在發動中");
						return false;
					}
				}
				case 24:			//金色子彈
				{
					if(giPlyTriggeredItemLevel[client][23]>0)
					{
						PrintToChat(client, " \x09銀色子彈正在發動中");
						return false;
					}
				}
				case 46:			//Costco套餐
				{
					int bp_space=0, total_empty_space=0;
					for(; bp_space<giPlyBPSpace[client]; bp_space++)
					{
						if(giPlyBPItem[client][bp_space][PLAYER_ITEM_ID]==0)
							total_empty_space++;
					}

					return total_empty_space>=4;
				}

			}

			return true;
		}
		
		bool ProcessPlayerItem(int client, bool onoff, int item_id, int level, float time)
		{
			static int diff;
			diff = giPlyTriggeredItemLevel[client][item_id];

			#if defined DEBUG
				PrintToChat(client, "ProcessPlayerItem : %s Level:%d (%d)  LastTime:%.2f Mode:%s", NAME_OF_ITEM[item_id], giPlyTriggeredItemLevel[client][item_id], level, time, (onoff==OFF)?"OFF":"ON");
			#endif

			if(diff==0 && onoff==OFF)
				return false;

			if( ~ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE] & IT_INSTANTLY )
			{
				if(onoff == ON)
					giPlyTriggeredItemLevel[client][item_id] = Clamp(giPlyTriggeredItemLevel[client][item_id]+level, 0, ITEM_DEFAULT_DATA[item_id][ITEM_DATA_MAX_LEVEL]);
				else if(onoff == OFF)
					giPlyTriggeredItemLevel[client][item_id] = Clamp(giPlyTriggeredItemLevel[client][item_id]-level, 0, ITEM_DEFAULT_DATA[item_id][ITEM_DATA_MAX_LEVEL]);

				diff = giPlyTriggeredItemLevel[client][item_id] - diff;

				//Reomve Item From list if level == 0
				if(giPlyTriggeredItemLevel[client][item_id] <= 0)
				{
					RemoveArrayElement(giPlyTriggeredItem[client], TOTAL_ITEM, item_id);
					gfPlyItemTime[client][item_id] = 0.0;
					RemoveAllItemTimer(client, item_id);
				}

				//Calculate Ability Value
				if(ITEM_ABILITY_LIST[item_id][0] != 0)
				{
					int pos=0;
					while(ITEM_ABILITY_LIST[item_id][pos] != 0)
					{
						gfPlyFreeAbility[client][ITEM_ABILITY_LIST[item_id][pos]] += diff * ITEM_ABILITY_VALUE[item_id][pos];
						pos++;
					}

					CalculateFinalAbility(client);
				}

				//Set Remove Task
				if(giPlyTriggeredItemLevel[client][item_id] > 0 && time != 0.0)
				{
					#if defined DEBUG
						PrintToChat(client, "Item %s Level %d", NAME_OF_ITEM[item_id], giPlyTriggeredItemLevel[client][item_id]);
					#endif
					float gametime = GetGameTime();
					if(ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE] & IT_CONTINUOUS_EFFECT)
					{
						#if defined DEBUG
							PrintToChat(client, "Creating Item %s Effect Task", NAME_OF_ITEM[item_id]);
						#endif
						//Continuous Effect Task
						if( g_hItemEffectTimer[client][item_id] == INVALID_HANDLE )
						{
							//Seperate Remove Pack and Continuous Pack
							DataPack pack = new DataPack();			
							pack.WriteCell(client);
							pack.WriteCell(item_id);
							g_hItemEffectTimer[client][item_id] = CreateTimer(ITEM_DEFAULT_DATA[item_id][ITEM_DATA_CONTINUOUS_RATE]/100.0, ProcessItemContinuousEffect, pack, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE|TIMER_DATA_HNDL_CLOSE);
						}
					}

					//Insert Item id to trigger list
					if(ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE] & (IT_PERIOD|IT_ETERNAL|IT_EQUIP))
					{
						#if defined DEBUG
							PrintToChat(client, "Inserting %s to item array", NAME_OF_ITEM[item_id]);
						#endif
						
						InsertArrayElementNoRepeat(giPlyTriggeredItem[client], TOTAL_ITEM, item_id);	
					}

					//Remove Timer
					if( ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE] & IT_PERIOD )
					{
						#if defined DEBUG
							PrintToChat(client, "Creating Item %s Remove Task", NAME_OF_ITEM[item_id]);
						#endif

						DataPack pack = new DataPack();
						pack.WriteCell(client);
						pack.WriteCell(item_id);

						if(g_hItemRemoveTimer[client][item_id] == INVALID_HANDLE)
						{
							gfPlyItemTime[client][item_id] = gametime + time;
							g_hItemRemoveTimer[client][item_id] = CreateTimer(time, RemoveItem, pack, TIMER_FLAG_NO_MAPCHANGE|TIMER_DATA_HNDL_CLOSE);
						}
						else
						{
							gfPlyItemTime[client][item_id] += time;
							KillTimer(g_hItemRemoveTimer[client][item_id], true);
							g_hItemRemoveTimer[client][item_id] = CreateTimer(gfPlyItemTime[client][item_id] - gametime, RemoveItem, pack, TIMER_FLAG_NO_MAPCHANGE|TIMER_DATA_HNDL_CLOSE);
						}
					}
				
				}

			}
			

			if(onoff == ON)
				TriggerItemInstantEffect(client, item_id);

			return true;
		}

			TriggerItemInstantEffect(int client, int item_id)
			{
				//TO-DO health function recover
				// static float health;
				// health = ITEM_DEFAULT_DATA[item_id][ITEM_DATA_INSTANT_HEAL] / 100.0 * gfPlyCurAbility[client][ABILITY_ITEM_EFFECT];

				//TO-DO Equip 
				//TO-DO ABNORMAL
				//TO-DO MISSION
				// switch(item_id)
				// {
				// 	//case 2:SetEntityHealth(client, Clamp(GetClientHealth(client)+15, 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])));		//紅藥水, +15hp instantly
				// 	case 7:SetEntityHealth(client, 255);		//DEBUG回復薬
				// 	case 3:
				// 	{
				// 		giPlyBPSpace[client] = Clamp(giPlyBPSpace[client]+1, 0, MAX_BP_SPACE);			//BP拡張
				// 		PrintToChat(client, " \x04背包位置增加至%d", giPlyBPSpace[client]);
				// 	}
				// 	case 6:
				// 	{
				// 		giPlyWHSpace[client] = Clamp(giPlyWHSpace[client]+4, 0, MAX_WH_SPACE);			//WH拡張
				// 		PrintToChat(client, " \x04倉庫位置增加至%d", giPlyWHSpace[client]);
				// 	}
				// 	//case 8:SetEntityHealth(client, Clamp(GetClientHealth(client)+55, 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])));	//BR回復薬
				// 	//case 9:SetEntityHealth(client, GetClientHealth(client)+150); 		//腐肉A
				// 	//case 10:SetEntityHealth(client, GetClientHealth(client)+250); 		//腐肉B
				// 	//case 11:SetEntityHealth(client, GetClientHealth(client)+550); 		//腐肉C
				// 	case 9:
				// 	{
				// 		health *= 0.25;
				// 		PlayerAbnormalTrigger(client, ON, 6, 1, 15.1);
				// 	}
				// 	case 10:
				// 	{
				// 		health *= 0.25;
				// 		PlayerAbnormalTrigger(client, ON, 6, 1, 25.1);
				// 	}
				// 	case 11:
				// 	{
				// 		health *= 0.25;
				// 		PlayerAbnormalTrigger(client, ON, 6, 1, 35.1);
				// 	}
				// 	case 13:		//BR AK47
				// 	{
				// 		#if defined NEW_WEAPON_SYSTEM
				// 			int gun_id = GetPlayerWeaponSlot(client, 0);
				// 			if(gun_id!=-1) RemoveEntity(gun_id);

				// 			GivePlayerItem(client, "weapon_ak47");
				// 		#else
				// 			if(GetPlayerWeaponSlot(client, 0)==-1) GivePlayerItem(client, "weapon_ak47");
				// 			else
				// 			{
				// 				for(int pos=0; pos<MAX_WEAPON_LEVEL; pos++)
				// 				{
				// 					if(giPlyWeaponBag[client][pos][WB_WEAPON_ID]==0)
				// 					{
				// 					giPlyWeaponBag[client][pos][WB_WEAPON_ID] = 28;
				// 					giPlyWeaponBag[client][pos][WB_AMMO] = 30;
				// 					giPlyWeaponBag[client][pos][WB_RESERVE_AMMO] = 90;
				// 					break;
				// 					}
				// 				}
				// 			}
				// 		#endif
				// 	}
				// 	case 14:ResetLevelAndSP(client);		//等級重置
				// 	case 16:		//精霊石
				// 	{
				// 		//SetEntityHealth(client, Clamp(GetClientHealth(client)+50, 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])));
				// 		for(int pos=0; pos<TOTAL_EQUIP_TYPE_ALL_IDENTITY; pos++)
				// 			gfPlyCurrentEquipCooldown[client][pos] = 0.0;
				// 	}
				// 	case 17:		//精霊魂石
				// 	{
				// 		SetEntityHealth(client, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT]));
				// 		for(int pos=0; pos<TOTAL_EQUIP_TYPE_ALL_IDENTITY; pos++)
				// 		{
				// 			gfPlyCurrentEquipCooldown[client][pos] = 0.0;
				// 			if(giPlyCurrentEquipRemain[client][pos] >= 0) giPlyCurrentEquipRemain[client][pos]++;
				// 		}
				// 	}
				// 	case 20:		//解毒剤
				// 	{
				// 		if(giPlyTriggerAbnormalLevel[client][6] > 0)
				// 			PlayerAbnormalTrigger(client, OFF, 6, giPlyTriggerAbnormalLevel[client][6], -1.0);
				// 	}
				// 	case 21:		//石膏
				// 	{
				// 		if(giPlyTriggerAbnormalLevel[client][14] > 0)
				// 			PlayerAbnormalTrigger(client, OFF, 14, giPlyTriggerAbnormalLevel[client][14], -1.0);

				// 	}
				// 	case 22:		//黄金の風
				// 	{
				// 		for(int i=0; i<LOADED_ABNORMAL; i++)	
				// 			if(giPlyTriggerAbnormalLevel[client][i] > 0 && (ABNORMAL_DEFAULT_DATA[i][ABNORMAL_DATA_TYPE]&AT_REMOVABLE) && ABNORMAL_DEFAULT_DATA[i][ABNORMAL_DATA_TYPE]&AT_DEBUFF)
				// 				PlayerAbnormalTrigger(client, OFF, i, giPlyTriggerAbnormalLevel[client][i], -1.0);
				// 	}
				// 	case 26:		//決闘のナカリナ（未完成）
				// 	{
				// 	}
				// 	// case 27:SetEntityHealth(client, Clamp(GetClientHealth(client)+30, 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])));		//回復薬 hp+30
				// 	// case 28:SetEntityHealth(client, Clamp(GetClientHealth(client)+50, 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])));		//好傷藥 hp+50
				// 	case 36:		//氰化物
				// 	{
				// 		//CreateTimer(0.1, PlayerSuicide, client, TIMER_FLAG_NO_MAPCHANGE);
				// 		UnlockPlayerMission(client, 47, .dontBroadcast=true, .play_sound=true);
				// 		ForcePlayerSuicide(client);
				// 		PrintToChat(client, " \x07你對這個世界生無可戀", BOSS_ID[0]);
				// 	}
				// 	case 37:		//神風
				// 	{
				// 		if(GAME_BOSS)
				// 		{
				// 			if(GAME_FINAL_FINISHED)
				// 			{
				// 				SDKHooks_TakeDamage(BOSS_ID[0], 0, 0, 99999.0, 0, .bypassHooks=false);
				// 				UnlockPlayerMission(client, 49, .play_sound=true);

				// 			}
				// 			else
				// 			{
				// 				SDKHooks_TakeDamage(BOSS_ID[0], 0, 0, 1500.0, 0, .bypassHooks=false);
				// 				UnlockPlayerMission(client, 48, .play_sound=true);
				// 			}

				// 			PrintToChat(BOSS_ID[0], " \x04%N對你使用神風攻擊", client);
				// 			PrintToChat(client, " \x07你對BOSS使用神風攻擊");
				// 		}
				// 		else
				// 		{
				// 			UnlockPlayerMission(client, 47, .dontBroadcast=true, .play_sound=true);
				// 			PrintToChat(client, " \x07你對空氣使用神風攻擊");
				// 		}
				// 		ForcePlayerSuicide(client);
				// 	}
				// 	case 38:
				// 	{
				// 		PlayerAbnormalTrigger(client, ON, 29, 10, 1.0);
				// 	}
				// 	case 39:		//我是傳奇
				// 	{
				// 		giPlyLastHitBy[client] = 0;
				// 		int target=1, hit_counter=0, kill_counter=giPlyKilledPlayerAmount[client][IDENTITY_ZOMBIE];
				// 		UnlockPlayerMission(client, 59);

				// 		for(; target<=MaxClients; target++)
				// 		{
				// 			if(IsClientValid(target) && IsPlayerZombie(target) && IsPlayerAlive(target))
				// 			{
				// 				SDKHooks_TakeDamage(target, 0, client, 9999.0, 0, .bypassHooks=false);
				// 				hit_counter++;
				// 			}
				// 		}

				// 		if(hit_counter>=1)
				// 		{
				// 			UnlockPlayerMission(client, 62);
				// 			if(hit_counter>=5)
				// 			{
				// 				UnlockPlayerMission(client, 63);
				// 				if(hit_counter>=10)
				// 				{
				// 					UnlockPlayerMission(client, 64, .dontBroadcast=false, .play_sound=true);
									
				// 					if(hit_counter>=20)
				// 						UnlockPlayerMission(client, 65, .dontBroadcast=false, .play_sound=true);
				// 				}
				// 			}
				// 		}

				// 		kill_counter = giPlyKilledPlayerAmount[client][IDENTITY_ZOMBIE] - kill_counter;
				// 		if(kill_counter>=1)
				// 		{
				// 			UnlockPlayerMission(client, 66);
				// 			if(kill_counter>=5)
				// 			{
				// 				UnlockPlayerMission(client, 67);
				// 				if(kill_counter>=12)
				// 				{
				// 					UnlockPlayerMission(client, 68);
				// 					if(kill_counter>=20)
				// 						UnlockPlayerMission(client, 58);
				// 				}
				// 			}
				// 		}

				// 		SDKHooks_TakeDamage(client, 0, 0, 99999.0, 0, .bypassHooks=false);
				// 	}
				// 	case 40:		//南瓜
				// 	{
				// 		if(GetRandomInt(0, 100) > 70)		//TRICK
				// 		{
				// 			UnlockPlayerMission(client, 60, .dontBroadcast=true);
				// 			PlayerAbnormalTrigger(client, ON,  4, 1, 10.0);
				// 			PlayerAbnormalTrigger(client, ON,  6, 2, 20.0);
				// 			PlayerAbnormalTrigger(client, ON, 25, 1, 20.0);
				// 			DisplayPlayerTempHudMessage(client, "TRICK!!!", .last_time=1.5, .color={255, 50, 255, 255});
							
				// 		}
				// 		else			//TREAT
				// 		{
				// 			UnlockPlayerMission(client, 61, .dontBroadcast=true);
				// 			PlayerAbnormalTrigger(client, ON,  7, 1, 10.0);
				// 			PlayerAbnormalTrigger(client, ON, 12, 1, 20.0);
				// 			PlayerAbnormalTrigger(client, ON, 29, 5, 1.0);
				// 			DisplayPlayerTempHudMessage(client, "TREAT!!!", .last_time=1.5, .color={50, 255, 50, 255});
				// 		}
				// 	}
				// 	case 44:
				// 	{
				// 		KillTimer(g_hPlyWeaponSupply[client]);
				// 		g_hPlyWeaponSupply[client] = INVALID_HANDLE;

				// 		if(gfPlyNextSupplyTime[client]-GetGameTime() <= 40.0)
				// 		{
				// 			PlayerSupplyTask(INVALID_HANDLE, client);
				// 		}
				// 		else
				// 		{
				// 			gfPlyNextSupplyTime[client] -= 40.0;
				// 			g_hPlyWeaponSupply[client] = CreateTimer(gfPlyNextSupplyTime[client]-GetGameTime(), PlayerSupplyTask, client, TIMER_FLAG_NO_MAPCHANGE);
				
				// 		}
				// 	}
				// 	case 46:
				// 	{
				// 		UnlockPlayerMission(client, 57, .dontBroadcast=false, .play_sound=true);
				// 		PrintToChatAll(" \x05%N - \x04等級:%d \x09- \x05我愛COSTCO!!!", client, giPlyLevel[client]);
						
				// 		KillTimer(g_hPlyWeaponSupply[client]);
				// 		g_hPlyWeaponSupply[client] = INVALID_HANDLE;

				// 		PrintToChatAll("%.4f sec", gfPlyNextSupplyTime[client]-GetGameTime());
				// 		if(gfPlyNextSupplyTime[client]-GetGameTime() <= 40.0)
				// 		{
				// 			PlayerSupplyTask(INVALID_HANDLE, client);
				// 		}
				// 		else
				// 		{
				// 			gfPlyNextSupplyTime[client] -= 40.0;
				// 			g_hPlyWeaponSupply[client] = CreateTimer(gfPlyNextSupplyTime[client]-GetGameTime(), PlayerSupplyTask, client, TIMER_FLAG_NO_MAPCHANGE);
				
				// 		}
				// 	}
				// 	case 47:PlayerAbnormalTrigger(client, OFF, 31, 10, 1.0);
				// 	case 48:PlayerAbnormalTrigger(client, OFF, 32, 10, 1.0);
				// 	case 49:
				// 	{
				// 		PlayerAbnormalTrigger(client, OFF, 31, 10, 1.0);
				// 		PlayerAbnormalTrigger(client, OFF, 32, 10, 1.0);
				// 	}
				// }


				//TO-DO Level
				// SetEntityHealth(client, 
				// 					RoundFloat(
				// 							FloatClamp(
				// 								GetClientHealth(client) + (IsPlayerNewbie(client)&&giCurRoundMode!=GAMEMODE_BR?2.0:1.0) * health , 
				// 								0.0, 
				// 								gfPlyCurAbility[client][ABILITY_HP_LIMIT])
				// 							)
				// 				);

				#if defined DEBUG
					PrintToChat(client, "Triggering %s Instant effect", NAME_OF_ITEM[item_id]);
				#endif
			}


			public Action ProcessItemContinuousEffect(Handle timer, DataPack pack)
			{
				pack.Reset();
				static int client, item_id, level;
				client = pack.ReadCell();
				item_id = pack.ReadCell();
				level = giPlyTriggeredItemLevel[client][item_id];

				// switch(item_id)
				// {
				// 	case 1, 30, 32:SetEntityHealth(client, Clamp(GetClientHealth(client)+(level), 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])) );		//造血剤
				// 	case 4:SetEntityHealth(client, Clamp(GetClientHealth(client)+(7*level), 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])) );		//再生薬
				// 	case 12:SetEntityHealth(client, Clamp(GetClientHealth(client)+(15*level), 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])) );			//BR再生薬
				// 	case 29:SetEntityHealth(client, Clamp(GetClientHealth(client)+(15*level), 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])) );		//再生薬II
				// }


				//TO-DO Level
				// SetEntityHealth(client, 
				// 					RoundFloat(
				// 							FloatClamp(
				// 								GetClientHealth(client) + (IsPlayerNewbie(client)&&giCurRoundMode!=GAMEMODE_BR?2.0:1.0) * ITEM_DEFAULT_DATA[item_id][ITEM_DATA_CONTINUOUS_HEAL] * level / 100.0, 
				// 								0.0, 
				// 								gfPlyCurAbility[client][ABILITY_HP_LIMIT])
				// 							)
				// 				);				


				#if defined DEBUG
					PrintToChat(client, "Triggering %s : %d", NAME_OF_ITEM[item_id], level);
				#endif

				return Plugin_Continue;
			}

			public Action RemoveItem(Handle timer, DataPack pack)
			{
				pack.Reset();
				int client = pack.ReadCell();
				int item_id = pack.ReadCell();
				
				#if defined DEBUG
					PrintToChat(client, "Removing %s : %s", NAME_OF_ITEM[item_id], PlayerItemTrigger(client, OFF, item_id, giPlyTriggeredItemLevel[client][item_id], 1.0)?"Success":"Failed");
				#else
					PlayerItemTrigger(client, OFF, item_id, giPlyTriggeredItemLevel[client][item_id], 1.0);
				#endif
				return Plugin_Stop;
			}

	RemoveAllItemTimer(int client, int item_id )
	{
		if(g_hItemEffectTimer[client][item_id] != INVALID_HANDLE)
		{
			KillTimer(g_hItemEffectTimer[client][item_id], true);
			g_hItemEffectTimer[client][item_id] = INVALID_HANDLE;
		}

		if(g_hItemRemoveTimer[client][item_id] != INVALID_HANDLE)
		{
			KillTimer(g_hItemRemoveTimer[client][item_id]);
			g_hItemRemoveTimer[client][item_id] = INVALID_HANDLE;
		}
	}

/*  Item Spawn  */
	
	CreateItemEntity(int item_id=-1, int durability=-1, float origin[3]=NULL_VECTOR, int owner=-1, float lasttime=15.0, int r=200, int g=200, int b=55, String:model_route[]="", int laser_sprite=-1)
	{
		if(item_id == -1)
			item_id = RANDOM_ITEM_LIST[IDENTITY_HUMAN][GetRandomInt(0, RANDOM_ITEM_AMOUNT[IDENTITY_HUMAN]-1)];

		if(durability == -1)
			durability = ITEM_DEFAULT_DATA[item_id][ITEM_DATA_DURABILITY];

		PrintToServer("%.4f %.4f %.4f", origin[0], origin[1], origin[2]);
		if(IsNullVector(origin))
		{
			if(CURRENT_MAP_ITEM_ORIGIN_AMOUNT > 0)
			{
				CopyVector(origin, CURRENT_MAP_ITEM_ORIGIN[GetRandomInt(0, CURRENT_MAP_ITEM_ORIGIN_AMOUNT-1)]);
			}
			else
			{
				return;
			}
		}
		PrintToServer("%.4f %.4f %.4f", origin[0], origin[1], origin[2]);
		new ce_index;

		do{
			ce_index = CreateTsubakiEntity(ITEM_ENTITY
								, .model_route=(strlen(model_route)==0)?DEFAULT_INVISIBLE_TOUCHER_MODEL:model_route
								, .owner=owner
								, .solid_type=DEFAULT_ITEM_SOLID
								, .movetype=MOVETYPE_NOCLIP
								, .r=255
								, .g=255
								, .b=255
								, .a=0
								, .min_size=ITEM_ENTITY_MIN_SIZE
								, .max_size=ITEM_ENTITY_MAX_SIZE
								);

			if(ce_index != -1)
			{
				tbk_iuser[ce_index][0] = item_id;
				tbk_iuser[ce_index][1] = durability;
				tbk_iuser[ce_index][2] = CreateRGBAInt(r, g, b, 255);
				tbk_iuser[ce_index][3] = RoundFloat(255.0 / (lasttime/ITEM_ENTITY_THINKTIME));
				tbk_iuser[ce_index][4] = (laser_sprite==-1)?DEFAULT_LASER_SPRITE_ID:laser_sprite;

				tbk_fuser[ce_index][0] = GetGameTime();

				//newline replaced
				CopyVector(tbk_vuser[ce_index][0], origin);

				SDKHookEx(g_iCustomEntityRef[ce_index], SDKHook_Touch, OnItemEntityTouch);
				
				SetEntityNextThink(ce_index, ITEM_ENTITY_THINKTIME, ItemEntityThink, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
				SetRemoveEntityTask(ce_index, lasttime);

				TeleportEntity(g_iCustomEntityRef[ce_index], origin, NULL_VECTOR, NULL_VECTOR);

				break;
			}

		}while(ce_index == -1);
	}

		public OnItemEntityTouch(int ent, int target)
		{
			#if defined DEBUG
				PrintToServer("Item Entity Touching %N", target);
			#endif

			static ce_index;

			if(target!=GetEntityOwner(ent) && IsClientValid(target) && IsPlayerHuman(target) && IsPlayerAlive(target))
			{
				int pos = GetPlayerBackpackEmptySpace(target);
				if(pos == -1) 
					return;

				if( (ce_index=EntRefToCustomEntityIndex(EntIndexToEntRef(ent))) == -1 )
					return;

				if(GivePlayerBPItem(target, tbk_iuser[ce_index][0], tbk_iuser[ce_index][1], .give_only=true))
				{
					RemoveTsubakiEntity(INVALID_HANDLE, ce_index);
				}

			}

		}

		public Action ItemEntityThink(Handle timer, int ce_index)
		{
			#if defined DEBUG
				PrintToServer("Item Entity Thinking");
			#endif

			static float start[3], end[3];
			static ref;

			ref = g_iCustomEntityRef[ce_index];

			//newline replaced
			CopyVector(start, tbk_vuser[ce_index][0]);

			//newline replaced
			CopyVector(end, tbk_vuser[ce_index][0]);
			end[2] += ITEM_ENTITY_SEARCH_RANGE*1.25;

			TBKLaserTE(.sprite_id=tbk_iuser[ce_index][4]
						, .start=start
						, .end=end
						, .life=ITEM_ENTITY_THINKTIME
						, .width=5.0
						, .end_width=5.0
						, .r=GetRGB_R(tbk_iuser[ce_index][2])
						, .g=GetRGB_G(tbk_iuser[ce_index][2])
						, .b=GetRGB_B(tbk_iuser[ce_index][2])
						, .a=GetRGBA_A(tbk_iuser[ce_index][2])
						, .speed=10);
			tbk_iuser[ce_index][2] = CreateRGBAInt(.R=GetRGB_R(tbk_iuser[ce_index][2]) 
												, .G=GetRGB_R(tbk_iuser[ce_index][2])
												, .B=GetRGB_B(tbk_iuser[ce_index][2])
												, .A=Clamp( GetRGBA_A(tbk_iuser[ce_index][2])-tbk_iuser[ce_index][3], 0, 255)
												);
			
			//Let owner to get it back
			if(GetEntityOwner(ref)!=0 && GetGameTime() > tbk_fuser[ce_index][0]+1.9)
			{
				SetEntityOwner(ref, 0);
			}


			return Plugin_Continue;
		}

	public Action CreateRandomItemEntityOnMap(Handle timer)
	{
		if(gbsGameStatus & (GAME_STATUS_PREPARE|GAME_STATUS_END|GAME_STATUS_PROGRESS_COMPLETE|GAME_STATUS_BOSS))
			return Plugin_Stop;

		int i, total = CURRENT_MAP_DIFFICULTY * GetRandomInt(1, 3);
		float origin[3];

		for(i=0; i<total; i++)
		{	
			if(CURRENT_MAP_ITEM_ORIGIN_AMOUNT > 0)
			{
				//newline replaced
				CopyVector(origin, CURRENT_MAP_ITEM_ORIGIN[GetRandomInt(0, CURRENT_MAP_ITEM_ORIGIN_AMOUNT-1)]);
				origin[2] += 10.0;
			}

			CreateItemEntity(.origin=origin);
		}

		return Plugin_Continue;
	}

	stock RetrieveItemData(Database db) {
		
		char query[1024];
		DBResultSet rs;
		FormatEx(query, sizeof(query), "SELECT id,"					//0	
									..." name, "					//1
									..." description, "				//2
									..." durability, "				//3
									..." rare, "					//4
									..." last_time, "				//5
									..." continuous_effect_rate, "	//6
									..." gamemode_flag, "			//7
									..." identity_flag, "			//8
									..." type_flag, "				//9
									..." ability_affect, "			//10
									..." per_ability_affect, "		//11
									..." max_level, "				//12
									..." appear_gamemode, "			//13
									..." appear_identity, "			//14
									..." hp_recover_instantly,"		//15
									..." hp_recover_per_period, "	//16
									..." sell_price, "				//17
									..." appear_in_store"			//18
									..." FROM Server_Item_Info "
									..." ORDER BY display_id");

		if( (rs=SQL_Query(db, query)) == null )
		{
			LogError("Failed to retrieve Item Data\n%s", query);
			SetTsubakiFailState("TSUBAKI SERVER ERROR\n0x35E2761DCE478BE0");
			SetFailState("Failed to Retrieve Item Data");
			return;
		}
		
		int id;
		char flags[64], ability_flags[256];
		LOADED_ITEM_AMOUNT = GAMBLE_ITEM_AMOUNT = GREAT_GAMBLE_ITEM_AMOUNT = RANDOM_ITEM_AMOUNT_IN_STORE = 0;

		for(id=0; id<TOTAL_IDENTITY; id++)
			RANDOM_ITEM_AMOUNT[id]=RANDOM_ITEM_AMOUNT_AFTER_KILL[id]=0;

		while(rs.FetchRow())
		{
			id = rs.FetchInt(0);
			ITEM_DEFAULT_DATA[id][ITEM_DATA_TYPE] 			= rs.FetchInt(0);
			rs.FetchString(1, NAME_OF_ITEM[id], 32);
			rs.FetchString(2, DESCRIPTION_OF_ITEM[id], 64);
			ITEM_DEFAULT_DATA[id][ITEM_DATA_DURABILITY] 	= rs.FetchInt(3);
			ITEM_DEFAULT_DATA[id][ITEM_DATA_RARE] 			= rs.FetchInt(4);
			ITEM_DEFAULT_DATA[id][ITEM_DATA_LAST_TIME] 		= RoundFloat(rs.FetchFloat(5)*100.0);
			ITEM_DEFAULT_DATA[id][ITEM_DATA_CONTINUOUS_RATE] = RoundFloat(rs.FetchFloat(6)*100.0);
			ITEM_DEFAULT_DATA[id][ITEM_DATA_MAX_LEVEL] = rs.FetchInt(12);
			ITEM_DEFAULT_DATA[id][ITEM_DATA_INSTANT_HEAL] = RoundFloat(rs.FetchFloat(15)*100.0);
			ITEM_DEFAULT_DATA[id][ITEM_DATA_CONTINUOUS_HEAL] = RoundFloat(rs.FetchFloat(16)*100.0);
			ITEM_DEFAULT_DATA[id][ITEM_DATA_SELL_PRICE] = rs.FetchInt(17);

			if(rs.FetchInt(18) >= 1)
				RANDOM_ITEM_IN_STORE[RANDOM_ITEM_AMOUNT_IN_STORE] = id;
			
			flags[0] = 0;
			rs.FetchString(7, flags, 64);
			FlagsToBitsum(flags, ITEM_DEFAULT_DATA[id][ITEM_DATA_AVAILABLE_GAMEMODE], 1);

			flags[0] = 0;
			rs.FetchString(8, flags, 64);
			FlagsToBitsum(flags, ITEM_DEFAULT_DATA[id][ITEM_DATA_IDENTITY], 1);

			flags[0] = 0;
			rs.FetchString(9, flags, 64);
			FlagsToBitsum(flags, ITEM_DEFAULT_DATA[id][ITEM_DATA_TYPE], 1);

			ability_flags[0] = 0;
			rs.FetchString(10, ability_flags, 256);
			if(strlen(ability_flags) > 2)
			{
				ParseAbilityFlagsData(ability_flags, ITEM_ABILITY_LIST[id], ITEM_ABILITY_VALUE[id]);
			}

			if(ITEM_DEFAULT_DATA[id][ITEM_DATA_RARE] <= MAX_APPEAR_ITEM_RARE)
			{
				int i, identity, pos;
				char identity_flags[32];
				rs.FetchString(14, identity_flags, sizeof(identity_flags));
				
				pos = flags[0] = 0;
				rs.FetchString(13, flags, sizeof(flags));

				if(FindCharInString(flags, 'a'+giCurRoundMode-1) != -1) 		//Search if CurRoundMode inside appear_gamemode_flag
				{
					while( 'a' <= identity_flags[pos] <= 'z')			//Damage and on ground item
					{
						if('a' <= identity_flags[pos] < 'a'+TOTAL_IDENTITY-1)
						{
							identity = identity_flags[pos] - 'a' + 1;

							for(i=ITEM_DEFAULT_DATA[id][ITEM_DATA_RARE]; i<=5; i++)
								RANDOM_ITEM_LIST[identity][RANDOM_ITEM_AMOUNT[identity]++] = id;
						}
						else if('p' <= identity_flags[pos] < 'p'+TOTAL_IDENTITY-1)						
						//Killed appear item	p-human q-leader r-zombie s-boss
						{
							identity = identity_flags[pos] - 'p' + 1;

							for(i=ITEM_DEFAULT_DATA[id][ITEM_DATA_RARE]; i<=5; i++)
								RANDOM_ITEM_LIST_AFTER_KILL[identity][RANDOM_ITEM_AMOUNT_AFTER_KILL[identity]++] = id;
						}

						pos++;
					}
				}
			}

			if(ITEM_DEFAULT_DATA[id][ITEM_DATA_TYPE] & IT_GAMBLE) GAMBLE_ITEM[GAMBLE_ITEM_AMOUNT++] = id;
			if(ITEM_DEFAULT_DATA[id][ITEM_DATA_TYPE] & IT_GREAT_GAMBLE) GREAT_GAMBLE_ITEM[GREAT_GAMBLE_ITEM_AMOUNT++] = id;

			ITEM_DISPLAY_SORT[LOADED_ITEM_AMOUNT++] = id;
		}
	}

	void DisplayItemDictMenu(int client, bool:display_all=false, bool:is_admin=false) {
		if(is_admin) PLY_MENU_ID(client) = MENUID_ITEM_DICT_ADMIN;
		else if(display_all) PLY_MENU_ID(client) = MENUID_ITEM_DICT_DISPLAY_ALL;

		new i=0, pos=0;
		Panel panel = new Panel();

		if(is_admin){
			new String:menu_msg[64];
			FormatEx(menu_msg, sizeof(menu_msg), "物品圖鑑【管理員】　対象:%N", PLY_MENU_TARGET(client));
 			panel.SetTitle(menu_msg);
		}
		else if(display_all) 	panel.SetTitle("物品圖鑑【全物品】");
		else 					panel.SetTitle("物品圖鑑");

		for(; i<MAX_MENU_ITEM && (pos=PLY_MENU_PAGE(client)*MAX_MENU_ITEM+i)<LOADED_ITEM_AMOUNT; i++) {
			//TO-DO Check if player unlocked item
			if(display_all || is_admin)
				panel.DrawItem(NAME_OF_ITEM[ITEM_DISPLAY_SORT[pos]]);
			else
				panel.DrawItem(UNKNOWN_ITEM_NAME);
		}

		AddPanelPageController(panel, client, LOADED_ITEM_AMOUNT);

		panel.SetKeys(MENU_KEYS_ALL);
		panel.Send(client, ItemDictMenuHandler, MENU_TIME_FOREVER);
		delete panel;
	}

		public int ItemDictMenuHandler(Menu menu, MenuAction action, int client, int pos) {
			if(action == MenuAction_Select) {
				switch(pos) {
					case 7: {
						if(PanelHasPreviousPage(client)) {
							PLY_MENU_PAGE(client)--;
							DisplayItemDictMenu(client, .display_all=(PLY_MENU_ID(client)==MENUID_ITEM_DICT_DISPLAY_ALL), .is_admin=(PLY_MENU_ID(client)==MENUID_ITEM_DICT_ADMIN));
						}
					}
					case 8: {
						if(PanelHasNextPage(client, LOADED_ITEM_AMOUNT)) {
							PLY_MENU_PAGE(client)++;
							DisplayItemDictMenu(client, .display_all=(PLY_MENU_ID(client)==MENUID_ITEM_DICT_DISPLAY_ALL), .is_admin=(PLY_MENU_ID(client)==MENUID_ITEM_DICT_ADMIN));
						}
					}
					case 9:return 1;
					default: {
						//TO-DO Check if player has item
						if(ITEM_DISPLAY_SORT[GetPanelItemPos(client,pos-1)] < LOADED_ITEM_AMOUNT)
							DisplayItemDetailMenu(client, ITEM_DISPLAY_SORT[GetPanelItemPos(client,pos-1)], .is_admin=(PLY_MENU_ID(client)==MENUID_ITEM_DICT_ADMIN));
					}

				}
			}

			return 1;
		}

	void DisplayItemDetailMenu(client, item_id, bool:is_admin=false) {
		
		static item_type_bit[] = {
			IT_REMOVABLE,
			IT_ETERNAL,
			IT_PERIOD,
			IT_EQUIP,
			IT_INSTANTLY,
			IT_CONTINUOUS_EFFECT,
			IT_GAME_PAUSE,
			IT_TRIGGER_WHILE_DEAD,
			0
		};

		static String:item_type_name[][] = {
			"可移除"
			,"永久性"
			,"時間性"
			,"装備"
			,"一次性"
			,"持続効果"
			,"暫停時可使用"
			,"死亡時可使用"
		};

		new String:msg[256];
		new i=0, cnt=0;
		PLY_MENU_ITEM(client) = item_id;

		if(is_admin) FormatEx(msg, sizeof(msg), "物品詳細　対象:%N", PLY_MENU_TARGET(client));
		else FormatEx(msg, sizeof(msg), "物品詳細");

		Panel panel = GetPanelForDisplayOnly(msg, .cancel_only=false);
		panel.DrawText(EMPTY_LINE);
		msg[0]=0; FormatEx(msg, sizeof(msg), "アイテム名:%s", NAME_OF_ITEM[item_id]);
		panel.DrawText(msg);
		msg[0]=0; FormatEx(msg, sizeof(msg), "説明：%s", DESCRIPTION_OF_ITEM[item_id]); 
		panel.DrawText(msg);
		panel.DrawText(EMPTY_LINE);
		
		msg[0]=0;
		FormatEx(msg, sizeof(msg), "身分:");
		for(i=1; i<TOTAL_IDENTITY; i++) {
			if(ITEM_DEFAULT_DATA[item_id][ITEM_DATA_IDENTITY]&(1<<i))
				Format(msg, sizeof(msg), "%s%s,", msg, NAME_OF_IDENTITY[i]);
		}
		panel.DrawText(msg);

		msg[0] = 0;
		for(; item_type_bit[i]!=0; i++) {
			if(ITEM_DEFAULT_DATA[item_id][ITEM_DATA_TYPE]&item_type_bit[i]){
				Format(msg, sizeof(msg), "%s|%s", msg, item_type_name[i]);
				cnt++;
			}
			
			if(cnt%4==0 && cnt!=0) {
				panel.DrawText(msg);
				cnt = msg[0] = 0;
			}
		}

		if(cnt != 0) 
			panel.DrawText(msg);


		msg[0] = 0; FormatEx(msg, sizeof(msg), "能力"); 
		panel.DrawText(msg);
		for(i=0; i<TOTAL_ABILITY && ITEM_ABILITY_LIST[item_id][i] != 0; i++)
		{
			msg[0] = 0; FormatEx(msg, sizeof(msg), "%s:%.4f(%d)", NAME_OF_ABILITY[ITEM_ABILITY_LIST[item_id][i]], ITEM_ABILITY_VALUE[item_id][i], ITEM_ABILITY_LIST[item_id][i]); 
			panel.DrawText(msg);
		}
		panel.DrawItem("返回目錄");

		if(is_admin)
		{
			panel.DrawItem("發送到背包");
			panel.DrawItem("發送到倉庫");
			panel.DrawItem("即時使用");
			panel.DrawItem("生成物品實體");
		}

		panel.SetKeys(MENU_KEYS_ALL);
		panel.Send(client, ItemDetailMenuHandler, MENU_TIME_FOREVER);

		CloseHandle(panel);
	}

		public int ItemDetailMenuHandler(Menu menu, MenuAction action, int client, int pos) {
			
			if(action == MenuAction_Select) {
				if(pos == 1) {
					DisplayItemDetailMenu(client, PLY_MENU_ITEM(client));
				} else if(PLY_MENU_ID(client) == MENUID_ITEM_DICT_ADMIN) {
					switch(pos) {
						case 2:{
							//TO-DO Add Item to Player Backpack
							GivePlayerBPItem(PLY_MENU_TARGET(client), PLY_MENU_ITEM(client), .give_only=true);
						}
						case 3:{
							GivePlayerWHItem(PLY_MENU_TARGET(client), PLY_MENU_ITEM(client));
						}
						case 4:{
							PlayerItemTrigger(client, ON, PLY_MENU_ITEM(client), 1, 20.0);
							DisplayItemDetailMenu(client, PLY_MENU_ITEM(client), .is_admin=true);
						}
						case 5: {
							new Float:origin[3];
							GetPlayerAimOrigin(client, origin);
							origin[2] += 5.0;

							CreateItemEntity(.item_id=PLY_MENU_ITEM(client), .origin=origin );
						}
						default: {
							return 1;
						}	
					}
					DisplayItemDetailMenu(client, PLY_MENU_ITEM(client), .is_admin=true);
				}

				switch(pos) {
					case 1: DisplayItemDictMenu(client, .display_all=(PLY_MENU_ID(client) == MENUID_ITEM_DICT_DISPLAY_ALL), .is_admin=(PLY_MENU_ID(client)==MENUID_ITEM_DICT_ADMIN));

				}
			}


			return 1;
		}

	#if defined DEBUG

		public Action DebugDisplayItemInfoMenu(int client, int args)
		{
			// if(!gbPlyDeveloper[client])
			// 	return Plugin_Continue;
			ResetPlayerMenu(client);

			PLY_MENU_TARGET(client) = GetPlayerAimingTarget(client);
			if(PLY_MENU_TARGET(client) == -1) PLY_MENU_TARGET(client) = client;

			DisplayItemDictMenu(client, .is_admin=true);
			return Plugin_Handled;
		}

	#endif


/*  Abnormal  */

	bool PlayerAbnormalTrigger(int client, bool onoff, int abnormal, int level, float time, int triggered_player=0)
	{
		#if defined DEBUG
			PrintToChat(client, "Try to %s Abnormal %s (%d : %.4f)", onoff?"ON":"OFF", NAME_OF_ABNORMAL[abnormal], level, time);
		#endif

		if(!IsClientValid(client) || level == 0)
			return false;

		if(abnormal <= 0 || abnormal >= TOTAL_ABNORMAL)
			return false;

		if(onoff == OFF)
		{
			ProcessPlayerAbnormal(client, onoff, abnormal, level, time);
			giPlyAbnormalTriggeredPlayer[client][abnormal] = 0;
		}
		else if (CanPlayerTriggerAbnormal(client, abnormal) )
		{
			ProcessPlayerAbnormal(client, onoff, abnormal, level, time);

			// TO-DO EQUIP
			// if( (ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_TYPE]&AT_INSURANCE_TARGET) && giPlyCurrentEquip[client][GetEquipSlot(33)] == 33 ) 
			// {
			// 	giEquipParam[client][GetEquipSlot(33)] += DEBUFF_INSURANCE_AMOUNT;
			// 	GivePlayerCash(client, DEBUFF_INSURANCE_AMOUNT);
			// }

			giPlyAbnormalTriggeredPlayer[client][abnormal] = triggered_player;
		}
		else
			return false;
		
		return true;
	}

		#if defined DEBUG
		bool CanPlayerTriggerAbnormal(int client, int abnormal)
		{
			PrintToChat(client, "Checking Mode");
			//If Current mode cannot activate abnormal
			if(~ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_AVAILABLE_GAMEMODE] & (1<<giCurRoundMode) )
				return false;
			
			PrintToChat(client, "Check if its equip");
			//Equip Abnormal must be activated
			if(ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_TYPE] & AT_EQUIP)
				return true;

			//If Player is dead and Abnormal cannot be triggered while dead
			if( !IsPlayerAlive(client) )
			{
				PrintToChat(client, "Check if it can be triggered by dead");
				if(~ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_TYPE] & AT_TRIGGER_WHILE_DEAD)
					return false;
			}
				
			PrintToChat(client, "Checking Identity");
			//Player Identity cannot trigger abnormal
			if(~ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_IDENTITY] & (1<<PLY_IDENTITY(client)) )
				return false;
				
			PrintToChat(client, "Check Game Pause");
			//Game Pause and cannot be triggered while game pause
			if( !GAME_START && (~ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_TYPE] & AT_GAME_PAUSE) )
				return false;

			//TO-DO EQUIP
			// if((giPlyTriggeredItemLevel[client][41]>0||giPlyTriggeredItemLevel[client][46]>0) && (ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_TYPE]&AT_DEBUFF) )
			// {
			// 	char msg[64];
			// 	FormatEx(msg, sizeof(msg), "防禦了異常【%s】", NAME_OF_ABNORMAL[abnormal]);
			// 	DisplayPlayerTempHudMessage(client, msg);
			// 	return false;
			// }	
			
			PrintToChat(client, "Success");
			return true;
		}
		#else
		bool CanPlayerTriggerAbnormal(int client, int abnormal)
		{
			//If Current mode cannot activate abnormal
			if(~ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_AVAILABLE_GAMEMODE] & (1<<giCurRoundMode) )
				return false;

			//Equip Abnormal must be activated
			if(ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_TYPE] & AT_EQUIP)
				return true;

			//If Player is dead and Abnormal cannot be triggered while dead
			if( !IsPlayerAlive(client) && (~ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_TYPE] & AT_TRIGGER_WHILE_DEAD) )
				return false;
				
			//Player Identity cannot trigger abnormal
			if(~ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_IDENTITY] & (1<<PLY_IDENTITY(client)) )
				return false;
				
			//Game Pause and cannot be triggered while game pause
			if( !GAME_START && (~ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_TYPE] & AT_GAME_PAUSE) )
				return false;


			//TO-DO EQUIP
			// if((giPlyTriggeredItemLevel[client][41]>0||giPlyTriggeredItemLevel[client][46]>0) && (ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_TYPE]&AT_DEBUFF) )
			// {
			// 	char msg[64];
			// 	FormatEx(msg, sizeof(msg), " 防禦了異常【%s】", NAME_OF_ABNORMAL[abnormal]);
			// 	DisplayPlayerTempHudMessage(client, msg);
			// 	return false;
			// }

			return true;
		}
		#endif

		ProcessPlayerAbnormal(int client, bool onoff, int abnormal, int level, float time)
		{
			#if defined DEBUG
				PrintToChat(client, "Process %s Abnormal %s[%d] (%d : %.4f)", onoff?"ON":"OFF", NAME_OF_ABNORMAL[abnormal], abnormal, level, time);
			#endif


			int diff = giPlyTriggerAbnormalLevel[client][abnormal];

			if(diff == 0 && onoff == OFF)
				return true;

			//Calculate Abnormal Level
			if(onoff == ON)
				giPlyTriggerAbnormalLevel[client][abnormal] = Clamp(giPlyTriggerAbnormalLevel[client][abnormal]+level, 0, ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_MAX_LEVEL]);
			else
				giPlyTriggerAbnormalLevel[client][abnormal] = Clamp(giPlyTriggerAbnormalLevel[client][abnormal]-level, 0, ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_MAX_LEVEL]);

			//Insert abnormal into Player Abnormal List
			if(diff==0 && onoff==ON)
				InsertArrayElementNoRepeat(giPlyTriggerAbnormal[client], TOTAL_ABNORMAL, abnormal);

			//Calculate the difference before after
			diff = giPlyTriggerAbnormalLevel[client][abnormal] - diff;

			//If abnormal is off, remove all data
			if(giPlyTriggerAbnormalLevel[client][abnormal] <= 0)
			{
				RemoveArrayElement(giPlyTriggerAbnormal[client], TOTAL_ABNORMAL, abnormal);
				gfPlyAbnormalTime[client][abnormal] = 0.0;
				RemoveAllAbnormalTimer(client, abnormal);
			}

			//Abnormal Ability Calculate
			if(diff!=0 && ABNORMAL_ABILITY_LIST[abnormal][0] != -1)
			{
				int pos=0;

				while(pos<TOTAL_ABILITY && ABNORMAL_ABILITY_LIST[abnormal][pos] > 0)
				{
					#if defined DEBUG
						PrintToServer("Ability %d:%.4f", ABNORMAL_ABILITY_LIST[abnormal][pos], diff * ABNORMAL_ABILITY_VALUE[abnormal][pos]);
					#endif
					gfPlyAbnormalAbility[client][ABNORMAL_ABILITY_LIST[abnormal][pos]] += diff * ABNORMAL_ABILITY_VALUE[abnormal][pos];
					pos++;
				}

				CalculateFinalAbility(client);
			}

			//Create Effect or Remove Timer
			if(giPlyTriggerAbnormalLevel[client][abnormal] > 0 && time != 0.0)
			{
				float gametime = GetGameTime();

				if(ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_TYPE] & (AT_CONTINUOUS_EFFECT))
				{
					if(g_hAbnormalEffectTimer[client][abnormal] == INVALID_HANDLE)
					{
						DataPack pack = new DataPack();
						pack.WriteCell(client);
						pack.WriteCell(abnormal);
						
						g_hAbnormalEffectTimer[client][abnormal] = CreateTimer(ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_CONTINUOUS_EFFECT_RATE]/100.0, ProcessAbnormalContinuousEffect, pack, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
					}
				}

				if(ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_TYPE] & (AT_ETERNAL|AT_EQUIP) )
				{
					gfPlyAbnormalTime[client][abnormal] = time;
				}
				else
				{
					DataPack pack = new DataPack();
					pack.WriteCell(client);
					pack.WriteCell(abnormal);
						
					if(g_hAbnormalRemoveTimer[client][abnormal] == INVALID_HANDLE)
					{
						gfPlyAbnormalTime[client][abnormal] = gametime + time;
						g_hAbnormalRemoveTimer[client][abnormal] = CreateTimer(time, RemoveAbnormal, pack, TIMER_DATA_HNDL_CLOSE|TIMER_FLAG_NO_MAPCHANGE);
					}
					else
					{
						KillTimer(g_hAbnormalRemoveTimer[client][abnormal], true);
						gfPlyAbnormalTime[client][abnormal] += time;
						g_hAbnormalRemoveTimer[client][abnormal] = CreateTimer(gfPlyAbnormalTime[client][abnormal]-gametime, RemoveAbnormal, pack, TIMER_DATA_HNDL_CLOSE|TIMER_FLAG_NO_MAPCHANGE);
					}
				}
			}
			
			TriggerAbnormalInstantEffect(client, abnormal);

			return false;
		}

		//One time abnormal Effect
		TriggerAbnormalInstantEffect(const int &client, const int& abnormal)
		{
			//TO-DO WEAPON
			//TO-DO EQUIP
			// switch(abnormal)
			// {
			// 	case 2:		//睡眠
			// 	{
			// 		if(giPlyTriggerAbnormalLevel[client][abnormal] > 0) 
			// 		{
			// 			#if defined NEW_WEAPON_SYSTEM
			// 				GetPlayerCurrentWeaponInfo(client, 0, giPlyWeaponBeforeSleep[client][0][WB_WEAPON_ID], giPlyWeaponBeforeSleep[client][0][WB_AMMO], giPlyWeaponBeforeSleep[client][0][WB_RESERVE_AMMO]);
			// 				GetPlayerCurrentWeaponInfo(client, 1, giPlyWeaponBeforeSleep[client][1][WB_WEAPON_ID], giPlyWeaponBeforeSleep[client][1][WB_AMMO], giPlyWeaponBeforeSleep[client][1][WB_RESERVE_AMMO]);
			// 			#else
			// 				GetPlayerCurrentWeaponInfo(client, 0, giPlyWeaponBeforeSleep[client][0]);	
			// 				GetPlayerCurrentWeaponInfo(client, 1, giPlyWeaponBeforeSleep[client][1]);
			// 			#endif
			// 			StripPlayerWeapon(client);
			// 			ScreenFade(client, 1, 0, FFADE_STAYOUT|FFADE_PURGE, {0, 0, 0, 220});
			// 			SetEntityRenderColor(client, 255, 255, 255, 0);
			// 		}
			// 		else
			// 		{
			// 			if(g_hEntityThinkTask[client] != INVALID_HANDLE)
			// 			{
			// 				KillTimer(g_hEntityThinkTask[client]);
			// 				g_hEntityThinkTask[client] = INVALID_HANDLE;
			// 			}
			// 			RemoveEntity(g_iCustomEntityRef[client]);
			// 			g_iCustomEntityRef[client] = 0;

			// 			#if defined NEW_WEAPON_SYSTEM
			// 				GivePlayerWeaponByInfo(client, giPlyWeaponBeforeSleep[client][0][WB_WEAPON_ID], giPlyWeaponBeforeSleep[client][0][WB_AMMO]);
			// 				GivePlayerWeaponByInfo(client, giPlyWeaponBeforeSleep[client][1][WB_WEAPON_ID], giPlyWeaponBeforeSleep[client][1][WB_AMMO]);
			// 			#else
			// 				GivePlayerWeaponByInfo(client, giPlyWeaponBeforeSleep[client][0]);
			// 				GivePlayerWeaponByInfo(client, giPlyWeaponBeforeSleep[client][1]);
			// 			#endif
			// 			GivePlayerItem(client, WEAPON_KNIFE);
			// 			ScreenFade(client, 100, 0, FFADE_IN|FFADE_PURGE, {160, 160, 160, 70});
			// 			SetEntityRenderColor(client, 255, 255, 255, 255);

			// 		} 

			// 		DeterminePlayerFreeze(client);
			// 	}
			// 	case 3:		//重傷昏迷
			// 	{
			// 		if(giPlyTriggerAbnormalLevel[client][abnormal] > 0)
			// 		{
			// 			if(giPlyTriggerAbnormalLevel[client][2] == 0)
			// 			{
			// 				#if defined NEW_WEAPON_SYSTEM
			// 					GetPlayerCurrentWeaponInfo(client, 0, giPlyWeaponBeforeSleep[client][0][WB_WEAPON_ID], giPlyWeaponBeforeSleep[client][0][WB_AMMO], giPlyWeaponBeforeSleep[client][0][WB_RESERVE_AMMO]);
			// 					GetPlayerCurrentWeaponInfo(client, 1, giPlyWeaponBeforeSleep[client][1][WB_WEAPON_ID], giPlyWeaponBeforeSleep[client][1][WB_AMMO], giPlyWeaponBeforeSleep[client][1][WB_RESERVE_AMMO]);
			// 				#else
			// 					GetPlayerCurrentWeaponInfo(client, 0, giPlyWeaponBeforeSleep[client][0]);	
			// 					GetPlayerCurrentWeaponInfo(client, 1, giPlyWeaponBeforeSleep[client][1]);
			// 				#endif
			// 			}
			// 			else
			// 				PlayerAbnormalTrigger(client, OFF, 2, 1, -1.0);

			// 			StripPlayerWeapon(client);
			// 			ScreenFade(client, 1, 0, FFADE_STAYOUT|FFADE_PURGE, {255, 255, 255, 180});
			// 		}
			// 		else
			// 		{
			// 			#if defined NEW_WEAPON_SYSTEM
			// 				GivePlayerWeaponByInfo(client, giPlyWeaponBeforeSleep[client][0][WB_WEAPON_ID], giPlyWeaponBeforeSleep[client][0][WB_AMMO]);
			// 				GivePlayerWeaponByInfo(client, giPlyWeaponBeforeSleep[client][1][WB_WEAPON_ID], giPlyWeaponBeforeSleep[client][1][WB_AMMO]);
			// 			#else
			// 				GivePlayerWeaponByInfo(client, giPlyWeaponBeforeSleep[client][0]);
			// 				GivePlayerWeaponByInfo(client, giPlyWeaponBeforeSleep[client][1]);
			// 			#endif

			// 			GivePlayerItem(client, WEAPON_KNIFE);
 			// 			ScreenFade(client, 100, 0, FFADE_IN|FFADE_PURGE, {255, 255, 255, 180});
			// 		}

			// 		DeterminePlayerFreeze(client);
			// 		DeterminePlayerGodMode(client);
			// 	}
			// 	case 4:
			// 	{
			// 		DeterminePlayerFreeze(client);
			// 	}
			// 	case 15:	//無敵
			// 	{
			// 		DeterminePlayerGodMode(client);
			// 	}
			// 	case 21:	//隱形
			// 	{
			// 		SetEntityRenderColor(client, 255, 255, 255, 255-giPlyTriggerAbnormalLevel[client][abnormal]);
			// 	}
			// 	case 24:	//飛行
			// 	{
			// 		if(giPlyTriggerAbnormalLevel[client][abnormal] > 0)
			// 			SetEntityMoveType(client, MOVETYPE_FLY);
			// 		else
			// 			SetEntityMoveType(client, MOVETYPE_WALK);
			// 	}
			// 	case 25:	//燃焼
			// 	{
			// 		if(giPlyTriggerAbnormalLevel[client][abnormal] > 0)
			// 			IgniteEntity(client, ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_CONTINUOUS_EFFECT_RATE]/100.0-0.1);
			// 		else
			// 			ExtinguishEntity(client);
			// 	}
			// 	case 26, 27:	//治療
			// 	{
			// 		DeterminePlayerFreeze(client);
			// 	}
			// 	case 28:		//偽装
			// 	{
			// 		if(giPlyTriggerAbnormalLevel[client][abnormal] > 0)
			// 		{
			// 			SetEntityModel(client, PLAYER_MODEL_PATH[GetRandomInt(0, LOADED_MODEL-1)]);
			// 		}
			// 		else
			// 		{
			// 			if(IsPlayerZombie(client))
			// 				SetEntityModel(client, ZOMBIE_MODEL[giPlyCurZombieType[client]]);
			// 		}
			// 	}
			// }
			#if defined DEBUG
				PrintToChat(client, "Trigger %s One Time Abnormal Effect", NAME_OF_ABNORMAL[abnormal]);
			#endif
		}

		RemoveAllAbnormalTimer(int client, int abnormal)
		{
			if(g_hAbnormalEffectTimer[client][abnormal] != INVALID_HANDLE)
			{
				KillTimer(g_hAbnormalEffectTimer[client][abnormal], true);
				g_hAbnormalEffectTimer[client][abnormal] = INVALID_HANDLE;
			}

			if(g_hAbnormalRemoveTimer[client][abnormal] != INVALID_HANDLE)
			{
				KillTimer(g_hAbnormalRemoveTimer[client][abnormal], true);
				g_hAbnormalRemoveTimer[client][abnormal] = INVALID_HANDLE;
			}
		}

		public Action RemoveAbnormal(Handle timer, DataPack pack)
		{
			pack.Reset();
			int client = pack.ReadCell();
			int abnormal = pack.ReadCell();

			#if defined DEBUG
				PrintToChat(client, "Removing %s (Level : %d)", NAME_OF_ABNORMAL[abnormal], giPlyTriggerAbnormalLevel[client][abnormal]);
			#endif
			PlayerAbnormalTrigger(client, OFF, abnormal, giPlyTriggerAbnormalLevel[client][abnormal], 1.0);
			
			return Plugin_Stop;
		}

		public Action ProcessAbnormalContinuousEffect(Handle timer, DataPack pack)
		{
			int client, abnormal, level;
			pack.Reset();
			client = pack.ReadCell();
			abnormal = pack.ReadCell();
			level = giPlyTriggerAbnormalLevel[client][abnormal];

			#if defined DEBUG
			PrintToChat(client, "Triggering %s Continuous Effect (%d)", NAME_OF_ABNORMAL[abnormal], level);
			#endif

			//TO-DO EQUIP
			//TO-DO WEAPON
			// switch(abnormal)
			// {
			// 	case 1:		//1HP 電波
			// 	{
			// 		if(GetClientHealth(client)<=1)
			// 			PlayerAbnormalTrigger(client, OFF, abnormal, giPlyTriggerAbnormalLevel[client][abnormal], 1.0);
			// 		else
			// 			SetEntityHealth(client, GetClientHealth(client)-1);
			// 	}
			// 	case 2:		//睡眠
			// 	{
			// 		SetEntityHealth(client, Clamp(GetClientHealth(client)+GetRandomInt(2,5), 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])));
			// 	}
			// 	case 3:		//重傷昏迷
			// 	{
			// 		SetEntityHealth(client, Clamp(GetClientHealth(client)+1, 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])));
			// 	}
			// 	case 6:		//毒
			// 	{
			// 		SDKHooks_TakeDamage(client, 0, 0, level*3.0, 0);
			// 		ScreenFade(client, 200, 0, FFADE_IN, {0, 255, 0, 255});
			// 	}
			// 	case 10:	//疫苗効果
			// 	{
			// 		SDKHooks_TakeDamage(client, 0, 0, level*GetRandomFloat(5.0, 8.0), 0);
			// 	}
			// 	case 13:	//大爆炸準備中
			// 	{
			// 		int target;
			// 		float origin[3], target_origin[3];
			// 		char fmt[] = "大爆炸準備中\n距離:%.0f/%.0f";
			// 		GetClientAbsOrigin(client, origin);
			// 		SetHudTextParams(-1.0, 0.65, 1.0, 255, 0, 255, 255, 1, 0.0, 0.0, 0.0);
					
			// 		for(target=1; target<=MaxClients; target++)
			// 		{
			// 			if(client!=target && IsClientValid(target) && IsPlayerAlive(target) && IsPlayerHuman(target))
			// 			{
			// 				GetClientAbsOrigin(target, target_origin);
			// 				ShowHudText(target, HUD_CHN_BOSS_SKILL, fmt, GetVectorDistance(target_origin, origin), BIG_BANG_RANGE);
			// 			}
			// 		}
			// 	}

			// 	case 17:	//CAMP METER
			// 	{
			// 		GetClientAbsOrigin(client, BR_PLAYER_NEW_ORIGIN(client));
			// 		BR_MOVING_DISTANCE(client) = GetVectorDistance(BR_PLAYER_NEW_ORIGIN(client), BR_PLAYER_OLD_ORIGIN(client));

			//newline replaced
			//		CopyVector(BR_PLAYER_OLD_ORIGIN(client), BR_PLAYER_NEW_ORIGIN(client));

			// 		//受傷或行夠都唔係扣
			// 		if(BR_MOVING_DISTANCE(client) < BR_CAMP_DISTNACE)
			// 		{
			// 			if(gfPlyLastDamageTime[client]<GetGameTime()-(ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_CONTINUOUS_EFFECT_RATE]/100.0))
			// 			{
			// 				BR_CAMP_METER(client) -= BR_CAMP_METER_LOST_PER_CHECK * (BR_CAMP_DISTNACE-BR_MOVING_DISTANCE(client)) / BR_CAMP_DISTNACE;
			// 			}
			// 		}
			// 		else
			// 			BR_CAMP_METER(client) += BR_CAMP_METER_INCREASE_PER_100 * (BR_MOVING_DISTANCE(client)-BR_CAMP_DISTNACE)/100.0; 
						

			// 		#if defined DEBUG
			// 			PrintToChat(client, "Moved Distance : %.2f", BR_MOVING_DISTANCE(client));
			// 		#endif

			// 		if(BR_CAMP_METER(client) <= 0)
			// 			ForcePlayerSuicide(client);

			// 	}
			// 	case 25:	//燃焼
			// 	{
			// 		SDKHooks_TakeDamage(client, 0, 0, level*5.0, 0);
			// 		ScreenFade(client, 200, 0, FFADE_IN, {255, 0, 0, 255});

			// 		IgniteEntity(client, ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_CONTINUOUS_EFFECT_RATE]/100.0);
			// 	}
			// 	case 27:SetEntityHealth(client, Clamp(GetClientHealth(client)+(level), 0, RoundFloat(gfPlyCurAbility[client][ABILITY_HP_LIMIT])) );
			// 	case 30:
			// 	{
			// 		if(GAME_BOSS)
			// 		{
			// 			if( IsClientValid(BOSS_ID[0]) && (GetEntityFlags(BOSS_ID[0])&(FL_SWIM|FL_INWATER|FL_WATERJUMP)) && CURRENT_MAP_ITEM_ORIGIN_AMOUNT > 0 )
			// 			{
			// 				TeleportEntity(BOSS_ID[0], CURRENT_MAP_ITEM_ORIGIN[GetRandomInt(0, CURRENT_MAP_ITEM_ORIGIN_AMOUNT-1)], NULL_VECTOR, NULL_VECTOR);
			// 				DisplayPlayerTempHudMessage(BOSS_ID[0], "你被人類隊長傳送了!!!", .color={255, 50, 50, 255});
			// 			}
			// 		}
			// 	}
			// }

			return Plugin_Continue;
		}

	RetrieveAbnormalData(Database db)
	{
		char query[1024];
		DBResultSet rs;
		FormatEx(query, sizeof(query), 	"SELECT id"
										...", name"
										...", max_level"
										...", type_flag"
										...", identity_flag"
										...", gamemode_flag"
										...", continuous_effect_rate"
										...", ability_data"
										..." FROM Server_Abnormal_Data");

		if( (rs=SQL_Query(db, query)) == null )
		{
			LogError("Failed to retrieve Abnormal Data\n%s", query);
			SetTsubakiFailState("TSUBAKI SERVER ERROR\n0x51A0112A55B548CB");
			SetFailState("Failed to Retrieve Abnormal Data");
			return;
		}

		LOADED_ABNORMAL = 0;
		int id;
		char flags[64], ability_flags[256];

		while(rs.FetchRow())
		{
			id = rs.FetchInt(0);
			rs.FetchString(1, NAME_OF_ABNORMAL[id], 32);
			ABNORMAL_DEFAULT_DATA[id][ABNORMAL_DATA_MAX_LEVEL] = rs.FetchInt(2);
			ABNORMAL_DEFAULT_DATA[id][ABNORMAL_DATA_CONTINUOUS_EFFECT_RATE] = RoundFloat(rs.FetchFloat(6)*100.0);

			flags[0] = 0;
			rs.FetchString(3, flags, sizeof(flags));						//type_flag
			FlagsToBitsum(flags, ABNORMAL_DEFAULT_DATA[id][ABNORMAL_DATA_TYPE], 1);

			flags[0] = 0;
			rs.FetchString(4, flags, sizeof(flags));						//identity_flag
			FlagsToBitsum(flags, ABNORMAL_DEFAULT_DATA[id][ABNORMAL_DATA_IDENTITY], 1);

			flags[0] = 0;
			rs.FetchString(5, flags, sizeof(flags));						//gamemode_flag
			FlagsToBitsum(flags, ABNORMAL_DEFAULT_DATA[id][ABNORMAL_DATA_AVAILABLE_GAMEMODE], 1);

			ability_flags[0] = 0;	
			rs.FetchString(7, ability_flags, sizeof(ability_flags));		//ability_data
			ParseAbilityFlagsData(ability_flags, ABNORMAL_ABILITY_LIST[id], ABNORMAL_ABILITY_VALUE[id]);
			
			if(ABNORMAL_DEFAULT_DATA[id][ABNORMAL_DATA_TYPE] & AT_GAMBLE_BUFF) GAMBLE_BUFF[GAMBLE_BUFF_AMOUNT++] = id;
			else if(ABNORMAL_DEFAULT_DATA[id][ABNORMAL_DATA_TYPE] & AT_GAMBLE_DEBUFF) GAMBLE_DEBUFF[GAMBLE_DEBUFF_AMOUNT++] = id;


			LOADED_ABNORMAL++;
		}

		delete rs;
	}

	void DisplayAbnormalDictMenu(client, pos, bool:is_admin=false) {
		static Menu menu = view_as<Menu>(INVALID_HANDLE);
		
		if(menu == INVALID_HANDLE) {
			menu = CreateMenu(AbnormalDictMenuHandler, MenuAction_Select);

			for(new id=1, String:szId[4]; id<LOADED_ABNORMAL; id++) {
				IntToString(id, szId, sizeof(szId));
				menu.AddItem(szId, NAME_OF_ABNORMAL[id]);
			}
		}

		if(is_admin) {
			new String:msg[64];
			FormatEx(msg, sizeof(msg), "異常狀態圖鑑【管理員】　對象:%N", PLY_MENU_TARGET(client));
 			menu.SetTitle(msg);
		}
		else menu.SetTitle("異常狀態圖鑑");
		
		menu.DisplayAt(client, (pos/MAX_MENU_ITEM)*MAX_MENU_ITEM, MENU_TIME_FOREVER);
	}

		public int AbnormalDictMenuHandler(Menu menu, MenuAction action, int client, int abnormal_id)
		{
			if(action == MenuAction_Select)
			{
				char szId[5];
				menu.GetItem(abnormal_id, szId, sizeof(szId));
				
				DisplayAbnormalDetailMenu(client, .abnormal=StringToInt(szId), .is_admin=(PLY_MENU_ID(client)==MENUID_ABNORMAL_DICT_ADMIN));
			}

			return 0;
		}

	void DisplayAbnormalDetailMenu(client, abnormal, bool:is_admin=false) {
		
		static abnormal_type_bit[] = {
			AT_BUFF,
			AT_DEBUFF,
			AT_REMOVABLE,
			AT_ETERNAL,
			AT_EQUIP,
			AT_CONTINUOUS_EFFECT,
			AT_GAME_PAUSE,
			AT_TRIGGER_WHILE_DEAD,
			0
		};

		static String:abnormal_type_name[][] = {
			 "BUFF"
			,"DEBUFF"
			,"可移除"
			,"永久性"
			,"装備"
			,"持続効果"
			,"暫停時可使用"
			,"死亡時可使用"
		};
		
		new String:msg[256];
		new i=0, cnt=0;
		PLY_MENU_ITEM(client) = abnormal;

		if(is_admin) FormatEx(msg, sizeof(msg), "異常狀態詳細　対象:%N", PLY_MENU_TARGET(client));
		else FormatEx(msg, sizeof(msg), "異常狀態詳細");

		Panel panel = GetPanelForDisplayOnly(msg, .cancel_only=false);
		panel.DrawText(EMPTY_LINE);
		msg[0]=0; FormatEx(msg, sizeof(msg), "異常狀態:%s", NAME_OF_ABNORMAL[abnormal]);
		panel.DrawText(msg);
		panel.DrawText(EMPTY_LINE);
		
		msg[0]=0;
		FormatEx(msg, sizeof(msg), "身分:");
		for(i=1; i<TOTAL_IDENTITY; i++) {
			if(ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_IDENTITY]&(1<<i))
				Format(msg, sizeof(msg), "%s%s,", msg, NAME_OF_IDENTITY[i]);
		}
		panel.DrawText(msg);

		msg[0] = 0;
		for(; abnormal_type_bit[i]!=0; i++) {
			if(ABNORMAL_DEFAULT_DATA[abnormal][ABNORMAL_DATA_TYPE]&abnormal_type_bit[i]){
				Format(msg, sizeof(msg), "%s|%s", msg, abnormal_type_name[i]);
				cnt++;
			}
			
			if(cnt%4==0 && cnt!=0) {
				panel.DrawText(msg);
				cnt = msg[0] = 0;
			}
		}

		if(cnt != 0) 
			panel.DrawText(msg);


		msg[0] = 0; FormatEx(msg, sizeof(msg), "能力"); 
		panel.DrawText(msg);
		for(i=0; i<TOTAL_ABILITY && ABNORMAL_ABILITY_LIST[abnormal][i] != -1; i++)
		{
			msg[0] = 0; 
			FormatEx(msg, sizeof(msg), "%s:%.4f(%d)", NAME_OF_ABILITY[ABNORMAL_ABILITY_LIST[abnormal][i]], ABNORMAL_ABILITY_VALUE[abnormal][i], ABNORMAL_ABILITY_LIST[abnormal][i]); 
			panel.DrawText(msg);
		}
		panel.DrawItem("返回目錄");

		if(is_admin)
		{
			panel.DrawItem("即時使用");
		}

		panel.SetKeys(MENU_KEYS_ALL);
		panel.Send(client, AbnormalDetailMenuHandler, MENU_TIME_FOREVER);

		CloseHandle(panel);
	}

		public int AbnormalDetailMenuHandler(Menu menu, MenuAction action, int client, int pos) {
			if(action == MenuAction_Select) {
				switch(pos) {
					case 1:{
						DisplayAbnormalDictMenu(client, .pos=PLY_MENU_PAGE(client)*MAX_MENU_ITEM, .is_admin=(PLY_MENU_ID(client)==MENUID_ABNORMAL_DICT_ADMIN));
					}
					case 2:{
						if(PLY_MENU_ID(client) == MENUID_ABNORMAL_DICT_ADMIN) {
							PlayerAbnormalTrigger(client, ON, PLY_MENU_ITEM(client), 1, 20.0);
						}
					}
				}
			}

			return 1;
		}


	#if defined DEBUG
		public Action DebugDisplayAbnormalInfo(int client, int args)
		{
			PLY_MENU_ID(client) = MENUID_ABNORMAL_DICT_ADMIN;
			
			PLY_MENU_TARGET(client) = GetPlayerAimingTarget(client);
			if(PLY_MENU_TARGET(client)==-1) PLY_MENU_TARGET(client) = client;

			DisplayAbnormalDictMenu(client, 0, .is_admin=true);

			return Plugin_Handled;
		}
	#endif

	bool GivePlayerBPItem(int client, int item_id, int durability=-1, bool give_only=true)
	{
		if(item_id <= 0 || item_id > TOTAL_ITEM || NAME_OF_ITEM[item_id][0] == 0)
			return false;

		if(durability==-1)
			durability = ITEM_DEFAULT_DATA[item_id][ITEM_DATA_DURABILITY];

		int pos = GetPlayerBackpackEmptySpace(client);

		if(pos == -1)
		{
			if(give_only)
			{
				return false;
			}

			if(!PlayerItemTrigger(client, ON, item_id, 1, ITEM_DEFAULT_DATA[item_id][ITEM_DATA_LAST_TIME]/100.0))
				return false;
		}
		else
		{
			giPlyBPItem[client][pos][PLAYER_ITEM_ID] = item_id;
			giPlyBPItem[client][pos][PLAYER_ITEM_DURABILITY] = durability;

			PrintToChat(client, " \x10取得%s (%d)", NAME_OF_ITEM[item_id], durability);
		}

		return true;
	}

	bool GivePlayerWHItem(int client, int item_id, int durability=-1)
	{
		if(item_id <= 0 || item_id > TOTAL_ITEM || NAME_OF_ITEM[item_id][0] == 0)
			return false;

		if(durability==-1)
			durability = ITEM_DEFAULT_DATA[item_id][ITEM_DATA_DURABILITY];

		int pos = GetPlayerWarehouseEmptySpace(client);

		if(pos != -1) {
			giPlyWHItem[client][pos][PLAYER_ITEM_ID] = item_id;
			giPlyWHItem[client][pos][PLAYER_ITEM_DURABILITY] = durability;

			PrintToChat(client, " \x10倉庫回收了%s (%d)", NAME_OF_ITEM[item_id], durability);
		} else {
			PrintToChat(client, " \x07倉庫沒有位置導致無法回收%s (%d)", NAME_OF_ITEM[item_id], durability);
		}

		return true;
	}

	public Action PlayerCallBackpackUseMenu(int client, int args)
	{
		ResetPlayerMenu(client);
		PLY_MENU_ID(client) = MENUID_BACKPACK_USE;
		
		DisplayBackpackPanel(client);
		return Plugin_Handled;
	}

	//TO-DO PLAYER SETTING
		// public Action DisplayBackpackTask(Handle timer, int client)
		// {
		// 	if(IsClientValid(client) && !PS_DISABLE_AUTO_BP(client) && !GAME_END)
		// 	{
		// 		switch(PLY_IDENTITY(client))
		// 		{
		// 			case IDENTITY_HUMAN,IDENTITY_LEADER:	
		// 			{
		// 				if(PLY_MENU_ID(client) == MENUID_EQUIP)
		// 				{
		// 					//PrintToServer("Display Equip Menu");
		// 					PlayerCallSelectEquipSlotPanel(client, 0);
		// 				}
		// 				else
		// 					PlayerCallBackpackUseMenu(client, 0);
		// 			}
		// 			case IDENTITY_ZOMBIE, IDENTITY_BOSS:PlayerCallWeaponBagPanel(client, 0);
		// 		}
		// 	}

		// 	return Plugin_Continue;
		// }

	public Action PlayerCallBackpackThrowMenu(int client, int args)
	{
		ResetPlayerMenu(client);
		PLY_MENU_ID(client) = MENUID_BACKPACK_THROW;
		DisplayBackpackPanel(client);
		return Plugin_Handled;
	}

	public Action PlayerCallBPToWHMenu(int client, int args)
	{
		ResetPlayerMenu(client);
		PLY_MENU_ID(client) = MENUID_BP_TO_WH;
		DisplayBackpackPanel(client);

		return Plugin_Handled;		
	}

	DisplayBackpackPanel(int client)	
	{
		Panel panel = new Panel();

		if(PLY_MENU_ID(client)==MENUID_BACKPACK_USE)
			panel.SetTitle("背包（使用物品）");
		else if(PLY_MENU_ID(client)==MENUID_BACKPACK_THROW)
			panel.SetTitle("背包（扔棄物品）");
		else if(PLY_MENU_ID(client)==MENUID_BP_TO_WH)
			panel.SetTitle("背包（存在倉庫）");
		char menu_item[64];
		int i=0, pos;

		if(PLY_MENU_ID(client) == MENUID_BACKPACK_USE)
		{
			panel.DrawItem("装備へ", IsPlayerHuman(client)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);
		}
		panel.CurrentKey = 2;
		for(; i<5&&(pos=PLY_MENU_PAGE(client)*5+i)<giPlyBPSpace[client] ;i++)
		{
			if(giPlyBPItem[client][pos][PLAYER_ITEM_ID] == 0) {
				FormatEx(menu_item, sizeof(menu_item), EMPTY_ITEM_NAME);
			}
			else
			{
				FormatEx(menu_item, sizeof(menu_item), "%s - %s", NAME_OF_ITEM[giPlyBPItem[client][pos][PLAYER_ITEM_ID]], DESCRIPTION_OF_ITEM[giPlyBPItem[client][pos][PLAYER_ITEM_ID]]);
			}

			// 左右顯示
			// if((pos=(pos+5)%MAX_BP_SPACE)<giPlyBPSpace[client]) {
			// 	if(giPlyBPItem[client][pos][PLAYER_ITEM_ID] == 0) {
			// 		Format(menu_item, sizeof(menu_item), "%s　　%s%d.%s", menu_item, (pos<5)?"優先":"後備", i, EMPTY_ITEM_NAME);
			// 	}
			// 	else
			// 	{
			// 		Format(menu_item, sizeof(menu_item), "%s　　%s%d.%s", menu_item, (pos<5)?"優先":"後備", i, NAME_OF_ITEM[giPlyBPItem[client][pos][PLAYER_ITEM_ID]]);
			// 	}
			// }
			
			panel.DrawItem(menu_item);
			menu_item[0] = 0;
		}

		panel.CurrentKey = 7;
		if(PLY_MENU_PAGE(client)>0)
			panel.DrawItem("上一頁");
		else if((i+1)*PLY_MENU_PAGE(client)<giPlyBPSpace[client])
			panel.DrawItem("下一頁");
		
		if(PLY_MENU_ID(client) == MENUID_BACKPACK_USE)
		{
			panel.DrawText("　\n輔助功能");
			panel.CurrentKey = 8;
			panel.DrawItem("手電筒 (/flashlight)");
			//panel.DrawItem("夜視鏡", (giPlyCurrentEquip[client][GetEquipSlot(12)]==12)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);
			//if(giCurRoundMode == GAMEMODE_BR) panel.DrawItem("睡眠");
		}	

		panel.CurrentKey = 9; panel.DrawItem(CANCEL);
		
		// 上下顯示
		// panel.DrawText(EMPTY_LINE);
		// for(i=5; i<10&&(pos=(PLY_MENU_PAGE(client)*5+i)%10)<giPlyBPSpace[client]; i++) {
		// 	if(giPlyBPItem[client][pos][PLAYER_ITEM_ID] == 0) {
		// 		FormatEx(menu_item, sizeof(menu_item), "%s%d.%s", (pos<5)?"優先":"後備", i%5, EMPTY_ITEM_NAME);
		// 	} else {
		// 		FormatEx(menu_item, sizeof(menu_item), "%s%d.%s - %s", (pos<5)?"優先":"後備", i%5, NAME_OF_ITEM[giPlyBPItem[client][pos][PLAYER_ITEM_ID]], DESCRIPTION_OF_ITEM[giPlyBPItem[client][pos][PLAYER_ITEM_ID]]);
		// 	}

		// 	panel.DrawText(menu_item);
		// 	menu_item[0] = 0;
		// }

		panel.SetKeys(MENU_KEYS_ALL);
		panel.Send(client, BackpackPanelHandler, MENU_TIME_FOREVER);
		delete panel;
	}
	
		public int BackpackPanelHandler(Menu menu, MenuAction action, int client, int pos)
		{
			if(action == MenuAction_Select)
			{
				EMIT_MENU_SOUND(client);
				switch(PLY_MENU_ID(client))
				{
					case MENUID_BACKPACK_USE:
					{
						switch(pos)
						{
							//TO-DO EQUIP
							//case 1:PlayerCallSelectEquipSlotPanel(client, 0);
							case 7:
							{
								PLY_MENU_PAGE(client) = PLY_MENU_PAGE(client)==0?1:0;
								DisplayBackpackPanel(client);
							}
							case 8:
							{
								SetEntProp(client, Prop_Send, m_fEffects, GetEntProp(client, Prop_Send, m_fEffects) ^ 4);
								DisplayBackpackPanel(client);
							}
							case 9:return 1;
							default:
							{
								pos = PLY_MENU_PAGE(client)*5+pos-2;

								int item_id;

								if((item_id=giPlyBPItem[client][pos][PLAYER_ITEM_ID]) != 0)
								{
									#if defined DEBUG
										PrintToChat(client, "Trying to trigger item %s", NAME_OF_ITEM[item_id]);
									#endif

									if(PlayerItemTrigger(client, ON, item_id, 1, ITEM_DEFAULT_DATA[item_id][ITEM_DATA_LAST_TIME]/100.0))
									{
										if(--giPlyBPItem[client][pos][PLAYER_ITEM_DURABILITY] <= 0)
										{
											//TO-DO Check Player Setting
											if(pos<=5) {
												for(int i=0; i<TOTAL_PLAYER_ITEM_INFO; i++) {
													giPlyBPItem[client][pos][i] = giPlyBPItem[client][pos+5][i];
												}
												ClearBPSpace(client, pos+5);
											} else {
												ClearBPSpace(client, pos);
											}
										}

										switch(item_id)
										{
											case 32:GivePlayerBPItem(client, 33);	//開心樂園
										}
									}
									
									DisplayBackpackPanel(client);
								}
							}
						}
					}
					case MENUID_BACKPACK_THROW:
					{
						pos = PLY_MENU_PAGE(client)*5+pos-2;
						if(giPlyBPItem[client][pos][PLAYER_ITEM_ID] != 0)
						{
							float origin[3];
							GetClientAbsOrigin(client, origin);
							origin[2] += 2.0;
							CreateItemEntity(giPlyBPItem[client][pos][PLAYER_ITEM_ID], giPlyBPItem[client][pos][PLAYER_ITEM_DURABILITY], origin, client);
							ClearBPSpace(client, pos);
							DisplayBackpackPanel(client);
						}
					}
					case MENUID_BP_TRADE:			//
					{
						if(GAME_START)
						{
							PrintToChat(client, " \x07遊戲開始中不能進行交易");
							return 1;
						} 

						if(giPlyBPItem[client][pos][PLAYER_ITEM_ID] != 0)
						{
							int target = GetPlayerAimingTarget(client);
							if(target == -1)
							{
								PrintToChat(client, " \x07請指向一個對象");
							}
							else
							{
								int bp_space = GetPlayerBackpackEmptySpace(target), info=0;
								if(bp_space == -1) PrintToChat(client, " \x07對方背包沒有位置了");
								else
								{
									for(; info<TOTAL_PLAYER_ITEM_INFO; info++)
									{
										giPlyBPItem[target][bp_space][info] = giPlyBPItem[client][pos][info];
										giPlyBPItem[client][pos][info] = 0;
									}
									PrintToChat(client, " \x04你給予了%N一個%s", target, NAME_OF_ITEM[giPlyBPItem[target][bp_space][PLAYER_ITEM_ID]]);
									PrintToChat(target, " \x04%N給予了你一個%s", client, NAME_OF_ITEM[giPlyBPItem[target][bp_space][PLAYER_ITEM_ID]]);
								}
							}
							DisplayBackpackPanel(client);
						}
					}
					case MENUID_BP_TO_WH:		//Using menu
					{
						if(giPlyBPItem[client][pos][PLAYER_ITEM_ID] != 0)
						{
							int wh_space = GetPlayerWarehouseEmptySpace(client), info=0;
							if(wh_space == -1) 
								PrintToChat(client, " \x07倉庫沒有位置了");
							else
							{
								for(; info<TOTAL_PLAYER_ITEM_INFO; info++)
								{
									giPlyWHItem[client][wh_space][info] = giPlyBPItem[client][pos][info]; 
									giPlyBPItem[client][pos][info] = 0;
								}
								
								PrintToChat(client, " \x04已將%s存入倉庫", NAME_OF_ITEM[giPlyWHItem[client][wh_space][PLAYER_ITEM_ID]]);
								DisplayBackpackPanel(client);
							}
						}
					}
				}
			}
			else if(action == MenuAction_Cancel)
			{
				PLY_MENU_ID(client) = 0;
			}
			else if(action == MenuAction_End)
			{
				delete menu;
			}

			return 0;
		}


	DisplayWarehousePanel(int client)
	{
		if(PLY_MENU_PAGE(client)<0) return;

		int i=0, wh_space, item_id;
		char menu_item[64], fmt[64];
		Panel panel = new Panel();

		if(PLY_MENU_ID(client) == MENUID_WH_SELL)
		{
			panel.SetTitle("倉庫（出售物品）");
			strcopy(fmt, sizeof(fmt), "%s($%d)");
		} 
		else if(PLY_MENU_ID(client) == MENUID_WH_TO_BP) 
		{
			panel.SetTitle("倉庫（提取物品）");
		}
		else
		{
			delete panel;
			return;
		}

		for(; i<MAX_MENU_ITEM&&(wh_space=PLY_MENU_PAGE(client)*MAX_MENU_ITEM+i)<giPlyWHSpace[client]; i++)
		{
			item_id = giPlyWHItem[client][wh_space][PLAYER_ITEM_ID];

			if(PLY_MENU_ID(client) == MENUID_WH_SELL) FormatEx(menu_item, sizeof(menu_item), fmt, (item_id==0)?EMPTY_ITEM_NAME:NAME_OF_ITEM[item_id], ITEM_DEFAULT_DATA[item_id][ITEM_DATA_SELL_PRICE]);
			else FormatEx(menu_item, sizeof(menu_item), (item_id==0)?EMPTY_ITEM_NAME:NAME_OF_ITEM[item_id]);
			
			panel.DrawItem(menu_item);
		}

		panel.CurrentKey = 7; if(PLY_MENU_PAGE(client)>0) panel.DrawItem("上一頁", ITEMDRAW_DEFAULT);
		panel.CurrentKey = 8; if((PLY_MENU_PAGE(client)*MAX_MENU_ITEM+i)<giPlyWHSpace[client]) panel.DrawItem("下一頁", ITEMDRAW_DEFAULT);
		panel.CurrentKey = 9; panel.DrawItem(CANCEL, ITEMDRAW_DEFAULT);

		panel.SetKeys(MENU_KEYS_ALL);
		panel.Send(client, WarehousePanelHandler, MENU_TIME_FOREVER);

		delete panel;
	}

		public int WarehousePanelHandler(Menu menu, MenuAction action, int client, int pos)
			{
				if(action == MenuAction_Select)
				{
					EMIT_MENU_SOUND(client);
					switch(pos) {
						case 7: {
							if(PanelHasPreviousPage(client)) {
								PLY_MENU_PAGE(client)--;
								DisplayWarehousePanel(client);
							}
						}
						case 8:
						{
							if(PanelHasNextPage(client, giPlyWHSpace[client])) {
								PLY_MENU_PAGE(client)++;
								DisplayWarehousePanel(client);
							}
						}
						case 9: return 1;
						default: {
							pos = pos-1 + PLY_MENU_PAGE(client) * MAX_MENU_ITEM;

							switch(PLY_MENU_ID(client)) {
								case MENUID_WH_TO_BP:			//Using Panel
								{
									if(giPlyWHItem[client][pos][PLAYER_ITEM_ID] != 0 && pos<giPlyWHSpace[client])
									{
										int bp_space = GetPlayerBackpackEmptySpace(client), info=0;
										if(bp_space == -1) 
											PrintToChat(client, " \x07背包沒有位置了");
										else
										{
											for(; info<TOTAL_PLAYER_ITEM_INFO; info++)
											{
												giPlyBPItem[client][bp_space][info] = giPlyWHItem[client][pos][info]; 
												giPlyWHItem[client][pos][info] = 0;
											}
											
											PrintToChat(client, " \x04從倉庫提取%s", NAME_OF_ITEM[giPlyBPItem[client][bp_space][PLAYER_ITEM_ID]]);
											DisplayWarehousePanel(client);
										}
									}
								}
								case MENUID_WH_SELL:
								{
									int item_id;
									if((item_id=giPlyWHItem[client][pos][PLAYER_ITEM_ID]) != 0 && pos<giPlyWHSpace[client])
									{
										PrintToChat(client, " \x07出售%s 獲得$%d ($%d->$%d)", NAME_OF_ITEM[item_id], ITEM_DEFAULT_DATA[item_id][ITEM_DATA_SELL_PRICE], giPlyWHCash[client], giPlyWHCash[client]+ITEM_DEFAULT_DATA[item_id][ITEM_DATA_SELL_PRICE]);
										giPlyWHCash[client] += ITEM_DEFAULT_DATA[item_id][ITEM_DATA_SELL_PRICE];
										ClearWHSpace(client, pos);
										DisplayWarehousePanel(client);
									}
								}
							}
						}
					}
				}
				else if(action == MenuAction_Cancel)
				{
					PLY_MENU_ID(client) = 0;
				}
				else if(action == MenuAction_End)
				{
					delete menu;
				}

				return 0;
			}

	// DisplayBackpackMenu(int client, int pos=0)
	// {
	// 	Menu menu = new Menu(BackpackMenuHandler, MenuAction_Select);
	// 	switch(PLY_MENU_ID(client))
	// 	{
	// 		case MENUID_BACKPACK_USE:menu.SetTitle("背包（使用物品）");
	// 		case MENUID_BACKPACK_THROW:menu.SetTitle("背包（扔棄物品）");
	// 		case MENUID_BP_TO_WH:menu.SetTitle("背包（存入倉庫）");
	// 		case MENUID_BP_TRADE:menu.SetTitle("背包（交易物品）");
	// 	}

	// 	char menu_item[64];
	// 	if(PLY_MENU_ID(client) == MENUID_BACKPACK_USE)
	// 	{
	// 		menu.AddItem(EMPTY, "装備へ", IsPlayerHuman(client)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);
	// 	}

	// 	for(int i=0; i<giPlyBPSpace[client]; i++)
	// 	{
	// 		if(giPlyBPItem[client][i][PLAYER_ITEM_ID] == 0)
	// 			menu.AddItem(EMPTY, EMPTY_ITEM_NAME);
	// 		else
	// 		{
	// 			FormatEx(menu_item, sizeof(menu_item), "%s - %s", NAME_OF_ITEM[giPlyBPItem[client][i][PLAYER_ITEM_ID]], DESCRIPTION_OF_ITEM[giPlyBPItem[client][i][PLAYER_ITEM_ID]]);
	// 			menu.AddItem(EMPTY, menu_item);

	// 			menu_item[0] = 0;
	// 		}
	// 	}

	// 	menu.DisplayAt(client, (pos/MAX_MENU_ITEM)*MAX_MENU_ITEM, MENU_TIME_FOREVER);
	// }

		

	stock SortWarehouse(int& client)
	{
		int pos, pos2, sub, sort_id, item_id, warehouse_sort[MAX_WH_SPACE][TOTAL_PLAYER_ITEM_INFO];

		for(pos=pos2=0; pos<giPlyWHSpace[client]; pos++)
		{
			if(giPlyWHItem[client][pos][PLAYER_ITEM_ID]==0) continue;

			for(sub=0; sub<TOTAL_PLAYER_ITEM_INFO; sub++)
			{
				warehouse_sort[pos2][sub] = giPlyWHItem[client][pos][sub];
				giPlyWHItem[client][pos][sub] = 0;
			}
			pos2++;
		}

		for(pos=pos2=sort_id=0; sort_id<TOTAL_ITEM && (item_id=ITEM_DISPLAY_SORT[sort_id])!=0; sort_id++)
		{
			for(pos2=0; pos2<giPlyWHSpace[client]; pos2++)
			{
				if(warehouse_sort[pos2][PLAYER_ITEM_ID]==item_id)
				{
					for(sub=0; sub<TOTAL_PLAYER_ITEM_INFO; sub++)
					{
						giPlyWHItem[client][pos][sub] = warehouse_sort[pos2][sub];
						warehouse_sort[pos2][sub] = 0;
					}
					pos++;
				}
			}
		}

		PrintToChat(client, " \x07你的倉庫已經整理好");
	}

/*  Level and SP  */

	public Action PlayerCallSPMenu(int client, int args)
	{
		static char title[] = "技能點數 剩餘:%d";
		static char fmt[] = "%s:%d/%d", menu_item[64];
		static int i, ability;

		Panel panel = new Panel();
		FormatEx(menu_item, sizeof(menu_item), title, giPlyAvailableSP[client]);
		panel.SetTitle(menu_item);
		menu_item[0] = 0;

		for(i=0; i<MAX_MENU_ITEM && (ability=PLY_MENU_PAGE(client)*MAX_MENU_ITEM+i+1)<TOTAL_ABILITY; i++) {
			FormatEx(menu_item, sizeof(menu_item), fmt, NAME_OF_ABILITY[ability], giPlySP[client][ability], MAX_SP[ability]);
			panel.DrawItem(menu_item);
			menu_item[0] = 0;
		}

		AddPanelPageController(panel, client, TOTAL_ABILITY-1);
		
		panel.SetKeys(MENU_KEYS_ALL);
		panel.Send(client, SPMenuHandler, MENU_TIME_FOREVER);
		delete panel;

		return Plugin_Handled;
	}

		public int SPMenuHandler(Menu menu, MenuAction action, int client, int pos)
		{
			if(action == MenuAction_Select)
			{
				switch(pos) {
					case 7: {
						if(PanelHasPreviousPage(client)) {
							PLY_MENU_PAGE(client)--;
							PlayerCallSPMenu(client, 0);
						}
					}
					case 8: {
						if(PanelHasNextPage(client, TOTAL_ABILITY-1)) {
							PLY_MENU_PAGE(client)++;
							PlayerCallSPMenu(client, 0);
						}
					}
					case 9:return 1;
					default: {
						pos = PLY_MENU_PAGE(client)*MAX_MENU_ITEM + pos;
						if(giPlyAvailableSP[client]>0 && giPlySP[client][pos]<MAX_SP[pos])
						{
							giPlyAvailableSP[client]--;
							giPlySP[client][pos]++;

							if(GAME_START && g_hCalculateFinalAbilityTask[client] == INVALID_HANDLE)
								g_hCalculateFinalAbilityTask[client] = CreateTimer(5.0, TaskCalculatFinalAbility, client, TIMER_FLAG_NO_MAPCHANGE);

							PlayerCallSPMenu(client, (pos/6)*6);
							
						}

					}
				}
				pos+=1;
			}

			return 1;
		}

	stock LevelUp(int client, int level)
	{
		//TO-DO Equip
		//int before_level = giPlyLevel[client];
		for(int i=0; i<level; i++)
		{
			giPlyLevel[client]++;
			giPlyAvailableSP[client]++;
		}

		//TO-DO Equip
		// if(before_level != giPlyLevel[client])
		// 	CheckLevelUnlockEquipment(client, before_level);

		if(level > 0)
			PrintToChat(client, " \x04等級上昇至%d", giPlyLevel[client]);
	}

	// TO-DO Player Info
	// ResetLevelAndSP(int client)
	// {
	// 	if(giPlyID[client] == 0) 
	// 		return;

	// 	int level;
	// 	char query[512];
	// 	DBResultSet rs;
	// 	//Map Score Calculate
	// 	FormatEx(query, sizeof(query), "SELECT score FROM Server_Map_Score_Rank WHERE player_id=%d;", giPlyID[client]);

	// 	if( (rs=SQL_Query(g_hDatabase, query)) != null)
	// 	{
	// 		while(rs.FetchRow())
	// 		{
	// 			level += RoundToFloor( rs.FetchFloat(0) / MAP_SCORE_PER_LEVEL );
	// 		}

	// 		delete rs;
	// 	}

	// 	level += giPlyTotalPlaytime[client] / PLAYTIME_PER_LEVEL;

	// 	giPlyLevel[client] = 1;
	// 	giPlyAvailableSP[client] = DEFAULT_SP;
	// 	for(int i=0; i<TOTAL_ABILITY; i++)
	// 		giPlySP[client][i] = 0;
	// 	LevelUp(client, level);

	// 	SavePlayerSP(g_hDatabase, client, .clear_sp=false);
	// 	SavePlayerData(g_hDatabase, client, .reset_data=false);
	// }


/*  Equipment  */

	bool PlayerEquip(int client, int equip_id, bool call_calculate_ability=true, bool call_final_ability=true) {
		#if defined DEBUG
			PrintToChat(client, "You try to Equip %s", NAME_OF_EQUIP[equip_id]);
		#endif

		if(equip_id<=0 || equip_id>LOADED_EQUIP || !g_CvarAllowEquip.IntValue)
			return false;

		int slot=GetEquipSlot(equip_id);

		if(giPlyCurrentEquip[client][slot] != 0) {
			EmptyEquipSlot(client, slot, .calculate_equip_ability=false);
		}

		giPlyCurrentEquip[client][slot] = equip_id;
		
		//Equip Skill
		if(EQUIP_DEFAULT_DATA[equip_id][EQUIP_DATA_SKILL_ID] != 0) {
			AddPlayerSkill(client, EQUIP_DEFAULT_DATA[equip_id][EQUIP_DATA_SKILL_ID]);
		}

		//Equip Ability
		if(EQUIP_ABILITY_LIST[equip_id][0]>=1 && call_calculate_ability)
			CalculateEquipAbility(client, .call_final_ability=call_final_ability);

		PrintToChat(client, "已装備%s", NAME_OF_EQUIP[equip_id]);
		return true;
	}

	EmptyEquipSlot(int client, int slot=-1, bool calculate_equip_ability=true, bool call_final_ability=true) {
		if(calculate_equip_ability) {
			if(slot==-1) {		//empty_all
				for(slot=0; slot<TOTAL_EQUIP_TYPE_ALL_IDENTITY; slot++) {
					if(EQUIP_DEFAULT_DATA[giPlyCurrentEquip[client][slot]][EQUIP_DATA_SKILL_ID] != 0) {
						RemovePlayerSkill(client, EQUIP_DEFAULT_DATA[giPlyCurrentEquip[client][slot]][EQUIP_DATA_SKILL_ID]);
					}
					giPlyCurrentEquip[client][slot] = 0;
				}
			} else {
				if(EQUIP_DEFAULT_DATA[giPlyCurrentEquip[client][slot]][EQUIP_DATA_SKILL_ID] != 0) {
					RemovePlayerSkill(client, EQUIP_DEFAULT_DATA[giPlyCurrentEquip[client][slot]][EQUIP_DATA_SKILL_ID]);
				}
				giPlyCurrentEquip[client][slot] = 0;
			}
		}

		if(calculate_equip_ability) {
			CalculateEquipAbility(client, .call_final_ability=call_final_ability);
		}
	}

	CalculateEquipAbility(int client, bool call_final_ability=true)
	{
		int i=0, j=0, equip, ability=0;
		for(; ability<TOTAL_ABILITY; ability++) gfPlyEquipAbility[client][ability] = 0.0;
		for(; i<TOTAL_EQUIP_TYPE_ALL_IDENTITY; i++, j=0)
		{
			if((equip=giPlyCurrentEquip[client][i]) != 0)
			{
				while(j<TOTAL_ABILITY && (ability=EQUIP_ABILITY_LIST[equip][j]) != -1)
				{
					gfPlyEquipAbility[client][ability] += EQUIP_ABILITY_VALUE[equip][j];
					j++;
				}
			}
		}

		if(GAME_START && call_final_ability) {
			CalculateFinalAbility(client);
		}
	}

	public Action PlayerCallEquipSlotPanel(int client, int args) {
		
		ResetPlayerMenu(client);
		DisplayEquipSlotPanel(client);

		return Plugin_Handled;
	}

		void DisplayEquipSlotPanel(&client) {

			if(!g_CvarAllowEquip.IntValue) {
				PrintToChat(client, " \x07現在不能使用裝備");
				return;
			}

			PLY_MENU_ID(client) = MENUID_EQUIP;

			static String:EQUIP_ITEM_SLOT_FORMAT[] = "装備%c:%s";
			
			new String:menu_item[64];
			new slot=0;

			Panel panel = new Panel();
			panel.SetTitle("装備(按位置選擇)");

			for(; slot<TOTAL_EQUIP_TYPE; slot++) {
				if(giPlyCurrentEquip[client][slot]==0) {
					FormatEx(menu_item, sizeof(menu_item), EQUIP_ITEM_SLOT_FORMAT, NAME_OF_EQUIP_SLOT[slot], EMPTY_ITEM_NAME);
				}
				else {
					FormatEx(menu_item, sizeof(menu_item), EQUIP_ITEM_SLOT_FORMAT, NAME_OF_EQUIP_SLOT[slot], NAME_OF_EQUIP[giPlyCurrentEquip[client][slot]]);
				}

				panel.DrawItem(menu_item);
				menu_item[0] = 0;
			}

			for(; slot<TOTAL_EQUIP_TYPE_ALL_IDENTITY; slot++) {
				if(giPlyCurrentEquip[client][slot]!=0) {
					FormatEx(menu_item, sizeof(menu_item), EQUIP_ITEM_SLOT_FORMAT, NAME_OF_EQUIP_SLOT[slot], NAME_OF_EQUIP[giPlyCurrentEquip[client][slot]]);
					panel.DrawItem(menu_item);
					menu_item[0] = 0;
				}
			}

			DrawCancelToPanelWithNewLine(panel);
			panel.SetKeys(MENU_KEYS_ALL);
			panel.Send(client, EquipSlotPanelHandler, MENU_TIME_FOREVER);
			delete panel;
		}	

			public int EquipSlotPanelHandler(Menu menu, MenuAction action ,int client, int key) { 
				
				if(action == MenuAction_Select && GAME_PREPARE && 0<key<=TOTAL_EQUIP_TYPE) {
					EMIT_MENU_SOUND(client);
					ResetPlayerMenu(client);
					PLY_MENU_ID(client) = MENUID_EQUIP_CHOOSE;
					DisplayEquipChoosePanel(client, key-1);
				}

				return 1;
			}

		void DisplayEquipChoosePanel(client, slot) {
			static char EQUIP_ITEM_DISPLAY_FORMAT[] = "%s(%s)";
			static char EQUIP_ITEM_DISPLAY_CDR_FMT[] = "%s(冷卻中 [%d局])";

			if(!g_CvarAllowEquip.IntValue) {
				PrintToChat(client, " \x07現在不能使用裝備");
				return;
			}

			static String:menu_item[64], equip_id, i, bool:can_equip;
			Panel panel = new Panel();
			panel.SetTitle("装備選択");

			for(i=0; i<MAX_MENU_ITEM&&(equip_id=EQUIP_DISPLAY_LIST[slot][GetPanelItemPos(client,i)])!=0; i++) {
				can_equip = false;

				if( IsPlayerEquipUnlocked(client, equip_id) ){
					if( IsEquipInCooldown(client, equip_id) )
						FormatEx(menu_item, sizeof(menu_item), EQUIP_ITEM_DISPLAY_CDR_FMT, NAME_OF_EQUIP[equip_id], GetEquipCooldownRound(client, equip_id));
					else if(giPlyCurrentEquip[client][slot]==equip_id)
						FormatEx(menu_item, sizeof(menu_item), EQUIP_ITEM_DISPLAY_FORMAT, NAME_OF_EQUIP[equip_id], CURRENTLY_USING);
					else{
						FormatEx(menu_item, sizeof(menu_item), EQUIP_ITEM_DISPLAY_FORMAT, NAME_OF_EQUIP[equip_id], USEABLE);
						can_equip = true;
					}
				} else {
					FormatEx(menu_item, sizeof(menu_item), EQUIP_ITEM_DISPLAY_FORMAT, DISPLAY_EQUIP(equip_id)?NAME_OF_EQUIP[equip_id]:UNKNOWN_ITEM_NAME, LOCKED);
				}
				
				if(can_equip) {
					panel.DrawItem(menu_item);
				} else {
					Format(menu_item, sizeof(menu_item), "%d. %s", i+1, menu_item);
					panel.DrawText(menu_item);
				}
				menu_item[0] = 0;
			}

			panel.DrawText(EMPTY_LINE);

			panel.CurrentKey = 7;
			panel.DrawItem((PLY_MENU_PAGE(client)<=0)?PREVIOUS_MENU:PREVIOUS_PAGE);

			panel.CurrentKey = 8;
			if(PanelHasNextPage(client, LOADED_EQUIP_EACH_SLOT[slot]))
				panel.DrawItem(NEXT_PAGE);
			DrawCancelToPanelWithNewLine(panel);

			PLY_MENU_ITEM(client) = slot;		//Which Slot Client is watching
			panel.SetKeys(MENU_KEYS_ALL);
			panel.Send(client, EquipChooseMenuHandler, MENU_TIME_FOREVER);
			delete panel;
		}

			public int EquipChooseMenuHandler(Menu menu, MenuAction action, int client, int pos) {

				if(action == MenuAction_Select) {
					EMIT_MENU_SOUND(client);
					switch(pos) {
						case 7:{
							if(PanelHasPreviousPage(client)){
								PLY_MENU_PAGE(client)--;
								DisplayEquipChoosePanel(client, PLY_MENU_ITEM(client));
							} else {
								DisplayEquipSlotPanel(client);
							}
						}
						case 8:{
							if(PanelHasNextPage(client, LOADED_EQUIP_EACH_SLOT[PLY_MENU_ITEM(client)])){
								PLY_MENU_PAGE(client)++;
								DisplayEquipChoosePanel(client, PLY_MENU_ITEM(client));
							}
						}
						case 9:return 1;
						default: {
							if(GetPanelItemPos(client,pos-1)<LOADED_EQUIP_EACH_SLOT[PLY_MENU_ITEM(client)]) {
								DisplayPlayerConfirmEquipPanel(client, EQUIP_DISPLAY_LIST[PLY_MENU_ITEM(client)][GetPanelItemPos(client,pos-1)]);
							}
						}
					}
				}

				return 1;
			}

		void DisplayPlayerConfirmEquipPanel(client, equip_id, bool:ignore_all_condition=false) {
			if(GAME_START && !ignore_all_condition) {
				return;
			}
			
			static String:EQUIP_ITEM_DISPLAY_FORMAT[] = "装備確認　%s(%s)";
			static String:EQUIP_ITEM_DISPLAY_CDR_FMT[] = "装備確認　%s(冷卻中 [%d局])";

			PLY_MENU_TARGET(client) = equip_id;	//Equip Id

			new String:menu_item[512], slot=GetEquipSlot(equip_id), bool:can_equip=false;
			Panel panel = new Panel();

			if(EQUIP_DEFAULT_DATA[equip_id][EQUIP_DATA_IS_ENABLE] == EQUIP_DATA_DISABLE) {
				FormatEx(menu_item, sizeof(menu_item), "装備確認　%s\n【此裝備未開放使用】", (DISPLAY_EQUIP(equip_id))?NAME_OF_EQUIP[equip_id]:UNKNOWN_ITEM_NAME);
			} else if(!IsPlayerEquipUnlocked(client, equip_id)) {
				FormatEx(menu_item, sizeof(menu_item), "装備確認　%s【未解鎖】", (DISPLAY_EQUIP(equip_id))?NAME_OF_EQUIP[equip_id]:UNKNOWN_ITEM_NAME);
			} else if(GetEquipCooldownRound(client, equip_id)>0) {
				FormatEx(menu_item, sizeof(menu_item), EQUIP_ITEM_DISPLAY_CDR_FMT, NAME_OF_EQUIP[equip_id], GetEquipCooldownRound(client, equip_id));
			} else {
				FormatEx(menu_item, sizeof(menu_item), EQUIP_ITEM_DISPLAY_FORMAT, NAME_OF_EQUIP[equip_id], USEABLE);
				can_equip = true;
			}
			panel.SetTitle(menu_item);

			menu_item[0] = 0;
			panel.DrawText(EMPTY_LINE);
			FormatEx(menu_item, sizeof(menu_item), "裝備位置:#%c\n"
												..."現在裝備:%s\n"
												..."解鎖條件:%s\n"
												..."稀有度:%d\n"
												..."　\n"
												..."每次使用後局數CD:%d局\n"
												..."技能:%s\n"
												, NAME_OF_EQUIP_SLOT[slot]
												, (giPlyCurrentEquip[client][slot]==0)?EMPTY_ITEM_NAME:NAME_OF_EQUIP[giPlyCurrentEquip[client][slot]]
												, EQUIP_UNLOCK_CONDITION[equip_id]
												, EQUIP_DEFAULT_DATA[equip_id][EQUIP_DATA_RARE]
												, EQUIP_DEFAULT_DATA[equip_id][EQUIP_DATA_ROUND_COOLDOWN]+1
												, (EQUIP_DEFAULT_DATA[equip_id][EQUIP_DATA_SKILL_ID]==0)?EMPTY_ITEM_NAME:NAME_OF_SKILL[EQUIP_DEFAULT_DATA[equip_id][EQUIP_DATA_SKILL_ID]]
												);

			panel.DrawText(menu_item); menu_item[0] = 0;

			if((EQUIP_DEFAULT_DATA[equip_id][EQUIP_DATA_AVAILABLE_GAMEMODE]&(1<<giCurRoundMode))==0) {
				FormatEx(menu_item, sizeof(menu_item), "此模式不能裝備此道具");
			}

			panel.DrawText(EMPTY_LINE);
			if( giPlyCurrentEquip[client][slot]==equip_id ) {
				panel.DrawItem("解除");
			} else if(!can_equip) {
				panel.DrawText("現在不能裝備");
			} else {
				panel.DrawItem("裝備");
			}
			

			if(PLY_MENU_ID(client) == MENUID_EQUIP_CHOOSE) {
				panel.CurrentKey = 3;
				panel.DrawItem(PREVIOUS_MENU);
			}

			DrawCancelToPanelWithNewLine(panel);
			
			panel.SetKeys(MENU_KEYS_ALL);
			panel.Send(client, PlayerConfirmEquipPanelHandler, MENU_TIME_FOREVER);

			delete panel;

			return;
		}

			public int PlayerConfirmEquipPanelHandler(Menu menu, MenuAction action, int client, int key) {
				if(action == MenuAction_Select) {
					switch(key) {
						case 1:{		//Confirm
							EMIT_MENU_SOUND(client);
							PlayerEquip(client, .equip_id=PLY_MENU_TARGET(client));

							if(PLY_MENU_ID(client) == MENUID_EQUIP_CHOOSE) {
								ResetPlayerMenu(client);
								DisplayEquipSlotPanel(client);
							}
						}		
						case 3:{		//To Slot Item Menu
							EMIT_MENU_SOUND(client);
							if(PLY_MENU_ID(client) == MENUID_EQUIP_CHOOSE) {
								DisplayEquipChoosePanel(client, PLY_MENU_ITEM(client));
							}
						}
					}
				}

				return 1;
			}

	RetrieveEquipData(Database db) {
		char query[512];
		DBResultSet rs;
		FormatEx(query, sizeof(query), "SELECT"
									..."   id"
									..." , type"
									..." , rare"
									..." , name"
									..." , description"
									..." , unlock_condition"
									..." , gamemode_flag"
									..." , ability"
									..." , round_cooldown"
									..." , enable"
									..." , forced_auto_message"
									..." , skill_id"
									..." FROM Server_Equip_Data"
									..." WHERE id<%d "
									..." ORDER BY display_id;", 
									TOTAL_EQUIP);

		if( (rs=SQL_Query(db, query)) == null )
		{
			SetTsubakiFailState("TSUBAKI SERVER ERROR\n0xD8003CBB41115D74");
			SetFailState("Failed to retrieve Equip Data");
			return;
		}
		
		LOADED_EQUIP = 0;

		int id, slot;
		char flags[16], ability_flags[128];

		for(id=0; id<TOTAL_EQUIP_TYPE_ALL_IDENTITY; id++) {
			LOADED_EQUIP_EACH_SLOT[id] = 0;
		}

		while(rs.FetchRow())
		{
			id = rs.FetchInt(0);

			EQUIP_DEFAULT_DATA[id][EQUIP_DATA_TYPE] = rs.FetchInt(1);
			slot = EQUIP_DEFAULT_DATA[id][EQUIP_DATA_TYPE] - 1;

			if(slot < 0 || slot >= TOTAL_EQUIP_TYPE) {
				slot = TOTAL_EQUIP_TYPE_ALL_IDENTITY-1;
			}
			EQUIP_DISPLAY_LIST[slot][LOADED_EQUIP_EACH_SLOT[slot]++] = id;

			EQUIP_DEFAULT_DATA[id][EQUIP_DATA_RARE] = rs.FetchInt(2);
			NAME_OF_EQUIP[id][0]=0; 			rs.FetchString(3, NAME_OF_EQUIP[id], 32);
			EQUIP_DESCRIPTION[id][0]=0; 		rs.FetchString(4, EQUIP_DESCRIPTION[id], 128);
			EQUIP_UNLOCK_CONDITION[id][0]=0;	rs.FetchString(5, EQUIP_UNLOCK_CONDITION[id], 64);

			flags[0] = 0;
			rs.FetchString(6, flags, sizeof(flags));
			FlagsToBitsum(flags, EQUIP_DEFAULT_DATA[id][EQUIP_DATA_AVAILABLE_GAMEMODE], 1);
			
			ability_flags[0] = 0;
			rs.FetchString(7, ability_flags, sizeof(ability_flags));
			ParseAbilityFlagsData(ability_flags, EQUIP_ABILITY_LIST[id], EQUIP_ABILITY_VALUE[id]);

			#if defined DEBUG
				EQUIP_DEFAULT_DATA[id][EQUIP_DATA_ROUND_COOLDOWN] = -1;
			#else
				EQUIP_DEFAULT_DATA[id][EQUIP_DATA_ROUND_COOLDOWN] = rs.FetchInt(8);
			#endif
			EQUIP_DEFAULT_DATA[id][EQUIP_DATA_IS_ENABLE] = rs.FetchInt(9);
			EQUIP_DEFAULT_DATA[id][EQUIP_DATA_AUTO_TRIGGER_MSG] = rs.FetchInt(10);
			EQUIP_DEFAULT_DATA[id][EQUIP_DATA_SKILL_ID] = rs.FetchInt(11);

			LOADED_EQUIP++;
		}

		for(slot=0; slot<TOTAL_EQUIP_TYPE_ALL_IDENTITY; slot++)
			if(LOADED_EQUIP_EACH_SLOT[slot] < (TOTAL_EQUIP/2)-1)
				EQUIP_DISPLAY_LIST[slot][LOADED_EQUIP_EACH_SLOT[slot]] = 0;

		delete rs;
	}

	stock CheckLevelUnlockEquipment(int client, int start_level=1, bool dontBroadcast=false)
	{
		for(; start_level<=giPlyLevel[client]; start_level++)
		{
			switch(start_level)
			{
				case 1:
				{
					UnlockPlayerEquip(client, 1, dontBroadcast);
					UnlockPlayerEquip(client, 11, dontBroadcast);
					UnlockPlayerEquip(client, 17, dontBroadcast);
					UnlockPlayerEquip(client, 20, dontBroadcast);
					UnlockPlayerEquip(client, 23, dontBroadcast);
					UnlockPlayerEquip(client, 25, dontBroadcast);
				}
				case 3:{UnlockPlayerEquip(client, 10, dontBroadcast); UnlockPlayerEquip(client, 12, dontBroadcast);}
				case 5:{UnlockPlayerEquip(client, 3, dontBroadcast);}
				case 10:{UnlockPlayerEquip(client, 2, dontBroadcast); UnlockPlayerEquip(client, 6, dontBroadcast);}
				case 15:{UnlockPlayerEquip(client, 8, dontBroadcast); UnlockPlayerEquip(client, 16, dontBroadcast); UnlockPlayerEquip(client, 18, dontBroadcast);}
				case 20:{UnlockPlayerEquip(client, 27, dontBroadcast);}
				case 25:{UnlockPlayerEquip(client, 4, dontBroadcast); UnlockPlayerEquip(client, 9, dontBroadcast);}
				case 30:{UnlockPlayerEquip(client, 7, dontBroadcast); UnlockPlayerEquip(client, 29, dontBroadcast);}
			}
		}
	}

	stock bool DisplayEquipAutoTriggerMessage(int client, int equip_id, int header=0)
	{
		static char message[128];

		switch(equip_id)
		{
			case 2, 11, 13, 17, 18, 19:{}
			default:return false;
		}

		message[0] = 0;
		if(EQUIP_DEFAULT_DATA[equip_id][EQUIP_DATA_AUTO_TRIGGER_MSG]==0) header = GetRandomInt(1, 2);
		switch(header)
		{
			case 1:FormatEx(message, sizeof(message), " \x04感受到主人的危機,%s自動起動", NAME_OF_EQUIP[equip_id]);
			case 2:FormatEx(message, sizeof(message), " \x04認為主人會蠢到忘記用自己,%s自動觸發", NAME_OF_EQUIP[equip_id]);
		}

		PrintToChat(client, message);
		return true;
	}

	stock bool PlayerGamble(int client, int cost, bool buff, int force_result=-1, bool great_gamble=false, bool wh_cash=false)
	{
		if(wh_cash)
		{
			int cash = giPlyWHCash[client];
			if( cash < cost )
			{
				PrintToChat(client, " \x07你花光你的儲蓄了");
				return false;
			}

			giPlyWHCash[client] -= cost;
			PrintToChat(client, "儲蓄$%d -> $%d", cash, giPlyWHCash[client]);
		}
		else
		{
			if( giPlyBPCash[client] < cost )
			{
				PrintToChat(client, " \x07你太窮了");
				return false;
			}

			giPlyBPCash[client] -= cost;
			SetPlayerMoney(client, giPlyBPCash[client]);
		}


		if(buff)
		{
			if(force_result==-1)
				force_result = GetRandomInt(0, 5);

			switch(force_result)
			{
				case 0, 1, 2:		//Random Buff
				{
					PlayerAbnormalTrigger(client, ON, GAMBLE_BUFF[GetRandomInt(0, GAMBLE_BUFF_AMOUNT-1)], 1, 20.1);
					PrintToChat(client, " \x04你突然感覺到身體湧出一股力量");
				}
				case 3, 4:		//$$x2
				{
					GivePlayerCash(client, cost*(great_gamble?3:2));
					PrintToChat(client, " \x04上天決定將你倒入海既錢錢加倍給你");
				}
				case 5:
				{
					if(great_gamble)
						PlayerItemTrigger(client, ON, GREAT_GAMBLE_ITEM[GetRandomInt(0, GREAT_GAMBLE_ITEM_AMOUNT-1)], 1, 10.1);
					else
						PlayerItemTrigger(client, ON, GAMBLE_ITEM[GetRandomInt(0, GAMBLE_ITEM_AMOUNT-1)], 1, 10.1);
				}
			}

		}
		else
		{
			if(force_result==-1)
				force_result = GetRandomInt(0, 5);
			
			switch(force_result)
			{
				case 0, 1, 2:
				{
					PlayerAbnormalTrigger(client, ON, GAMBLE_DEBUFF[GetRandomInt(0, GAMBLE_DEBUFF_AMOUNT-1)], 1, 20.1);
					PrintToChat(client, " \x04你感覺到你被人下了咒");
				}
				case 3:
				{
					float velocity[3];
					GetEntityVelocity(client, velocity);
					velocity[2] = 9999.0;
					TeleportEntity(client, NULL_VECTOR, NULL_VECTOR, velocity);

					PrintToChat(client, " \x04恭喜你獲得一次笨豬跳的機會");
				}
				case 4, 5:		//Give $$
				{
					int target=1, amount=0, target_list[MAXPLAYERS];

					for(; target<=MaxClients; target++)
					{
						if(target!=client && IsClientValid(target) && IsPlayerAlive(target) && IsPlayerHuman(target) && !IsFakeClient(target))
							target_list[amount++] = target;
					}

					if(amount==0)
					{
						PrintToChat(client, " \x07你決定將$%d倒入海", cost);
					}
					else		//Give $$
					{
						amount=cost/amount;
						PrintToChatAll(" \x04%N決定派$%d比人類", client, amount);

						target=0;
						while(target_list[target]!=0)
							GivePlayerCash(target_list[target++], amount);

							
						if(!IsPlayerEquipUnlocked(client, 22))
							UnlockPlayerEquip(client, 22, .dontBroadcast=false);
					}
					
				}

			}
		}
		return true;
	}


/*  Weapon  */
	
	stock StartPlayerWeaponTimer(const int& client)
	{
		if(IsClientValid(client))
		{
			if(g_hPlyWeaponCheck[client] == INVALID_HANDLE)
				g_hPlyWeaponCheck[client] = CreateTimer(DEFAULT_WEAPON_LEVEL_CHECK_TIME, CheckPlayerWeaponTask, client, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);

			if(g_hPlyWeaponSupply[client] == INVALID_HANDLE)
			{
				float next_supply_time;
				next_supply_time = DEFAULT_SUPPLY_TIME[giPlyWeaponLevel[client]]*(1.0-gfPlyCurAbility[client][ABILITY_SUPPLY_TIME]) - DEFAULT_SUPPLY_TIME[giPlyWeaponLevel[client]]*0.5*(CURRENT_MAP_DIFFICULTY-1)/9.0;
				gfPlyNextSupplyTime[client] = GetGameTime() + next_supply_time + 1.0;
				g_hPlyWeaponSupply[client] = CreateTimer(next_supply_time, PlayerSupplyTask, client, TIMER_FLAG_NO_MAPCHANGE);
				
			}
				
			#if defined DEBUG
				PrintToChat(client, "Start up Weapon Timer");
			#endif
		}
	}

	stock RemovePlayerWeaponTimer(const int& client)
	{
		if(IsClientValid(client))
		{
			if(g_hPlyWeaponCheck[client] != INVALID_HANDLE)
			{
				KillTimer(g_hPlyWeaponCheck[client]);
				g_hPlyWeaponCheck[client] = INVALID_HANDLE;
			}

			if(g_hPlyWeaponSupply[client] != INVALID_HANDLE)
			{
				KillTimer(g_hPlyWeaponSupply[client]);
				gfPlyNextSupplyTime[client] = 0.0;
				g_hPlyWeaponSupply[client] = INVALID_HANDLE;
			}

			#if defined DEBUG
				PrintToChat(client, "Stop Weapon Timer");
			#endif
		}
	}

	public Action PlayerCallWeaponPanel(int client, int args) {

		ResetPlayerMenu(client);

		PLY_MENU_ID(client) = MENUID_WEAPON_PANEL;
		DisplayPlayerWeaponPanel(client);

		return Plugin_Handled;
	}

	void DisplayPlayerWeaponPanel(&client) {
		static i;
		Panel panel = new Panel();
		panel.SetTitle("武器設定");
		panel.DrawText(EMPTY_LINE);

		if(!CanPlayerCustomizeWeapon()) {
			panel.DrawText("現在不能設定");

			switch(g_CvarCanPlayerCustomizeWeapon.IntValue) {
				case C_WEAPON_BLOCKED_DUE_TO_GAMEMODE: {
					panel.DrawText("原因:現在模式不能自訂武器");
				}
				case C_WEAPON_BLOCKED_DUE_TO_DIFFICULT: {
					panel.DrawText("原因:現在難易度不能自訂武器");
				}
				case C_WEAPON_GAME_START: {

					panel.DrawText("原因:遊戲已經開始");

					for(i=0; i<MAX_WEAPON_LEVEL; i++) {
						panel.DrawItem((giPlyChosenWeaponList[client][i]==0)?EMPTY_ITEM_NAME:WEAPON_LIST[giPlyChosenWeaponList[client][i]], ITEMDRAW_DISABLED);
					}
				}
			}

			DrawCancelToPanelWithNewLine(panel);
			panel.SetKeys(MENU_KEYS_ALL);
			panel.Send(client, DummyMenuHandler, MENU_TIME_FOREVER);
		}
		else {
			for(i=0; i<MAX_WEAPON_LEVEL; i++) {
				panel.DrawItem((giPlyChosenWeaponList[client][i]==0)?EMPTY_ITEM_NAME:WEAPON_LIST[giPlyChosenWeaponList[client][i]], ITEMDRAW_DEFAULT);
			}
			
			DrawCancelToPanelWithNewLine(panel);
			panel.SetKeys(MENU_KEYS_ALL);
			panel.Send(client, WeaponPanelHandler, MENU_TIME_FOREVER);
		}

		delete panel;
	}

		public int WeaponPanelHandler(Menu menu, MenuAction action, int client, int key) {
			if(action == MenuAction_Select && g_CvarCanPlayerCustomizeWeapon.IntValue == C_WEAPON_ALLOW) {
				EMIT_MENU_SOUND(client);
				key -= 1;
				if(0 <= key < MAX_WEAPON_LEVEL) {
					DisplayWeaponChoicePanel(client, key);
				}
			}
			return 1;
		}

	void DisplayWeaponChoicePanel(int& client, int level) {
		static i, String:menu_item[64];
		Panel panel = new Panel();

		FormatEx(menu_item, sizeof(menu_item), "武器設定 等級%d", level+1);
		panel.SetTitle(menu_item); menu_item[0] = i = 0;

		PLY_MENU_ITEM(client) = level;

		while(i < WEAPON_AMOUNT_PER_LEVEL[level]) {
			panel.DrawItem(WEAPON_LIST[AVAILIABLE_WEAPON_ID[level][i]]);
			i++;
		}

		DrawCancelToPanelWithNewLine(panel);
		panel.SetKeys(MENU_KEYS_ALL);
		panel.Send(client, WeaponChoicePanelHandler, MENU_TIME_FOREVER);
		delete panel;
	}

		public int WeaponChoicePanelHandler(Menu menu, MenuAction action, int client, int key) {
			
			if(action==MenuAction_Select && CanPlayerCustomizeWeapon()) {
				key -= 1;
				if(0 <= key < WEAPON_AMOUNT_PER_LEVEL[PLY_MENU_ITEM(client)]) {
					EMIT_MENU_SOUND(client);
					giPlyChosenWeaponList[client][PLY_MENU_ITEM(client)] = key;
					
					if(PLY_MENU_ID(client) == MENUID_WEAPON_PANEL) {
						DisplayPlayerWeaponPanel(client);
					}
				}
			}

			return 1;
		}

		public Action CheckPlayerWeaponTask(Handle timer, int client)
			{
				if(GAME_START && IsClientConnected(client) && IsPlayerAlive(client) && IsPlayerHuman(client))
				{
					while( giPlyWeaponLevel[client]<MAX_WEAPON_LEVEL && gfPlyWeaponExp[client] >= WEAPON_EXP[giPlyWeaponLevel[client]] )
						++giPlyWeaponLevel[client];

					if( gbsPlyReceivedWeapon[client] < ALL_ONE_BS[giPlyWeaponLevel[client]] ) {
						int process_level=giPlyWeaponLevel[client], weapon_id, ent_id, slot;
						char weapon[32];
						bool processed[TOTAL_PROCESS_WEAPON_SLOT];

						for(; process_level>=0; process_level--) {
							if( ~gbsPlyReceivedWeapon[client] & (1<<process_level) ) {
								
								if(giPlyCurrentWeaponList[client][process_level] == WEAPON_ID_DONT_GIVE){
									gbsPlyReceivedWeapon[client] |= (1<<process_level);	
									continue;
								}
								
								if(giPlyChosenWeaponList[client][process_level] == 0)
									weapon_id = AVAILIABLE_WEAPON_ID[process_level][GetRandomInt(0, WEAPON_AMOUNT_PER_LEVEL[process_level])];
								else 
									weapon_id = giPlyChosenWeaponList[client][process_level];

								slot = WEAPON_LIST_INFO[weapon_id][WEAPON_INFO_SLOT];

								if(!processed[slot]) {
									if( (ent_id=GetPlayerWeaponSlot(client, slot)) != -1 )
										RemoveEntity(ent_id);

									FormatEx(weapon, sizeof(weapon), "weapon_%s", WEAPON_LIST[weapon_id]);
									DataPack pack = new DataPack();
									pack.WriteCell(client);
									pack.WriteString(weapon);
									pack.WriteCell((giCurRoundMode==GAMEMODE_VB || GAME_PROGRESS_COMPLETE)?WEAPON_LIST_INFO[weapon_id][WEAPON_INFO_AMMO]:WEAPON_LIST_INFO[weapon_id][WEAPON_INFO_SUPPLY_AMMO]);
									pack.WriteCell((giCurRoundMode==GAMEMODE_VB || GAME_PROGRESS_COMPLETE)?WEAPON_LIST_INFO[weapon_id][WEAPON_INFO_RESERVED]:WEAPON_LIST_INFO[weapon_id][WEAPON_INFO_RESERVED_AMMO]);
									CreateTimer(0.1, DelayGiveWeapon, pack, TIMER_DATA_HNDL_CLOSE|TIMER_FLAG_NO_MAPCHANGE);
									
									if(weapon_id != -1) 
										PrintToChat(client, "獲得 %s", WEAPON_LIST[weapon_id]);
									processed[slot] = true;
									weapon[0] = 0;
								}
								
								gbsPlyReceivedWeapon[client] |= (1<<process_level);	
							}
						}
					}
					return Plugin_Continue;
				}
				
				g_hPlyWeaponCheck[client] = INVALID_HANDLE;
				return Plugin_Stop;
			} 

				public Action DelayGiveWeapon(Handle timer, DataPack pack)
				{
					int client, ammo, ent, reserve;
					char weapon[32];
					pack.Reset();
					client = pack.ReadCell();

					if(IsClientValid(client) && IsPlayerAlive(client) && IsPlayerHuman(client))
					{
						pack.ReadString(weapon, sizeof(weapon));
						ammo = pack.ReadCell();
						reserve = pack.ReadCell();

						ent = GivePlayerItem(client, weapon);
						if(IsValidEntity(ent))
						{
							SetEntProp(ent, Prop_Send, M_CLIP1, ammo);
							SetEntProp(ent, Prop_Send, M_RESERVE_AMMO, reserve);
						}
					}

					return Plugin_Stop;
				}

		public Action PlayerSupplyTask(Handle timer, int client)
		{
			if(!IsClientValid(client) || !IsPlayerAlive(client) || !IsPlayerHuman(client))
			{
				gfPlyNextSupplyTime[client] = 0.0;
				g_hPlyWeaponSupply[client] = INVALID_HANDLE;
				return Plugin_Stop;
			}

			static slot, weapon_id, ent_id, String:weapon_name[32], Float:next_supply_time;

			//primary slot and secondary slot only?
			for(slot=0; slot<2; slot++) {
				if(WILL_SLOT_BE_SUPPLIED[slot] && (ent_id=GetPlayerWeaponSlot(client, 0))!=-1 ) {
					GetEntityClassname(ent_id, weapon_name, sizeof(weapon_name));

					weapon_id = GetW_IdByWeaponNameWithHeader(weapon_name[7]);		//7 because skipping 'weapon_'
					SetEntProp(weapon_id, Prop_Send, M_CLIP1, WEAPON_LIST_INFO[weapon_id][WEAPON_INFO_AMMO]);
					SetEntProp(weapon_id, Prop_Send, M_RESERVE_AMMO, WEAPON_LIST_INFO[weapon_id][WEAPON_INFO_RESERVED]);
				}
			}

			next_supply_time = DEFAULT_SUPPLY_TIME[giPlyWeaponLevel[client]] - DEFAULT_SUPPLY_TIME[giPlyWeaponLevel[client]]*0.5*(CURRENT_MAP_DIFFICULTY-1)/9.0;
			gfPlyNextSupplyTime[client] = GetGameTime() + next_supply_time + 1.0;
			g_hPlyWeaponSupply[client] = CreateTimer(next_supply_time, PlayerSupplyTask, client, TIMER_FLAG_NO_MAPCHANGE);
			

			return Plugin_Stop;
		}

	public void OnEntityCreated(int ent, const char[] clsname) {
		switch(clsname[0])
		{
			case 'w':
			{
				if(StrContains(clsname, WEAPON_HEADER) >= 0)
				{
					SDKHookEx(ent, SDKHook_ReloadPost, OnPlayerReloadPost);
				}
			}
			// case 'i':
			// {
			// 	if(StrEqual(clsname, INFERNO))
			// 		SDKHook(ent, SDKHook_SpawnPost, InfernoSpawn);
			// }
			// case 'h':
			// {
			// 	if(StrEqual(clsname, HEGRENADE_PROJECTILE))
			// 		SDKHook(ent, SDKHook_SpawnPost, HEGrenadeProjectileSpawnPost);
			// }
			// case 's':
			// {
			// 	if(StrEqual(clsname, SMOKEGRENADE_PROJECTILE))
			// 		SDKHook(ent, SDKHook_SpawnPost, SmokeProjectileSpawnPost);
			// }
			// case 'f':
			// {
			// 	if(StrEqual(clsname, FLASHBANG_PROJECTILE))
			// 		SDKHook(ent, SDKHook_SpawnPost, FlashbangProjectileSpawnPost);
			// 	else if(StrEqual(clsname, FUNC_DOOR))
			// 		SDKHook(ent, SDKHook_UsePost, OnDoorUse);
			// }
			// case 'd':
			// {
			// 	if(StrEqual(clsname, DECOY_PROJECTILE))
			// 		SDKHook(ent, SDKHook_SpawnPost, DecoyProjectileSpawnPost);
			// }
			// case 'm':
			// {
			// 	if(StrEqual(clsname, MOLOTOV_PROJECTILE))
			// 		SDKHook(ent, SDKHook_SpawnPost, MolotovProjectileSpawnPost);
			// }
			// case 't':
			// {
			// 	if(StrEqual(clsname, TAGRENADE_PROJECTILE))
			// 		SDKHook(ent, SDKHook_SpawnPost, TagGrenadeProjectileSpawnPost);
			// }
		}
	}
	
	public void OnPlayerReloadPost(int weapon, bool success)
		{
			if(success)
			{
				static client, i;
				client = GetEntityOwner(weapon);

				for(i=0; i<2; i++) {
					if(!IS_SLOT_INFINITY_AMMO[i])
						continue;

					if(GetPlayerWeaponSlot(client, i) == weapon) {
						SetEntProp(weapon, Prop_Send, M_RESERVE_AMMO, 1000);
						break;
					}
				}
				
				gbCanPlayerGetItem[GetEntPropEnt(weapon, Prop_Data, m_hOwnerEntity)] = true;
			}
		}

/*  Player Skills  */

	stock void AddPlayerSkill(int client, int skill_id, skill_amount=-1) {
		new pos=0;
		if(skill_amount==-1) {
			skill_amount = DEFAULT_SKILL_DATA[skill_id][SKILL_DATA_DEFAULT_AMOUNT];
		}

		if(IsSkillActive(skill_id)) {
			//Check for empty position
			for(pos=0; pos<MAX_ACTIVE_SKILL_SLOT; pos++) {
				if(giPlyActiveSkill[client][pos][PLY_SKILL_INFO_ID]==0) {
					giPlyActiveSkill[client][pos][PLY_SKILL_INFO_ID] = skill_id;
					giPlyActiveSkill[client][pos][PLY_SKILL_INFO_REMAIN] = skill_amount;
					break;
				}
			}
		} else if(IsSkillPassive(skill_id)) {
			//Check for empty position
			for(pos=0; pos<MAX_ACTIVE_SKILL_SLOT; pos++) {
				if(giPlyPassiveSkill[client][pos][PLY_SKILL_INFO_ID]==0) {
					giPlyPassiveSkill[client][pos][PLY_SKILL_INFO_ID] = skill_id;
					giPlyPassiveSkill[client][pos][PLY_SKILL_INFO_REMAIN] = skill_amount;
					break;
				}
			}
		}
	}

	stock void RemovePlayerSkill(int client, int skill_id) {
		new pos=0;

		if(IsSkillActive(skill_id)) {
			//Check for empty position
			for(pos=0; pos<MAX_ACTIVE_SKILL_SLOT; pos++) {
				if(giPlyActiveSkill[client][pos][PLY_SKILL_INFO_ID]==skill_id) {
					giPlyActiveSkill[client][pos][PLY_SKILL_INFO_ID] = 0;
					giPlyActiveSkill[client][pos][PLY_SKILL_INFO_REMAIN] = 0;
					break;
				}
			}
		} else if(IsSkillPassive(skill_id)){
			//Check for empty position
			for(pos=0; pos<MAX_ACTIVE_SKILL_SLOT; pos++) {
				//TO-DO Remove Passive Skill Effect
				if(giPlyPassiveSkill[client][pos][PLY_SKILL_INFO_ID]==skill_id) {
					giPlyPassiveSkill[client][pos][PLY_SKILL_INFO_ID] = 0;
					giPlyPassiveSkill[client][pos][PLY_SKILL_INFO_REMAIN] = 0;
					break;
				}
			}
		}
		
	}

	//如影響能力值，請直接創造一個異常
	stock bool TriggerPlayerSkill(client, skill_id, bool:onoff) {
		static Float:origin[3], Float:angles[3];
		if(!CanPlayerTriggerSkill(client, skill_id) )
			return false;

		PrintToChat(client, " \x04使用技能 %s", NAME_OF_SKILL[skill_id]);
		
		switch(skill_id) {
			//TO-DO Remove Debug Part
			case 1: {
				GetClientCenterOrigin(client, origin);
				GetClientAimAngles(client, angles);
				FireNonPenetrateBullet(DEFAULT_BULLET_MODEL, .origin=origin, .angles=angles, .owner=client);
			}
			case 2: {
				GetPlayerAimOrigin(client, origin); origin[2] += 50.0;
				AverageBullet(DEFAULT_BULLET_MODEL, origin, .owner=client);
			}
			case 3: {

			}
		}

		if( IsSkillHasDuration(skill_id) ) {

			DataPack pack = new DataPack();
			pack.WriteCell(client);
			pack.WriteCell(skill_id);

			if(g_hSkillEffectTimer[client][skill_id] == INVALID_HANDLE) {
				InsertArrayElementNoRepeat(giPlyTriggeredSkill[client], TOTAL_SKILL, skill_id);
				gfPlySkillTime[client][skill_id] = GetGameTime() + DEFAULT_SKILL_DATA[skill_id][SKILL_DATA_DEFAULT_DURATION]/100.0;
				g_hSkillEffectTimer[client][skill_id] = CreateTimer(DEFAULT_SKILL_DATA[skill_id][SKILL_DATA_DEFAULT_DURATION]/100.0, TriggerSkillDurationEffect, pack, TIMER_DATA_HNDL_CLOSE|TIMER_FLAG_NO_MAPCHANGE);
			} else {
				KillTimer(g_hSkillEffectTimer[client][skill_id]);
				gfPlySkillTime[client][skill_id] + DEFAULT_SKILL_DATA[skill_id][SKILL_DATA_DEFAULT_DURATION]/100.0;
				g_hSkillEffectTimer[client][skill_id] = CreateTimer(gfPlySkillTime[client][skill_id]-GetGameTime(), TriggerSkillDurationEffect, pack, TIMER_DATA_HNDL_CLOSE|TIMER_FLAG_NO_MAPCHANGE);
			}
			 
		}

		return true;
	}

		bool CanPlayerTriggerSkill(&client, &skill_id) {

			if(!GAME_START && (~DEFAULT_SKILL_DATA[skill_id][SKILL_DATA_TYPE]&ST_GAME_PAUSE)) 
				return false;

			switch(skill_id) {
				case 19: {		//召喚喪屍
				}
			}

			return true;
		}

		public Action TriggerSkillDurationEffect(Handle timer, DataPack pack) {
			static int client, skill_id;
			pack.Reset();
			client = pack.ReadCell();
			skill_id = pack.ReadCell();
			
			RemoveSkillDurationEffect(client, skill_id);

			return Plugin_Stop;
		}

		RemoveSkillDurationEffect(&client, &skill_id, bool:kill_timer=false) {
			RemoveArrayElement(giPlyTriggeredSkill[client], TOTAL_SKILL, skill_id);

			if(g_hSkillEffectTimer[client][skill_id] != INVALID_HANDLE && kill_timer) {
				KillTimer(g_hSkillEffectTimer[client][skill_id]);
			}
			g_hSkillEffectTimer[client][skill_id] = INVALID_HANDLE;
		}

	public Action PlayerCallActiveSkillPanel(client, args) {
		DisplayActiveSkillPanel(client);
		return Plugin_Handled;
	}

	void DisplayActiveSkillPanel(client) {
		Panel panel = new Panel();
		static String:menu_item[64], i, skill, menu_item_style, Float:gametime;
		static char SKILL_DISPLAY_FMT_IN_CD[] =  "%s (冷卻中 [%.0f秒])";
		static char SKILL_DISPLAY_FMT_USABLE[] = "%s (可使用)";
		static char SKILL_DISPLAY_FMT_NO_REMAIN[] = "%s (已用完)";
		static char PASSIVE_SKILL_DISPLAY_FMT[] = "被動%d.%s";
		gametime = GetGameTime();

		panel.SetTitle("技能");
		for(i=0; i<MAX_ACTIVE_SKILL_SLOT; i++) {
			menu_item_style=ITEMDRAW_DEFAULT;

			if( (skill=giPlyActiveSkill[client][i][PLY_SKILL_INFO_ID]) != 0 ) {
				if( SKILL_REMAIN_INFINITY < giPlyActiveSkill[client][i][PLY_SKILL_INFO_REMAIN] <= 0) {
					FormatEx(menu_item, sizeof(menu_item), SKILL_DISPLAY_FMT_NO_REMAIN, NAME_OF_SKILL[skill]);
					menu_item_style = ITEMDRAW_DISABLED;
				}
				else if(gfPlyActiveSkillCooldown[client][i] > gametime) {
					FormatEx(menu_item, sizeof(menu_item), SKILL_DISPLAY_FMT_IN_CD, NAME_OF_SKILL[skill], gfPlyActiveSkillCooldown[client][i]-gametime);
					menu_item_style = ITEMDRAW_DISABLED;
				}
				else 
					FormatEx(menu_item, sizeof(menu_item), SKILL_DISPLAY_FMT_USABLE, NAME_OF_SKILL[skill]);
			} else {
				FormatEx(menu_item, sizeof(menu_item), EMPTY_ITEM_NAME);
				menu_item_style = ITEMDRAW_DISABLED;
			}
			panel.DrawItem(menu_item, menu_item_style);
			menu_item[0] = 0;
		}

		panel.CurrentKey = 9;
		panel.DrawItem(CANCEL);
		panel.DrawText(EMPTY_LINE);
		for(i=0, menu_item_style=1; i<MAX_PASSIVE_SKILL_SLOT; i++) {
			if( (skill=giPlyPassiveSkill[client][i][PLY_SKILL_INFO_ID]) != 0 ) {
				FormatEx(menu_item, sizeof(menu_item), PASSIVE_SKILL_DISPLAY_FMT, menu_item_style, NAME_OF_SKILL[skill]);
				panel.DrawText(menu_item);
				menu_item[0] = 0;
				menu_item_style++;		//counter
			}
		}

		panel.SetKeys(MENU_KEYS_ALL);
		panel.Send(client, ActiveSkillPanelHandler, MENU_TIME_FOREVER);
	}

		ActiveSkillPanelHandler(Menu meun, MenuAction action, int client, int key) {
			if( action==MenuAction_Select ) {
				EMIT_MENU_SOUND(client);
				key -= 1;
				if(0<=key<MAX_ACTIVE_SKILL_SLOT && giPlyActiveSkill[client][key][PLY_SKILL_INFO_ID]!=0) {
					if(gfPlyActiveSkillCooldown[client][key] < GetGameTime() && TriggerPlayerSkill(client, giPlyActiveSkill[client][key][PLY_SKILL_INFO_ID], ON)) {
						gfPlyActiveSkillCooldown[client][key] = GetGameTime() + DEFAULT_SKILL_DATA[giPlyActiveSkill[client][key][PLY_SKILL_INFO_ID]][SKILL_DATA_DEFAULT_CD]/10.0;
					} else {
						PrintToChat(client, IN_COOLDOWN);
					}
					DisplayActiveSkillPanel(client);
				}
			}
			return 1;
		}

	stock void RetrieveSkillData(Database db) {
		new String:query[512];
		DBResultSet rs;
		FormatEx(query, sizeof(query), "SELECT"
									..."  id"
									..." ,name"
									..." ,description"
									..." ,type_flag"
									..." ,default_cooldown"
									..." ,default_amount"
									..." ,default_duration"
									..." FROM Player_Skills"
									);

		if( (rs=SQL_Query(db, query)) == null){
			SetTsubakiFailState("TSUBAKI SERVER ERROR\n0xD2ADA66BEE0E7698");
			SetFailState("Failed to retrieve Skill Data");
			return;
		}

		new id, String:type_flag[32];

		while(rs.FetchRow()) {
			id = rs.FetchInt(0);

			rs.FetchString(1, NAME_OF_SKILL[id], 32);
			rs.FetchString(2, DESCRIPTION_OF_SKILL[id], 64);
			rs.FetchString(3, type_flag, sizeof(type_flag));
			DEFAULT_SKILL_DATA[id][SKILL_DATA_DEFAULT_CD] = RoundFloat(rs.FetchFloat(4)*10.0);
			DEFAULT_SKILL_DATA[id][SKILL_DATA_DEFAULT_AMOUNT] = rs.FetchInt(5);
			DEFAULT_SKILL_DATA[id][SKILL_DATA_DEFAULT_DURATION] = RoundFloat(rs.FetchFloat(6)*100.0);
		}

		delete rs;
	}

/*  SQL Process  */

	stock Database SQLInit(String:confname[], String:database_path[]) {
		Database db;
		char buffer[256];

		bool database_found = FileExists(database_path);

		if(!database_found || !SQL_CheckConfig(confname)) {
			SetTsubakiFailState("TSUBAKI SERVER ERROR\n0x92C93091E8BDBAE2");
			SetFailState("Database Config %s NOT FOUND!!!", confname);
		}
		else {
			db = SQL_Connect(confname, true, buffer, sizeof(buffer));
		}

		return db;
	}

#pragma semicolon 0
