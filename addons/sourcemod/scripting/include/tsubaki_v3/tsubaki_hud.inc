#if defined _tsubaki_hud
    #endinput
#endif
#define _tsubaki_hud

/*  HUD Channel  */
//Please Add HUD Channel to here also as a comment for management
//#define HUD_CHN_GAME_STATUS 1       //tsubaki_game.inc

/**
 * 對指定玩家發送HUD 信息
 * 
 * @param client 玩家ID
 * @param message 顯示信息
 * @param position 表示位置(0.0~1.0), -1.0為中央
 * @param channel 頻道
 * @param last_time 顯示長度
 * @param fade_in_time 漸漸顯示時間
 * @param fade_out_time 漸漸消失時間
 * @param colors HUD顏色
 */
stock void DisplayHUDMessage(client, String:message[], Float:position[2], channel=-1, Float:last_time=1.0, Float:fade_in_time=0.0, Float:fade_out_time=0.0, int colors[4]={50, 255, 50, 255}) {

    if(1<=client<=MaxClients && IsClientInGame(client) && !IsFakeClient(client)) {
        SetHudTextParams(position[0], position[1], last_time, colors[0], colors[1], colors[2], colors[3], 1, 0.0, fade_in_time, fade_out_time);
        ShowHudText(client, channel, message);
    }

}

/**
 * 對全部玩家發送HUD 信息
 *
 * @param message 顯示信息
 * @param position 表示位置(0.0~1.0), -1.0為中央
 * @param channel 頻道
 * @param last_time 顯示長度
 * @param fade_in_time 漸漸顯示時間
 * @param fade_out_time 漸漸消失時間
 * @param colors HUD顏色
*/
stock void DisplayHUDMessageAll(char[] message, Float:position[2], channel=-1, Float:last_time=1.0, Float:fade_in_time=0.0, Float:fade_out_time=0.0, int colors[4]={50, 255, 50, 255}) {
    SetHudTextParams(position[0], position[1], last_time, colors[0], colors[1], colors[2], colors[3], 1, 0.0, fade_in_time, fade_out_time);
    for(new client=1; client<MaxClients; client++) {
        if(IsClientInGame(client) && !IsFakeClient(client)) {
            ShowHudText(client, channel, message);
        }
    }
}