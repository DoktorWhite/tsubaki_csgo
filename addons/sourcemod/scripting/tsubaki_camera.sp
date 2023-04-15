#include <sourcemod>
#include <sdktools_trace>
#include <tsubaki_v3/tsubaki_common>
#include <tsubaki_v3/tsubaki_menu>
#include <tsubaki_v3/tsubaki_camera>

#pragma semicolon 1

public Plugin myinfo = 
{
    name = "TsuBaKi Camera",
    author = "WhiteCola",
    description = "",
    version = "0.0",
    url = ""
}

ConVar g_CvarCameraControl;
int giPlyCameraRefId[MAXPLAYERS] = {0, ...};
int giPlyCameraMode[MAXPLAYERS] = {0, ...};
bool gbPlyCameraThink[MAXPLAYERS] = {false, ...};
float gfPlyCameraOriginDistance[MAXPLAYERS];
float gfPlyCameraOriginAngles[MAXPLAYERS][3];
float gfPlyCameraViewAngles[MAXPLAYERS][3];
    #define ANGLE_FOLLOW_PLAYER -999.0

    #define ResetPlayerCamera(%0)   SetClientViewEntity(%0,%0); ClientCommand(%0, "firstperson"); giPlyCameraMode[%0]=CAMERA_1ST; gbPlyCameraThink[%0]=false

    public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max) {
        CreateNative("DisplayCameraPanel", Native_DisplayCameraPanel);
        CreateNative("ChangePlayerCamera", Native_ChangePlayerCamera);
        return APLRes_Success;
    }
    
    public void OnPluginStart() {

        HookEvent("player_death", OnPlayerDeathPre, EventHookMode_Pre);

        if( (g_CvarCameraControl=FindConVar(TSUBAKI_CAMERA_CONTROL_COMMAND)) == null) {
            g_CvarCameraControl = CreateConVar("tsubaki_camera_control", "-1", "", FCVAR_HIDDEN, .hasMin=false, .hasMax=false);
        }
        g_CvarCameraControl.AddChangeHook(OnCvarCameraControlChanged);
    }

    public void OnMapStart() {
        for(int client=0; client<MAXPLAYERS; client++) {
            giPlyCameraRefId[client] = 0;
            giPlyCameraMode[client] = 0;
            gbPlyCameraThink[client] = false;
        }
    }
    
        public Action OnPlayerDeathPre(Event event, const char[] name, bool dontBroadcast) {
            int client = GetClientOfUserId(event.GetInt(e_UserID));

            ResetPlayerCamera(client);

            return Plugin_Continue;
        }

    public void OnClientDisconnect(client) {
        giPlyCameraMode[client] = 0;
        gbPlyCameraThink[client] = false;
    }

    public void OnCvarCameraControlChanged(ConVar convar, const char[] oldValue, const char[] newValue) {
        

        for(int client=1; client<=MaxClients; client++) {
            if(IsClientInGame(client) && IsPlayerAlive(client) && (~convar.IntValue&giPlyCameraMode[client])) {
                ResetPlayerCamera(client);
            }
        }
    }


    public Native_DisplayCameraPanel(Handle:plugin, numParams) {
        static Panel panel;

        if(panel == null) {
            panel = new Panel();

            panel.SetTitle("視角");
            for(int i=0; i<TOTAL_CAMERA_MODE; i++) {
                panel.DrawItem(NAME_OF_CAMERA[i]);
            }

            panel.DrawText(EMPTY_LINE);
            panel.CurrentKey = 9;
            panel.DrawItem(CANCEL);

            panel.SetKeys(MENU_KEYS_ALL);
        }

        panel.Send(GetNativeCell(1), CameraPanelHandler, MENU_TIME_FOREVER);
    }

        public int CameraPanelHandler(Menu menu, MenuAction action, int client, int key) {
            if(action==MenuAction_Select && 0 < key <= TOTAL_CAMERA_MODE) {
                if(ChangePlayerCamera(client, key-1) == -2) {
                    PrintToChat(client, " \x07現在不能使用此視角");
                }
            }
            return 1;
        }

    public int Native_ChangePlayerCamera(Handle:plugin, numParams) {
        int client = GetNativeCell(1), mode=GetNativeCell(2);
        // 無效視角
        if(mode<0 || mode>=TOTAL_CAMERA_MODE) {
            return -1;
        }

        //指令 tsubaki_camera_control 的值不允許對應視角
        if( (~g_CvarCameraControl.IntValue)&(1<<mode) ) {
            return -2;
        }

        if(giPlyCameraRefId[client] == 0) {
            giPlyCameraRefId[client] = CreateInvisibleLauncher(.clsname=TSUBAKI_CAMERA, .model_route=DEFAULT_LAUNCHER_MODEL, .owner=client, .get_reference=true);
        }

        if(IsPlayerAlive(client)) {
            giPlyCameraMode[client] = mode;
            gfPlyCameraOriginDistance[client] = CAMERA_DISTANCE[mode];
            
            SetEntPropEnt(client, Prop_Send, m_hObserverTarget, CAMERA_PROP[mode][CAMERA_PROP_OBSERVER_TARGET]);
            SetEntProp(client, Prop_Send, m_iObserverMode, CAMERA_PROP[mode][CAMERA_PROP_OBSERVERMODE]);
            //SetEntProp(client, Prop_Send, m_bDrawViewmodel, CAMERA_PROP[mode][CAMERA_PROP_DRAWVIEWMODEL]);

            switch(mode) {
                case CAMERA_1ST: {

                    SetClientViewEntity(client, client);
                    ClientCommand(client, "firstperson");

                    gbPlyCameraThink[client] = false;
                }
                case CAMERA_FIX: {
                    GetClientEyePosition(client, gfPlyCameraOriginAngles[client]);
                    GetClientEyeAngles(client, gfPlyCameraViewAngles[client]);

                    TeleportEntity(giPlyCameraRefId[client], gfPlyCameraOriginAngles[client], gfPlyCameraViewAngles[client], NULL_VECTOR);
                    
                    SetClientViewEntity(client, giPlyCameraRefId[client]);
                    ClientCommand(client, "firstperson");

                    gbPlyCameraThink[client] = false;
                }
                case CAMERA_TOP_DOWN: {
                    gfPlyCameraOriginAngles[client][0]=ToRadian(90.0); gfPlyCameraOriginAngles[client][1]=0.0; gfPlyCameraOriginAngles[client][2]=0.0;
                    gfPlyCameraViewAngles[client][0]=ToRadian(-90.0); gfPlyCameraViewAngles[client][1]=ANGLE_FOLLOW_PLAYER; gfPlyCameraViewAngles[client][2]=ANGLE_FOLLOW_PLAYER;

                    SetClientViewEntity(client, giPlyCameraRefId[client]);
                    ClientCommand(client, "firstperson");
                                        
                    gbPlyCameraThink[client] = true;
                    RequestFrame(CameraTopDownThink, client);
                }
                default: {

                    SetClientViewEntity(client, client);
                    
                    SetEntPropEnt(client, Prop_Send, m_hObserverTarget, -1);
                    SetEntProp(client, Prop_Send, m_iObserverMode, 0);
                    ClientCommand(client, "thirdperson");
                    ClientCommand(client, "cam_idealdist %.0f", gfPlyCameraOriginDistance[client]);

                    gbPlyCameraThink[client] = false;

                }
            }

        }

        return 1;
    }

        public CameraTopDownThink(client) {
            static Float:origin[3], Float:dest[3], Float:angles[3];
            
            if(gbPlyCameraThink[client]) {
                GetClientAbsOrigin(client, origin);

                dest[0] = origin[0] + gfPlyCameraOriginDistance[client] * Cosine(gfPlyCameraOriginAngles[client][0]) * Cosine(gfPlyCameraOriginAngles[client][1]);
                dest[1] = origin[1] + gfPlyCameraOriginDistance[client] * Cosine(gfPlyCameraOriginAngles[client][0]) * Sine(gfPlyCameraOriginAngles[client][1]);
                dest[2] = origin[2] + gfPlyCameraOriginDistance[client] * Sine(gfPlyCameraOriginAngles[client][0]);
                
                TR_TraceRayFilter(origin, dest, MASK_SOLID, RayType_EndPoint, FilterAllEntity, client);
                TR_GetEndPosition(origin, INVALID_HANDLE);

                GetClientEyeAngles(client, angles);
                angles[0] = 90.0;

                TeleportEntity(giPlyCameraRefId[client], origin, angles, NULL_VECTOR);
                RequestFrame(CameraTopDownThink, client);
            }
        }