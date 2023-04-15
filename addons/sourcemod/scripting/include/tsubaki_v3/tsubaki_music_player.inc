#if defined _tsubaki_music_player
    #endinput
#endif
#define _tsubaki_music_player

#pragma semicolon 1
/**
 * 設定玩家音樂大小
 */
native SetPlayerMusicVolume(client, Float:volume);

/**
 * 播放Server 音樂
 * 
 * @param music_path 音樂檔案位置（可直接使用PrecacheSound 的位置）
 * @param repeat true:重複播放
 * @param length 長度
 */
native PlayMusic(String:music_path[], bool:repeat, Float:length, Float:volume);

/**
 * 停止現時在播放的音樂
 */
native StopCurrentMusic();

#pragma semicolon 0