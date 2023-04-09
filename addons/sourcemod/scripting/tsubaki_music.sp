#include <sourcemod>
#include <tsubaki_v3/common>

new Float:gfPlyMusicVolume[MAXPLAYERS];
new giCurrentMusicPlayerRefId;
new String:gszCurrentBGMPath[256];
new Float:gfCurrentBGMVolume;

Handle g_hMusicTask;

public Plugin myinfo = 
{
    name = "TsuBaKi Music Player",
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

    if(giCurrentMusicPlayerRefId != 0) {
        RemoveEntity(CreateInvisibleLauncher);
    }

    new client, bool:repeat = GetNativeCell(2), Float:length = GetNativeCell(3);
    GetNativeString(1, gszCurrentBGMPath, sizeof(gszCurrentBGMPath));
    gfCurrentBGMVolume = GetNativeCell(4);

    PrintToServer("Duration : %.4f", GetSoundDuration(gszCurrentBGMPath));

    giCurrentMusicPlayerRefId = CreateInvisibleLauncher(TSUBAKI_MUSIC_PLAYER, DEFAULT_LAUNCHER_MODEL, .get_reference=true);
    if(IsValidEntity(giCurrentMusicPlayerRefId)) {
        for(; client<=MaxClients; client++) {
            if(IsClientInGame(client)) {
                EmitSoundToClient(client
                    , .sample=gszCurrentBGMPath
                    , .entity=giCurrentMusicPlayerRefId
                    , .channel=SNDCHAN_AUTO
                    , .level=SNDLEVEL_NORMAL
                    , .flags=SND_NOFLAGS
                    , .volume=gfCurrentBGMVolume
                );
            }
        }
    }

    if(repeat) {
        g_hMusicTask = CreateTimer(length, RepeatMusic, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
    } else {
        g_hMusicTask = CreateTimer(length, StopMusic, TIMER_FLAG_NO_MAPCHANGE);
    }
}

    public Action StopMusic(Handle timer) {
        RemoveEntity(giCurrentMusicPlayerRefId);
        giCurrentMusicPlayerRefId = 0;
        
        g_hMusicTask = INVALID_HANDLE;
        return Plugin_Stop;
    }

    public Action RepeatMusic(Handle timer) {
        RemoveEntity(giCurrentMusicPlayerRefId);
        giCurrentMusicPlayerRefId = CreateInvisibleLauncher(TSUBAKI_MUSIC_PLAYER, DEFAULT_LAUNCHER_MODEL, .get_reference=true);

        if(IsValidEntity(giCurrentMusicPlayerRefId)) {
                for(; client<=MaxClients; client++) {
                    if(IsClientInGame(client)) {
                        EmitSoundToClient(client
                            , .sample=gszCurrentBGMPath
                            , .entity=giCurrentMusicPlayerRefId
                            , .channel=SNDCHAN_AUTO
                            , .level=SNDLEVEL_NORMAL
                            , .flags=SND_NOFLAGS
                            , .volume=gfCurrentBGMVolume
                        );
                    }
                }
            }
        return Plugin_Continue;
    }