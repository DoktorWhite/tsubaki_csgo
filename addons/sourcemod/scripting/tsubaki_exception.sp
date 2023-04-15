#include <sourcemod>
#include <tsubaki_v3/tsubaki_hud>

new String:CURRENT_ERROR_MSG[512];

    public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max) {
        CreateNative("SetTsubakiFailState", Native_SetTsubakiFailState);
        return APLRes_Success;
    }    

    public Native_SetTsubakiFailState(Handle:plugin, numParams) {
        GetNativeString(1, CURRENT_ERROR_MSG, sizeof(CURRENT_ERROR_MSG));

        CreateTimer(1.0, ContinuouslyDisplayFailState, _, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
    }

    public Action ContinuouslyDisplayFailState(Handle timer) {

        DisplayHUDMessageAll(CURRENT_ERROR_MSG
                            , .position=Float:{-1.0,-1.0}
                            , .colors={255,0,0,255}
                            , .last_time=1.0);

        return Plugin_Continue;
    }

    