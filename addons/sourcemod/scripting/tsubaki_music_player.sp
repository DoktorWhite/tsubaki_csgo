#include <sourcemod>
#include <tsubaki_v3/tsubaki_common>

#include <tsubaki_v3/tsubaki_music_player>

new Float:gfPlyMusicVolume[MAXPLAYERS];
new giCurrentMusicPlayerRefId;
new String:gszCurrentBGMPath[256];
new String:gszPreviousBGMPath[256];
new Float:gfCurrentBGMVolume;

#define TSUBAKI_MUSIC_PLAYER "tsubaki_music_player"

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
    CreateNative("PlayMusic", Native_PlayMusic);
    CreateNative("SetPlayerMusicVolume", Native_SetPlayerMusicVolume);
    CreateNative("StopCurrentMusic", Native_StopCurrentMusic);
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

public Native_PlayMusic(Handle:plugin, numParmas) {
    if(g_hMusicTask != INVALID_HANDLE) {
        KillTimer(g_hMusicTask);
    }
    
    if(gszCurrentBGMPath[0] != 0) {
        gszPreviousBGMPath[0] = 0;
        FormatEx(gszPreviousBGMPath, sizeof(gszPreviousBGMPath), gszCurrentBGMPath);
        StopMusic(gszPreviousBGMPath);
    }

    new bool:repeat = GetNativeCell(2), Float:length = GetNativeCell(3);
    GetNativeString(1, gszCurrentBGMPath, sizeof(gszCurrentBGMPath));
    gfCurrentBGMVolume = GetNativeCell(4);

    if(giCurrentMusicPlayerRefId == 0 || !IsValidEntity(giCurrentMusicPlayerRefId) ) {
        giCurrentMusicPlayerRefId = CreateInvisibleLauncher(TSUBAKI_MUSIC_PLAYER, DEFAULT_LAUNCHER_MODEL, .get_reference=true);
    }

    CreateTimer(0.1, DelayPlayMusic, .flags=TIMER_FLAG_NO_MAPCHANGE);

    if(repeat) {
        g_hMusicTask = CreateTimer(length+0.1, RepeatMusic, .flags=TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
    } else {
        g_hMusicTask = CreateTimer(length+0.1, TaskStopMusic, .flags=TIMER_FLAG_NO_MAPCHANGE);
    }
}

    public Action DelayPlayMusic(Handle timer) {
        for(int client=1; client<=MaxClients; client++) {
            if(IsClientInGame(client)) {
                EmitSoundToClient(client
                    , .sample=gszCurrentBGMPath
                    , .entity=giCurrentMusicPlayerRefId
                    , .channel=SNDCHAN_STATIC
                    , .level=SNDLEVEL_NONE
                    , .flags=SND_NOFLAGS
                    , .volume=gfCurrentBGMVolume
                );
            }
        }

        return Plugin_Stop;
    }

    public Action TaskStopMusic(Handle timer) {
        StopCurrentMusic();

        g_hMusicTask = INVALID_HANDLE;
        return Plugin_Stop;
    }

    public Native_StopCurrentMusic(Handle:plugin, numParams) {
        if(gszCurrentBGMPath[0] != 0) {
            StopMusic(gszCurrentBGMPath);
            gszCurrentBGMPath[0] = 0;
        }
    }

    void StopMusic(String:music_path[]) {
        PrintToServer("Stopping %s", music_path);

        EmitSoundToAll(
            .sample=music_path
            , .entity=giCurrentMusicPlayerRefId
            , .channel=SNDCHAN_STATIC
            , .level=SNDLEVEL_NONE
            , .flags=SND_STOP
            , .volume=0.0
        );
        
        gszCurrentBGMPath[0] = 0;
    }

    public Action RepeatMusic(Handle timer) {
        for(int client=1; client<=MaxClients; client++) {
            if(IsClientInGame(client)) {
                EmitSoundToClient(client
                    , .sample=gszCurrentBGMPath
                    , .entity=0
                    , .channel=SNDCHAN_STATIC
                    , .level=SNDLEVEL_NONE
                    , .flags=SND_NOFLAGS
                    , .volume=gfCurrentBGMVolume
                );
            }
        }
        return Plugin_Continue;
    }