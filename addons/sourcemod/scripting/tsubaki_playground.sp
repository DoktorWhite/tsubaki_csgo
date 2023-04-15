#define DEBUG

#include <tsubaki_v3/tsubaki_common>
#include <tsubaki_v3/tsubaki_hud>
#include <tsubaki_v3/tsubaki_camera>
#include <tsubaki_v3/tsubaki_music_player>
#include <tsubaki_v3/tsubaki_game_core>

#if defined DEBUG
    char TSUBAKI_DATABASE[] = "tsubaki-csgo-debug";
#else
    char TSUBAKI_DATABASE[] = "tsubaki-csgo";
#endif
stock Database g_hDatabase;

public Plugin myinfo = 
{
    name = "Playground",
    author = "WhiteCola",
    description = "Kano-Bi CSGO Version",
    version = "0.0",
    url = ""
}


public void OnPluginStart() {
    giCurRoundMode = GAMEMODE_HVZ;

    new String:buffer[256];

    //Player Commands
    RegConsoleCmd("ab", DebugDisplayAbility);
    RegConsoleCmd("an", DebugDisplayAbnormalInfo);
    RegConsoleCmd("item", DebugDisplayItemInfoMenu);
    RegConsoleCmd("reload_plugin", ReloadServer);
    RegConsoleCmd("bp", PlayerCallBackpackUseMenu);
    RegConsoleCmd("sp", PlayerCallSPMenu);
    RegConsoleCmd("active", PlayerCallActiveSkillPanel);
    RegConsoleCmd("camera", PlayerCallCameraPanel);
    RegConsoleCmd("equip", PlayerCallEquipSlotPanel);
    RegConsoleCmd("weapon", PlayerCallWeaponPanel);
    
    
    //For DEBUG Only
    RegConsoleCmd("wh", PlayerCallWarehouseMenu);
    RegConsoleCmd("explosion", CreateEnvExplosion);
    RegConsoleCmd("lu", ForcePlayerLevelUp);
    RegConsoleCmd("fl", PlayerSwitchFlashlight);
    RegConsoleCmd("nv", PlayerSwitchNightvision);
    RegConsoleCmd("play_music", DebugPlayMusic);
    RegConsoleCmd("stop_music", DebugStopMusic);
    RegConsoleCmd("get_weapon_name", DebugGetWeaponName);
    RegConsoleCmd("debug_weapon", DebugWeaponMenu);
    RegConsoleCmd("dmg_area", DebugDamageAreaPanel);
    RegConsoleCmd("trap", DebugTrapPanel);

    FormatEx(NAME_OF_SKILL[2], 32, "主動技能 2"); DEFAULT_SKILL_DATA[2][SKILL_DATA_DEFAULT_CD] = 100;
    FormatEx(NAME_OF_SKILL[3], 32, "主動技能 3"); DEFAULT_SKILL_DATA[3][SKILL_DATA_DEFAULT_CD] = 100;
    FormatEx(NAME_OF_SKILL[4], 32, "主動技能 4"); DEFAULT_SKILL_DATA[4][SKILL_DATA_DEFAULT_CD] = 100;
    
    FormatEx(NAME_OF_SKILL[5], 32, "被動技能 A"); DEFAULT_SKILL_DATA[5][SKILL_DATA_DEFAULT_CD] = 100;
    FormatEx(NAME_OF_SKILL[6], 32, "被動技能 B"); DEFAULT_SKILL_DATA[6][SKILL_DATA_DEFAULT_CD] = 100;

    // FormatEx(NAME_OF_EQUIP[1], 32, "裝備 A1"); EQUIP_DISPLAY_LIST[0][0] = 1; FormatEx(EQUIP_UNLOCK_CONDITION[1], 128, "CONDITION A1"); 
    // FormatEx(NAME_OF_EQUIP[2], 32, "裝備 A2"); EQUIP_DISPLAY_LIST[0][1] = 2; 
    // FormatEx(NAME_OF_EQUIP[3], 32, "裝備 A3"); EQUIP_DISPLAY_LIST[0][2] = 3; 
    // FormatEx(NAME_OF_EQUIP[4], 32, "裝備 A4"); EQUIP_DISPLAY_LIST[0][3] = 4; 
    // FormatEx(EQUIP_UNLOCK_CONDITION[4], 128, "CONDITION A4"); EQUIP_DEFAULT_DATA[4][EQUIP_DATA_IS_ENABLE]=-1; EQUIP_DEFAULT_DATA[4][EQUIP_DATA_SKILL_ID]=2; 
    // LOADED_EQUIP_EACH_SLOT[0]=4;
    // FormatEx(NAME_OF_EQUIP[5], 32, "裝備 B1"); LOADED_EQUIP_EACH_SLOT[1]=1; EQUIP_DISPLAY_LIST[1][0] = 5; LOADED_EQUIP_EACH_SLOT[1] = 1;
    // FormatEx(NAME_OF_EQUIP[6], 32, "裝備 C1"); LOADED_EQUIP_EACH_SLOT[2]=1; EQUIP_DISPLAY_LIST[2][0] = 6; LOADED_EQUIP_EACH_SLOT[2] = 1;
    // FormatEx(NAME_OF_EQUIP[7], 32, "裝備 D1"); LOADED_EQUIP_EACH_SLOT[3]=1; EQUIP_DISPLAY_LIST[3][0] = 7; LOADED_EQUIP_EACH_SLOT[3] = 1;
    // FormatEx(NAME_OF_EQUIP[8], 32, "裝備 L1"); LOADED_EQUIP_EACH_SLOT[4]=1; EQUIP_DISPLAY_LIST[4][0] = 8; LOADED_EQUIP_EACH_SLOT[4] = 1;

    //Can Player Use Equip
    if( (g_CvarAllowEquip=FindConVar(TSUBAKI_ALLOW_EQUIP_COMMAND)) == null ) {
        g_CvarAllowEquip = CreateConVar(TSUBAKI_ALLOW_EQUIP_COMMAND, "1", "", FCVAR_HIDDEN, .hasMin=true, .min=0.0, .hasMax=true, .max=1.0);
    }

    if( (g_CvarCanPlayerCustomizeWeapon=FindConVar(TSUBAKI_PLAYER_CUSTOMIZE_WEAPON)) == null ) {
        g_CvarCanPlayerCustomizeWeapon = CreateConVar(TSUBAKI_PLAYER_CUSTOMIZE_WEAPON, "1", "", FCVAR_HIDDEN);
    }

    //SQL 
    BuildPath(Path_SM, buffer, 256, "data\\sqlite\\%s-sqlite.sq3", TSUBAKI_DATABASE);
    g_hDatabase = SQLInit(TSUBAKI_DATABASE, buffer);
    RetrieveItemData(g_hDatabase);
    RetrieveAbnormalData(g_hDatabase);
    RetrieveSkillData(g_hDatabase);
    RetrieveEquipData(g_hDatabase)
}

public void OnMapStart() {
    //SQL
    RetrieveGamemodeAbilityData(g_hDatabase, giCurRoundMode);

    PrecahceTsubakiLauncherModel();
    PrecahceTsubakiInvisibleToucher();
    PrecahceTsubakiLaserSprite();

    PrecacheSound("tsubaki/bgm/Adam__G_Darius.mp3");
    AddFileToDownloadsTable("sound/tsubaki/bgm/Adam__G_Darius.mp3");

    PrecahceTsubakiLauncherModel();
    PrecahceTsubakiInvisibleToucher();
    PrecahceTsubakiLaserSprite();
    PrecacheTsubakiBulletModel();


    //for debug only
    ServerCommand("%s -1", TSUBAKI_CAMERA_CONTROL_COMMAND);
}

public void OnClientPostAdminCheck(client) {
    PLY_IDENTITY(client) = IDENTITY_HUMAN;

    //TO-DO Switch To Game Mode OnPlayerSpawn
    //SDKHookEx(client, SDKHook_Spawn, OnPlayerSpawnPre);
    SDKHookEx(client, SDKHook_SpawnPost, OnPlayerSpawn);

    //FOR Debug Only
    giPlyBPSpace[client] = MAX_BP_SPACE-1;
    giPlyWHSpace[client] = MAX_WH_SPACE;

    giPlyActiveSkill[client][0][PLY_SKILL_INFO_ID] = 0;
    giPlyActiveSkill[client][1][PLY_SKILL_INFO_ID] = 1; giPlyActiveSkill[client][1][PLY_SKILL_INFO_REMAIN] = DEFAULT_SKILL_DATA[1][SKILL_DATA_DEFAULT_AMOUNT];
    giPlyActiveSkill[client][2][PLY_SKILL_INFO_ID] = 2; giPlyActiveSkill[client][2][PLY_SKILL_INFO_REMAIN] = 5;
    giPlyActiveSkill[client][3][PLY_SKILL_INFO_ID] = 3; giPlyActiveSkill[client][3][PLY_SKILL_INFO_REMAIN] = 5; gfPlyActiveSkillCooldown[client][3]=9999.0;
    
    giPlyPassiveSkill[client][0][PLY_SKILL_INFO_ID] = 0;
    giPlyPassiveSkill[client][1][PLY_SKILL_INFO_ID] = 5;

    giPlyCurrentEquip[client][0] = 0;
    giPlyCurrentEquip[client][1] = 5;
    giPlyCurrentEquip[client][2] = 6;
    giPlyCurrentEquip[client][3] = 7;

    gbsPlyEquipUnlocked[client][0] = -1;
}


public void OnPlayerSpawn(client) {
    CalculateFinalAbility(client, .spawn=true);
}




public void FunctionsValidation() {
    DisplayHUDMessage(1, "Validate", {-1.0, -1.0});
    DisplayHUDMessageAll("Validate", {-1.0, -1.0});
    PlayerItemTrigger(0, ON, 0, 0, 0.0);
}





public Action CreateEnvExplosion(int client, int args) {
    float origin[3];
    GetPlayerAimOrigin(client, origin);

    CreateTsubakiExplosion(origin);

    return Plugin_Handled;
}

#define SF_ENVEXPLOSION_NODAMAGE 0x00000001 // when set, ENV_EXPLOSION will not actually inflict damage
#define SF_ENVEXPLOSION_REPEATABLE 0x00000002 // can this entity be refired?
#define SF_ENVEXPLOSION_NOFIREBALL 0x00000004 // don't draw the fireball
#define SF_ENVEXPLOSION_NOSMOKE  0x00000008 // don't draw the smoke
#define SF_ENVEXPLOSION_NODECAL  0x00000010 // don't make a scorch mark
#define SF_ENVEXPLOSION_NOSPARKS 0x00000020 // don't make sparks
#define SF_ENVEXPLOSION_NOSOUND  0x00000040 // don't play explosion sound.
#define SF_ENVEXPLOSION_RND_ORIENT 0x00000080 // randomly oriented sprites
#define SF_ENVEXPLOSION_NOFIREBALLSMOKE 0x0100
#define SF_ENVEXPLOSION_NOPARTICLES 0x00000200
#define SF_ENVEXPLOSION_NODLIGHTS 0x00000400
#define SF_ENVEXPLOSION_NOCLAMPMIN 0x00000800 // don't clamp the minimum size of the fireball sprite
#define SF_ENVEXPLOSION_NOCLAMPMAX 0x00001000 // don't clamp the maximum size of the fireball sprite
#define SF_ENVEXPLOSION_SURFACEONLY 0x00002000 // don't damage the player if he's underwater.
stock CreateTsubakiExplosion(float origin[3]) {

    new explosion = CreateEntityByName("env_explosion");
    if(explosion == -1) {
        return -1;
    }

    DispatchKeyValueVector(explosion, "Origin", origin);
    DispatchKeyValue(explosion,"iMagnitude", "100");
    DispatchKeyValue(explosion,"iRadiusOverride", "25");
    DispatchKeyValueFloat(explosion,"DamageForce", 200.0);

    new String:flags[32];
    FormatEx(flags, sizeof(flags), "%d", 0)
    DispatchKeyValue(explosion,"spawnflags", flags);
    DispatchSpawn(explosion);
    ActivateEntity(explosion);
    
    AcceptEntityInput(explosion, "Explode");
    AcceptEntityInput(explosion, "Kill"); 
}

public Action PlayerCallWarehouseMenu(client ,args) {
    ResetPlayerMenu(client);
    PLY_MENU_ID(client) = MENUID_WH_SELL;
    DisplayWarehousePanel(client);
    
    return Plugin_Handled;
}

public Action PlayerCallCameraPanel(client, args) {
    DisplayCameraPanel(client);

    return Plugin_Handled;
}

public Action ForcePlayerLevelUp(client, args) {

    int level = 1;
    if(args > 0) {
        GetCmdArgIntEx(1, level);
    }
    LevelUp(client, level);

    return Plugin_Handled;  
}

public Action PlayerSwitchNightvision(client, args) {
    SwitchNightVision(client, !IsplayerNightVisionOn(client));

    return Plugin_Handled;
}

public Action PlayerSwitchFlashlight(client, args) {
    SwitchFlashlight(client, !IsPlayerFlashlightOn(client));

    return Plugin_Handled;
}

public Action DebugPlayMusic(client, args) {

    PlayMusic("tsubaki/bgm/Adam__G_Darius.mp3", .repeat=true, .length=117.0, .volume=0.5);

    return Plugin_Handled;
}

public Action DebugStopMusic(client, args) {
    StopCurrentMusic();

    return Plugin_Handled;
}

public Action DebugGetWeaponName(client, args) {
    if(args == 0) {
        ReplyToCommand(client, "get_weapon_name <slot(start with 0)>");
        return Plugin_Handled;
    }

    new slot = GetCmdArgInt(1), weapon_ent_id = GetPlayerWeaponSlot(client, slot), String:clsname[32];

    if( weapon_ent_id != -1) {
        GetEntityClassname(weapon_ent_id, clsname, sizeof(clsname));
        int weapon_id = GetW_IdByWeaponNameWithHeader(clsname[7]);
        PrintToChat(client, "Weapon '%s' found on slot %d, id:%d", clsname, slot, weapon_id);
    } else {
        PrintToChat(client, "no weapon found in slot %d", slot);
    }

    return Plugin_Handled;
}

public Action DebugWeaponMenu(int client, int args)
{
    static Menu menu;

    if(menu == null) {
        menu = new Menu(DebugWeaponMenuHandler, MenuAction_Select);
        menu.SetTitle("武器メニュー");
        
        char weapon[64];
        for(int i=0; i<sizeof(WEAPON_LIST); i++)
        {
            Format(weapon, 64, "weapon_%s", WEAPON_LIST[i]);
            menu.AddItem(weapon, WEAPON_LIST[i]);
        }
        menu.ExitButton = true;
    }

    menu.Display(client, MENU_TIME_FOREVER);

    return Plugin_Handled;
}

    public int DebugWeaponMenuHandler(Menu menu, MenuAction action, int client, int item)
    {
        if(action == MenuAction_Select)
        {
            PrintToChat(client, "Key Pressed : %d", item);

            char weapon[64];
            GetMenuItem(menu, item, weapon, sizeof(weapon));
            PrintToServer("Client %N Chosen %s", client, weapon);

            GivePlayerItem(client, weapon);
        }

        return 0;
    }

public Action DebugDamageAreaPanel(client, args) {
    Panel panel = new Panel();
    panel.SetTitle("【DEBUG】Damage Area");

    for(int i=0; i<TOTAL_DAMAGE_AREA_TYPE; i++) {
        panel.DrawItem(NAME_OF_DAMAGE_AREA_TYPE[i]);
    }

    DrawCancelToPanelWithNewLine(panel);
    panel.Send(client, DebugDamageAreaPanelHandler, MENU_TIME_FOREVER);
    delete panel;

    return Plugin_Handled;
}

    public int DebugDamageAreaPanelHandler(Menu menu, MenuAction action, int client, int key) {

        if(action == MenuAction_Select) {
            key-=1;
            if(key < TOTAL_DAMAGE_AREA_TYPE) {
                new Float:origin[3];
                GetPlayerAimOrigin(client, origin);

                CreateDamageArea(DEFAULT_LASER_SPRITE_ID, view_as<DamageAreaType>(key), .origin=origin, .owner=client);
                //CreateTimer(1.0, GetDamageAreaDataAtTheMiddle, ref);
            }
        }

        return 1;
    }

        public Action GetDamageAreaDataAtTheMiddle(Handle timer, int ref) {
            
            new DamageAreaType:type, Float:damage, target_identity;
            GetDamageAreaDataByRef(ref, type, damage, target_identity);

            PrintToServer("GetDamageAreaDataAtTheMiddle");
            PrintToServer("Ref : %d", ref);
            PrintToServer("Type : %d", type);
            PrintToServer("Damage : %.4f", damage);
            PrintToServer("target_identity : %b", target_identity);

            return Plugin_Stop;
        }

public Action DebugTrapPanel(client, args) {
    Panel panel = new Panel();
    panel.SetTitle("【DEBUG】Trap");

    for(int i=0; i<TOTAL_TRAP_TYPE; i++) {
        panel.DrawItem(NAME_OF_TRAP[i]);
    }

    DrawCancelToPanelWithNewLine(panel);
    panel.Send(client, DebugTrapPanelHandler, MENU_TIME_FOREVER);
    delete panel;

    return Plugin_Handled;
}

    public int DebugTrapPanelHandler(Menu menu, MenuAction action, int client, int key) {
        if(action == MenuAction_Select) {
            key-=1;
            if(key < TOTAL_TRAP_TYPE) {
                new Float:origin[3];
                GetPlayerAimOrigin(client, origin);
                origin[2] += 25.0;

                int ref = CreateTrap(.sprite_id=DEFAULT_LASER_SPRITE_ID, .model_route=DEFAULT_LAUNCHER_MODEL, .type=view_as<TrapType>(key), .origin=origin);
                PrintToServer("Trap thinking : %.4f %.4f %.4f", origin[0], origin[1], origin[2]);

                CreateTimer(2.0, GetTrapDataAtTheMiddle, ref);
            }
        }

        return 1;
    }

    public Action GetTrapDataAtTheMiddle(Handle handle, int ref) {
        new owner, TrapType:type, Float:damage;

        GetTrapDataByRef(ref, type, owner, damage);

        PrintToServer("GetTrapDataAtTheMiddle");
        PrintToServer("Ref : %d", ref);
        PrintToServer("Type : %d", type);
        PrintToServer("Damage : %.4f", damage);
        PrintToServer("owner : %b", owner);

        return Plugin_Stop;
    }

public Action ReloadServer(client, args) {

    new String:map_name[64];
    GetCurrentMap(map_name, sizeof(map_name));

    ServerCommand("sm plugins reload tsubaki_playground");
    ServerCommand("changelevel %s", map_name);

    return Plugin_Handled;
}


