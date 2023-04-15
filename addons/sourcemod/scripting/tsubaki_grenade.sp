#include <sourcemod>
#include <sdktools>

#include <tsubaki_v3/tsubaki_grenade>

#pragma semicolon 1

Grenade g_Grenade[MAXPLAYERS];
ConVar g_CvarCanCustomizeGrenade;

//
    char NAME_OF_EXPLODE_TYPE[TOTAL_GRENADE_EXPLODE_TYPE+1][] = {
        "普通"
        , "觸碰後爆炸"
        , "指定時間後爆炸"
        , "停止後爆炸"
        , ""
    };

    char NAME_OF_GRENADE_TYPE[TOTAL_GRENADE_TYPE+1][] = {
        "普通"
        , "特效範圍攻擊"
        , "陷阱"
        , ""
    };

//classnames
    //char PROJECTILE_HEADER[] = "_projectile";
    // char INFERNO[] = "inferno";
    char HEGRENADE_PROJECTILE[] = "hegrenade_projectile";
    char SMOKEGRENADE_PROJECTILE[] = "smokegrenade_projectile";
    char FLASHBANG_PROJECTILE[] = "flashbang_projectile";
    char DECOY_PROJECTILE[] = "decoy_projectile";
    // char MOLOTOV_PROJECTILE[] = "molotov_projectile";
    char TAGRENADE_PROJECTILE[] = "tagrenade_projectile";

//models / sprites
    stock String:DEFAULT_LAUNCHER_MODEL[] = "models/blackout.mdl";
    stock DEFAULT_LASER_SPRITE_ID;

//Ent Properties
    stock String:m_nNextThinkTick[] = "m_nNextThinkTick";
    stock String:m_vecOrigin[] = "m_vecOrigin";
    #define GetEntityOrigin(%0,%1)  GetEntPropVector(%0, Prop_Send, m_vecOrigin, %1)

    stock String:m_vecVelocity[] = "m_vecVelocity";
    #define GetEntityVelocity(%0,%1)    GetEntPropVector(%0, Prop_Data, m_vecVelocity, %1)

    stock String:m_hOwnerEntity[] = "m_hOwnerEntity";
    #define GetEntityOwner(%0)			GetEntPropEnt(%0,Prop_Data,m_hOwnerEntity)

//Menu

	stock String:MENU_SOUND[] = "buttons/button14.wav";
	#define EMIT_MENU_SOUND(%0) EmitSoundToClient(%0, MENU_SOUND, %0, SNDCHAN_ITEM, SNDLEVEL_NONE, SND_NOFLAGS, 1.0, SNDPITCH_NORMAL, client)
	#define MENU_KEYS_ALL	(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9)

public Plugin myinfo = 
{
    name = "Tsubaki Grenade",
    author = "WhiteCola",
    description = "Kano-Bi CSGO Version",
    version = "0.0",
    url = ""
}

    public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max) {
        CreateNative("SwitchGrenadeExplodeType", Native_SwitchGrenadeExplodeType);
        CreateNative("SwitchGrenadeType", Native_SwitchGrenadeType);
        CreateNative("SwitchGrenadeLaserSprite", Native_SwitchGrenadeLaserSprite);
        CreateNative("SwitchCustomGrenadeOnOff", Native_SwitchCustomGrenadeOnOff);

        return APLRes_Success;
    }

    public OnPluginStart() {
        RegConsoleCmd("grenade", PlayerCallCustomGrenadePanel);
    }

    public OnConfigsExecuted() {
        if( (g_CvarCanCustomizeGrenade=FindConVar(CVAR_CAN_CUSTOMIZE_GRENADE)) == null ) {
            g_CvarCanCustomizeGrenade = CreateConVar(CVAR_CAN_CUSTOMIZE_GRENADE, "1", "", FCVAR_HIDDEN);
            g_CvarCanCustomizeGrenade.AddChangeHook(OnCustomGrenadeCvarChanged);
        }
    }

    public OnMapStart() {
        DEFAULT_LASER_SPRITE_ID=PrecacheModel("materials/sprites/laserbeam.vmt");
        AddFileToDownloadsTable("materials/sprites/laserbeam.vmt");

        PrecacheModel(DEFAULT_LAUNCHER_MODEL, false);
    }

    public OnClientPostAdminCheck(client) {
        g_Grenade[client].grenade_explode_type = GrenadeExplodeType_Normal;
        g_Grenade[client].grenade_type = GrenadeType_Normal;
        g_Grenade[client].laser_sprite_id = DEFAULT_LASER_SPRITE_ID;
        g_Grenade[client].grenade_trigger_time = 5.0;
        g_Grenade[client].enable = (g_CvarCanCustomizeGrenade.IntValue==CAN_CUSTOMIZE_GRENADE);
    }

    public OnCustomGrenadeCvarChanged(ConVar convar, const char[] oldValue, const char[] newValue) {
        for(int client=1; client<=MaxClients; client++) {
            g_Grenade[client].enable = (convar.IntValue==CAN_CUSTOMIZE_GRENADE);
        }
    }

/*  Grenade Entity  */
    public void OnEntityCreated(int ent, const char[] clsname) {
        switch(clsname[0]) {
            case 'h':
            {
                if(StrEqual(clsname, HEGRENADE_PROJECTILE))
                    SDKHook(ent, SDKHook_SpawnPost, OnProjectileSpawnPost);
            }
            case 's':
            {
                if(StrEqual(clsname, SMOKEGRENADE_PROJECTILE))
                    SDKHook(ent, SDKHook_SpawnPost, OnProjectileSpawnPost);
            }
            case 'f':
            {
                if(StrEqual(clsname, FLASHBANG_PROJECTILE))
                    SDKHook(ent, SDKHook_SpawnPost, OnProjectileSpawnPost);
            }
            case 'd':
            {
                if(StrEqual(clsname, DECOY_PROJECTILE))
                    SDKHook(ent, SDKHook_SpawnPost, OnProjectileSpawnPost);
            }
            // case 'i':
            // {
            //     if(StrEqual(clsname, INFERNO))
            //         SDKHook(ent, SDKHook_SpawnPost, InfernoSpawn);
            // }
            // case 'm':
            // {
            //     if(StrEqual(clsname, MOLOTOV_PROJECTILE))
            //         SDKHook(ent, SDKHook_SpawnPost, OnProjectileSpawnPost);
            // }
            case 't':
            {
                if(StrEqual(clsname, TAGRENADE_PROJECTILE))
                    SDKHook(ent, SDKHook_SpawnPost, OnProjectileSpawnPost);
            }
        }
    }

    public void OnProjectileSpawnPost(int ref) {
        static owner;

        if( 1<=(owner=GetEntityOwner(ref))<=MaxClients ) {
            ref = EntIndexToEntRef(ref);

            switch(g_Grenade[owner].grenade_explode_type) {
                case GrenadeExplodeType_Touch: {
                    SDKHookEx(ref, SDKHook_Touch, OnGrenadeTouch);
                    RequestFrame(StopGrenadeThinkingFrame, ref);
                }
                case GrenadeExplodeType_Time: {
                    CreateTimer(g_Grenade[owner].grenade_trigger_time, OnGrenadeTimesUp, ref, TIMER_FLAG_NO_MAPCHANGE);
                    RequestFrame(StopGrenadeThinkingFrame, ref);
                }
                case GrenadeExplodeType_ZeroVelocity: {
                    CreateTimer(0.2, CheckGrenadeZeroVelocityTask, ref, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
                    RequestFrame(StopGrenadeThinkingFrame, ref);
                }
            }
        }
    }

    void StopGrenadeThinkingFrame(int ref)
    {
        SetEntProp(ref, Prop_Data, m_nNextThinkTick, -1); // block grenade explosion
    }

    public void OnGrenadeTouch(int ent, int target) {
        ent = EntIndexToEntRef(ent);
        ProcessGrenadeEffect(ent);
    }

    public Action OnGrenadeTimesUp(Handle timer, int ref) {
        ProcessGrenadeEffect(ref);
        return Plugin_Stop;
    }

    public Action CheckGrenadeZeroVelocityTask(Handle timer, int ref) {
        static Float:velocity[3];

        if(!IsValidEntity(ref)) {
            return Plugin_Stop;
        }

        GetEntityVelocity(ref, velocity);
        if( velocity[0]==0.0 && velocity[1]==0.0 && velocity[2]==0.0 ) {
            ProcessGrenadeEffect(ref);
            return Plugin_Stop;
        }

        return Plugin_Continue;
    }

    void ProcessGrenadeEffect(&ref) {
        static owner, Float:origin[3];
        owner = GetEntityOwner(ref);
        GetEntityOrigin(ref, origin);

        switch(g_Grenade[owner].grenade_type) {
            case GrenadeType_Normal: {
                SetEntProp(ref, Prop_Data, m_nNextThinkTick, GetGameTickCount()+1);
                return;
            }
            case GrenadeType_Damage_Area: {
                CreateDamageArea(.laser_sprite_id=g_Grenade[owner].laser_sprite_id
                                , .type=g_Grenade[owner].damage_area_type
                                , .origin=origin
                                , .owner=owner
                                , .colors=DEFAULT_DAMAGE_AREA_COLORS[g_Grenade[owner].damage_area_type]);
            }
            case GrenadeType_Trap: {
                CreateTrap(.sprite_id=g_Grenade[owner].laser_sprite_id
                            , .model_route=DEFAULT_LAUNCHER_MODEL
                            , .type=g_Grenade[owner].trap_type
                            , .origin=origin
                            , .last_time=45.0
                            , .owner=owner
                            , .colors=DEFAULT_TRAP_COLORS[g_Grenade[owner].trap_type]);
            }
        }

        RemoveEntity(ref);
    }

    // public void InfernoSpawn(int ent) {
    //     new owner = GetEntityOwner(ent);
    //     if( owner<=0 || owner>MaxClients)
    //         return;   

    //     if(g_Grenade[owner].grenade_type != GrenadeType_Normal) {
    //         RemoveEntity(ent);
    //     }
    // }

/*  Player Settings  */

    public Native_SwitchGrenadeLaserSprite(Handle:plugin, numParams) {
        g_Grenade[GetNativeCell(1)].laser_sprite_id = GetNativeCell(2);
    }

    public Native_SwitchGrenadeExplodeType(Handle:plugin, numParams) {
        int client = GetNativeCell(1);
        g_Grenade[client].grenade_explode_type = GetNativeCell(2);
        switch(g_Grenade[client].grenade_explode_type) {
            case GrenadeExplodeType_Time: {
                g_Grenade[client].grenade_trigger_time = GetNativeCell(3);
            }
        }
    }

    public Native_SwitchGrenadeType(Handle:plugin, numParams) {
        int client = GetNativeCell(1);
        switch( (g_Grenade[client].grenade_type = GetNativeCell(2)) ) {
            case GrenadeType_Trap: {
                g_Grenade[client].trap_type = GetNativeCell(3);
            }
            case GrenadeType_Damage_Area: {
                g_Grenade[client].damage_area_type = GetNativeCell(3);
            }
        }
    }

    public Native_SwitchCustomGrenadeOnOff(Handle:plugin, numParams) {
        g_Grenade[GetNativeCell(1)].enable = GetNativeCell(2);
    }

    public Action PlayerCallCustomGrenadePanel(client, args) {

        DisplayCustomGrenadePanel(client);

        return Plugin_Handled;
    }

        void DisplayCustomGrenadePanel(&client) {
            new Panel:panel = new Panel(), String:menu_msg[64];

            static String:MENU_TITLE[] = "自訂手榴彈\n　\n";
            static String:EXPLOSION_TYPE_FMT[] = "引爆方式:%s";
            static String:GRENADE_TYPE_FMT[] = "手榴彈種類:%s";
            static String:EMPTY_LINE[] = "　";
            static String:CANCEL[] = "取消";

            panel.SetTitle(MENU_TITLE);

            FormatEx(menu_msg, sizeof(menu_msg), EXPLOSION_TYPE_FMT, NAME_OF_EXPLODE_TYPE[g_Grenade[client].grenade_explode_type]);
            panel.DrawItem(menu_msg);
            menu_msg[0] = 0;

            FormatEx(menu_msg, sizeof(menu_msg), GRENADE_TYPE_FMT, NAME_OF_GRENADE_TYPE[g_Grenade[client].grenade_type]);
            panel.DrawItem(menu_msg);
            menu_msg[0] = 0;

            panel.DrawText(EMPTY_LINE);

            panel.CurrentKey = 4;
            switch(g_Grenade[client].grenade_type) {
                case GrenadeType_Damage_Area: {
                    static String:DAMAGE_AREA_FMT[] = "特效範圍攻擊種類:%s";

                    FormatEx(menu_msg, sizeof(menu_msg), DAMAGE_AREA_FMT, NAME_OF_DAMAGE_AREA_TYPE[g_Grenade[client].damage_area_type]);
                    panel.DrawItem(menu_msg);
                    menu_msg[0] = 0;
                }
                case GrenadeType_Trap: {
                    static String:TRAP_TYPE_FMT[] = "陷阱種類:%s";

                    panel.CurrentKey = 4;
                    FormatEx(menu_msg, sizeof(menu_msg), TRAP_TYPE_FMT, NAME_OF_TRAP[g_Grenade[client].trap_type]);
                    panel.DrawItem(menu_msg);
                    menu_msg[0] = 0;
                }
            }

            switch(g_Grenade[client].grenade_explode_type) {
                case GrenadeExplodeType_Time: {
                    static String:EXPLOSION_TIME_FMT[] = "引爆時間:%.0f秒";

                    panel.CurrentKey = 5;
                    FormatEx(menu_msg, sizeof(menu_msg), EXPLOSION_TIME_FMT, g_Grenade[client].grenade_trigger_time);
                    panel.DrawItem(menu_msg);
                    menu_msg[0] = 0;
                }
            }
            
            panel.DrawText(EMPTY_LINE);
            panel.DrawItem(CANCEL);

            panel.CurrentKey = 9;
            panel.SetKeys(-1);
            panel.Send(client, CustomGrenadePanelHandler, MENU_TIME_FOREVER);
            delete panel;
        }

            public int CustomGrenadePanelHandler(Menu menu, MenuAction action, int client, int key) {

                if(action == MenuAction_Select) {
                    switch(key) {
                        case 1:{ 
                            g_Grenade[client].grenade_explode_type = (g_Grenade[client].grenade_explode_type+view_as<GrenadeExplodeType>(1))%GrenadeExplodeType_Last;
                        }
                        case 2:{
                            g_Grenade[client].grenade_type = (g_Grenade[client].grenade_type+view_as<GrenadeType>(1))%GrenadeType_Last;
                        }
                        case 4: {
                            switch(g_Grenade[client].grenade_type) {
                                case GrenadeType_Damage_Area: {
                                    g_Grenade[client].damage_area_type = (g_Grenade[client].damage_area_type+view_as<DamageAreaType>(1)) % DamageArea_Last;
                                }
                                case GrenadeType_Trap: {
                                    g_Grenade[client].trap_type = (g_Grenade[client].trap_type+view_as<TrapType>(1)) % TrapType_Last;
                                }
                            }
                        }
                        case 5: {
                            switch(g_Grenade[client].grenade_explode_type) {
                                case GrenadeExplodeType_Time: {
                                    g_Grenade[client].grenade_trigger_time = (RoundFloat(g_Grenade[client].grenade_trigger_time+5)%15+5)*1.0;
                                }
                            }
                        }
                        default: return 1;
                    }

                    DisplayCustomGrenadePanel(client);
                }

                return 1;
            }
