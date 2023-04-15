#if defined _tsubaki_menu
    #endinput
#endif
#define _tsubaki_menu

#include <sourcemod>

/*  Menu Variables  */
    
	stock String:MENU_SOUND[] = "buttons/button14.wav";
	#define EMIT_MENU_SOUND(%0) EmitSoundToClient(%0, MENU_SOUND, %0, SNDCHAN_ITEM, SNDLEVEL_NONE, SND_NOFLAGS, 1.0, SNDPITCH_NORMAL, client)
	#define MENU_KEYS_ALL	(1<<0|1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8|1<<9)

	stock int giPlyCurMenuId[MAXPLAYERS];
	stock int giPlyCurMenuPage[MAXPLAYERS];
	stock int giPlyCurMenuTarget[MAXPLAYERS];
	stock int giPlyCurMenuItem[MAXPLAYERS];

	#define MAX_MENU_ITEM		6
    #define MAX_MENU_ITEM_WITHOUT_PAGE  8
	#define PLY_MENU_ID(%0)		giPlyCurMenuId[%0]
	#define PLY_MENU_PAGE(%0)	giPlyCurMenuPage[%0]
	#define PLY_MENU_TARGET(%0) giPlyCurMenuTarget[%0]
	#define PLY_MENU_ITEM(%0)   giPlyCurMenuItem[%0]

    stock String:PREVIOUS_MENU[] = "返回";
    stock String:PREVIOUS_PAGE[] = "上一頁";
    stock String:NEXT_PAGE[] = "下一頁";
    #define PanelHasPreviousPage(%0) giPlyCurMenuPage[%0]>0
    #define PanelHasNextPage(%0,%1) ((giPlyCurMenuPage[%0]+1)*MAX_MENU_ITEM)<(%1)
    #define GetPanelItemPos(%0,%1) (giPlyCurMenuPage[%0]*MAX_MENU_ITEM+%1)

/*  Menu ID  Management  */
    //Please Add MenuId to here too for management
	// #define MENUID_BACKPACK_USE		8
	// #define MENUID_BACKPACK_THROW	9
	// #define MENUID_BP_TO_WH			12
	// #define MENUID_WH_TO_BP			14
	// #define MENUID_BP_TRADE			18
	// #define MENUID_WH_SELL			19
	// #define MENUID_EQUIP		16
    // #define MENUID_EQUIP_CHOOSE	17
    // #define MENUID_WEAPON_PANEL  21
	// #define MENUID_MISSION		31
	// #define MENUID_TUTORIAL		33
	// #define MENUID_MODEL_DISPLAY		61
	// #define MENUID_ARM_DISPLAY			62
	// #define MENUID_SOUND_DISPLAY		63
	// #define MENUID_CRAFT_MENU_IN_CM		102
	// #define MENUID_CRAFT_MENU			103
    // #define MENUID_ITEM_DICT_ADMIN       111
    // #define MENUID_ITEM_DICT_DISPLAY_ALL 112
    // #define MENUID_ITEM_DICT             113
    // #define MENUID_ABNORMAL_DICT_ADMIN   121

	//DONT use 200~299 if not citymall shop
	//#define MENUID_SHOP_BASE		200

/*  Menu  */
    stock void AddPanelPageController(Panel& panel, client, total_item, int offset=0) {
        panel.CurrentKey = 7;
        if(PanelHasPreviousPage(client))
            panel.DrawItem(PREVIOUS_PAGE);

        panel.CurrentKey = 8;
        if(PanelHasNextPage(client,total_item-offset))
            panel.DrawItem(NEXT_PAGE);
    }

    #define DrawCancelToPanelWithNewLine(%0) %0.DrawText(EMPTY_LINE); %0.CurrentKey=9; %0.DrawItem(CANCEL)

    #define ResetPlayerMenu(%0) PLY_MENU_ID(%0)=PLY_MENU_PAGE(%0)=PLY_MENU_TARGET(%0)=PLY_MENU_ITEM(%0)=0;CancelClientMenu(%0)


    stock void CreateAndDisplayPanel(String:title[], String:menu_item[][], start_pos, total_item, MenuHandler handler, target=0, bool:single_page_only=false, menu_id=0, menu_time=MENU_TIME_FOREVER) {
        new i, max_item=single_page_only?MAX_MENU_ITEM_WITHOUT_PAGE:(MAX_MENU_ITEM_WITHOUT_PAGE-2);
        Panel panel = new Panel();

        panel.SetTitle(title);

        for(i=0; i+start_pos<total_item && i<max_item; i++) {
            panel.DrawItem(menu_item[i+start_pos]);
        }

        if(target==0) {
            for(i=1; i<=MaxClients; i++) {
                panel.Send(i, handler, menu_time);
            }

        } else {
            panel.Send(target, handler, menu_time);
        }
        
        delete panel;
        return;
    }

    stock Panel GetPanelForDisplayOnly(char[] title, bool cancel_only=true, int keys=MENU_KEYS_ALL)
    {
        Panel panel = new Panel();
        panel.SetTitle(title);
        //Key that can be pressed (Without 0)
        panel.SetKeys(keys);
        //Number for DrawItem Below
        if(cancel_only)
            panel.CurrentKey = 9;

        return panel;
    }

    stock int DummyMenuHandler(Menu menu, MenuAction action, int client, int key)
    {
        #if defined DEBUG
        if(action == MenuAction_Select)
        {
            //EMIT_MENU_SOUND(client);
        }
        #endif


        return 0;
    }