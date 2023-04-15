#if defined _tsubaki_util
    #endinput
#endif
#define _tsubaki_util

#include <sourcemod>

stock String:m_bNightVisionOn[] = "m_bNightVisionOn";

/**
 * 開啟/關閉夜視
 */
native SwitchNightVision(client, bool:on);

#define IsplayerNightVisionOn(%0) (GetEntProp(client, Prop_Send, m_bNightVisionOn))


stock String:m_fEffects[] = "m_fEffects";
#define FLASHLIGHT_BIT  4
/**
 * 開啟/關閉手電筒
 */
native SwitchFlashlight(client, bool:on);

#define IsPlayerFlashlightOn(%0) (GetEntProp(client, Prop_Send, m_fEffects)&FLASHLIGHT_BIT)
