#include <sourcemod>
#include <tsubaki_v3/common>

new Float:gfPlyMusicVolume[MAXPLAYERS];
new giCurrentMusicPlayerRefId;
new String:gszCurrentBGMPath[256];

Handle g_hMusicTask;

public Plugin myinfo = 
{
    name = "TsuBaKi Camera",
    author = "WhiteCola",
    description = "",
    version = "0.0",
    url = ""
}

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max) {
    CreateNative("PlayerMusic", Native_PlayerMusic);
    CreateNative("SetPlayerMusicVolume", Native_SetPlayerMusicVolume);
    return APLRes_Success;
}

public OnMapStart() {
    giCurrentMusicPlayerRefId = 0;
    gszCurrentBGMPath[0] = 0;
    g_hMusicTask = INVALID_HANDLE;

    PrecahceTsubakiLauncherModel();
}

public Native_SetPlayerMusicVolume(Handle:plugin, numParams) {
    gfPlyMusicVolume[GetNativeCell(1)] = GetNativeCell(2);
}

public Native_PlayerMusic(Handle:plugin, numParmas) {
    if(g_hMusicTask != INVALID_HANDLE) {
        KillTimer(g_hMusicTask);
    }
}

    public Action RemoveMusic(Handle timer) {
        RemoveEntity(giCurrentMusicPlayerRefId);
        giCurrentMusicPlayerRefId = 0;
        
        g_hMusicTask = INVALID_HANDLE;
        return Plugin_Stop;
    }

    public Action RepeatMusic(Handle timer) {

        return Plugin_Continue;
    }