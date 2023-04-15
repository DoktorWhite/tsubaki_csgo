#if defined _tsubaki_camera
    #endinput
#endif
#define _tsubaki_camera
#define TSUBAKI_CAMERA_INCLUDED

#pragma semicolon 1

/*  Camera Variable  */
#define TSUBAKI_CAMERA "tsubaki_camera"
#define TSUBAKI_CAMERA_CONTROL_COMMAND "tsubaki_camera_control"


#define TOTAL_CAMERA_MODE   6
    #define CAMERA_1ST              0
    #define CAMERA_3RD              1
    #define CAMERA_3RD_CLOSE        2
    #define CAMERA_3RD_FAR          3
    #define CAMERA_FIX              4
    #define CAMERA_TOP_DOWN         5
    //#define CAMERA_3RD_SUPER_FAR    6

    #define ALLOW_1ST               (1<<CAMERA_1ST)
    #define ALLOW_3RD               (1<<CAMERA_3RD)
    #define ALLOW_3RD_CLOSE         (1<<CAMERA_3RD_CLOSE)
    #define ALLOW_3RD_FAR           (1<<CAMERA_3RD_FAR)
    #define ALLOW_FIX               (1<<CAMERA_FIX)
    #define ALLOW_TOP_DOWN          (1<<CAMERA_TOP_DOWN)
    //#define ALLOW_3RD_SUPER_FAR     (1<<CAMERA_3RD_SUPER_FAR)

    stock String:NAME_OF_CAMERA[TOTAL_CAMERA_MODE][] = {
        "第一人稱"
        , "第三人稱"
        , "第三人稱（近）"
        , "第三人稱（遠）"
        , "固定視角"
        , "天花板視角"
        //, "第三人稱（超遠）"
    };

    stock Float:CAMERA_DISTANCE[TOTAL_CAMERA_MODE] = {
        0.0
        ,125.0
        ,50.0
        ,250.0      //3RD FAR
        ,0.0
        ,500.0      //TOP_DOWN
        //,500.0
    };

    #define TOTAL_CAMERA_PROP   2
        #define CAMERA_PROP_OBSERVER_TARGET 0
        #define CAMERA_PROP_OBSERVERMODE    1
    stock int CAMERA_PROP[TOTAL_CAMERA_MODE][TOTAL_CAMERA_PROP] = {
        {-1, 0}
        ,{-1, 0}
        ,{-1, 0}
        ,{-1, 0}
        ,{-1, 1}
        ,{-1, 1}        //TOP DOWN
        //,{-1, 0}        //SUPER_FAR
    };

/**
 * 顯示玩家視角清單
 * See tsubaki_camera.sp -> Native_PlayerCallCameraPanel
 */
native void DisplayCameraPanel(int client);

/**
 * 改變玩家視角
 * See tsubaki_camera.sp -> Native_ChangePlayerCamera
 * 
 * @return 1 : 成功改變
 * @return -1 : 無效視角
 * @return -2 : 指令 tsubaki_camera_control 的值不允許對應視角
 */
native int ChangePlayerCamera(int client, int camera_mode);

#pragma semicolon 0