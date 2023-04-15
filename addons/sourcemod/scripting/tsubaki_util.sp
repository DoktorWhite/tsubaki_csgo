#include <tsubaki_v3/tsubaki_util>

#pragma semicolon 1

public Plugin myinfo = 
{
    name = "TsuBaKi Util",
    author = "WhiteCola",
    description = "",
    version = "0.0",
    url = ""
}


public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max) {
    CreateNative("SwitchNightVision", Native_SwitchNightVision);
    CreateNative("SwitchFlashlight", Native_SwitchFlashlight);
    return APLRes_Success;
}

public void OnPluginStart() {
    HookEvent("player_death", OnPlayerDeathPre, EventHookMode_Pre);
}

public Native_SwitchNightVision(Handle:plugin, numParams) {
    int client = GetNativeCell(1);
    bool on = GetNativeCell(2);

    if(IsPlayerAlive(client))
        SetEntProp(client, Prop_Send, m_bNightVisionOn, on?1:0);
}

public Native_SwitchFlashlight(Handle:plugin, numParams) {
    int client = GetNativeCell(1);
    bool on = GetNativeCell(2);

    if(IsPlayerAlive(client)) {
        if(on) {
            SetEntProp( client, Prop_Send, m_fEffects, GetEntProp(client, Prop_Send, m_fEffects)|(FLASHLIGHT_BIT) );

        } else {
            SetEntProp( client, Prop_Send, m_fEffects, GetEntProp(client, Prop_Send, m_fEffects)&(~FLASHLIGHT_BIT) );
        }
    }
}

public Action OnPlayerDeathPre(Event event, const char[] name, bool dontBroadcast) {
    static String:e_UserID[] = "e_UserID";

    int client = GetClientOfUserId(event.GetInt(e_UserID));

    if(1<=client<=MaxClients && IsClientInGame(client)) {
        SwitchNightVision(client, .on=false);
        SwitchFlashlight(client, .on=false);
    }

    return Plugin_Continue;
} 
