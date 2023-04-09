#if defined _tsubaki_common
    #endinput
#endif
#define _tsubaki_common

#pragma semicolon 1

#include <sourcemod>
#include <sdktools>
#include <cstrike>
#include <sdkhooks>
#include <sdktools_functions>
#include <sdktools_trace>
#include <dhooks>

#include <tsubaki_v3/tsubaki_menu>

/*  Maths  */

    #define M_1OVER2PI 1.57079633
    #define M_PI 3.14153927
    #define M_2PI 6.28318531
    #define M_STRAIGHT_ANGLE 180.0
    #define M_FULL_ANGLE 360.0
    #define ToRadian(%0) (%0 * M_PI / 180.0)
    #define ToDegree(%0) (%0 * 180.0 / M_PI)
    
    #define GetRandomAngle()	(GetRandomFloat(0.0, 360.0))

    stock int ALL_ONE_BS[] = {
        1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047, 4095, 8191, 16383, 32767, 65535, 131071,
        262143, 524287, 1048575, 2097151, 4194303, 8388607, 16777215, 33554431, 67108863, 134217727,		
        268435455, 536870911, 1073741823, 2147483647, -1
    };
    
    #define CopyVector(%0,%1) %0[0]=%1[0]; %0[1]=%1[1]; %0[2]=%1[2]
    stock float FloatClamp(float value, float minimum, float maximum) { return ( value<minimum)?minimum:((value>maximum)?maximum:value); }
    stock int Clamp(int value, int minimum, int maximum) { return (value<minimum)?minimum:((value>maximum)?maximum:value); }

    #define Abs(%0) ((%0<0)?%0*-1:%0)
    
    #define ON true
    #define OFF false 

/*  Colors  */

    #define GetRGBA_A(%0) ((%0>>24) & 0xFF)
    #define GetRGB_R(%0) ((%0&0xFF0000)>>16)
    #define GetRGB_G(%0) ((%0&0xFF00)>>8)
    #define GetRGB_B(%0) (%0&0xFF)
    stock int CreateRGBInt(R, G, B)	{return (R<<16) + (G<<8) + B;}
    stock int CreateRGBAInt(R, G, B, A) {return (A<<24) + (R<<16) + (G<<8) + B;}
    #define CreateRGBAIntByArray(%0) ((%0[3]<<24) + (%0[0]<<16) + (%0[1]<<8) + %0[2])

/*  Display Text  */

    stock char EMPTY[] = "";
    stock char EMPTY_LINE[] = "　";
    stock char EMPTY_ITEM_NAME[] = "---";
    stock char UNKNOWN_ITEM_NAME[] = "???";
    stock char USEABLE[] = "可使用";
    stock char CURRENTLY_USING[] = "使用中";
    stock char IN_COOLDOWN[] = "冷卻中";
    stock char LOCKED[] = "未解鎖";
    stock char NO_REMAIN[] = "已到達使用上限";
    stock char NO_AIM_TARGET[] = "沒有指到任何目標";
    stock char CANCEL[] = "取消";

/*  Flags  */
	stock int GetFlagNumber(char flag, int offset=0)
	{
		if('a' <= flag <= 'z')
			return flag - 'a' + offset;
		else if('A' <= flag <= 'Z')
		    return 26 + flag - 'A' + offset;
		
		return -1;
	}
    
    stock int FlagsToBitsum(char[] flags, int& output, int offset=0)
	{
		output = 0;
		int counter=0, length = strlen(flags), temp;

		for(counter=0; counter<length; counter++)
		{
			temp = offset + flags[counter] - 'a';

			if(temp < 0 || temp >= 32)
				continue;

			output |= (1<<temp);
		}

		return counter;
	}

/*  Arrays */

    stock InsertArrayElementNoRepeat(arr[], max_size, insert, empty_value=0)
    {
        int i=0;
        while(i<max_size && arr[i] != empty_value)
        {
            if(arr[i] == insert)
                return;

            i++;
        }

        if(i != max_size)
            arr[i] = insert;
    }

    stock RemoveArrayElement(arr[], max_size, remove, empty_value=0)
    {
        int pos = 0;
        while(pos<max_size && arr[pos]!=empty_value)
        {
            if(arr[pos] == remove)
            {
                arr[pos] = empty_value;
                break;
            }
            pos++;
        }
        
        while(pos+1<max_size && arr[pos+1]!=empty_value)
        {
            arr[pos] = arr[pos+1];
            arr[pos+1] = empty_value;
            pos++;
        }
    }

/*  Custom Entities  */

    #define MAX_ENTITY 2048
    #define BASE_CUSTOM_ENTITY_COUNTER	100
    #define MAX_CUSTOM_ENTITY	1948

    stock tbk_iuser[MAX_ENTITY][5];
    stock float tbk_fuser[MAX_ENTITY][5];
    stock float tbk_vuser[MAX_ENTITY][5][3];
    stock float tbk_damage[MAX_ENTITY];
    #define GetTsubakiEntityDamage(%0) tbk_damage[%0]
    #define SetTsubakiEntityDamage(%0,%1) tbk_damage[%0]=%1

    int g_iCustomEntityRef[MAX_ENTITY];
    stock Handle g_hEntityRemoveTask[MAX_ENTITY];
    stock Handle g_hEntityThinkTask[MAX_ENTITY];

    //Return -1 if no free space
    stock int GetEmptyCustomEntityIndex(const int& ref=0) {
        static int pos = BASE_CUSTOM_ENTITY_COUNTER, i;

        i=0;
        do {
            pos = (pos+1<MAX_ENTITY)?pos+1:BASE_CUSTOM_ENTITY_COUNTER;

            if(g_iCustomEntityRef[pos] == 0)
                break;

            i++;
        } while(i<MAX_CUSTOM_ENTITY);
        
        if(i == MAX_CUSTOM_ENTITY)
            return -1;
        
        g_iCustomEntityRef[pos] = ref;

        return pos;
    }

    /**
     * 
     * 
     * @return ce_index
     * @return -1 該entity ref 不存在
     */
    stock int EntRefToCustomEntityIndex(const int ref) {
        static int i;
        for(i=BASE_CUSTOM_ENTITY_COUNTER; i<MAX_ENTITY; i++) {
            if(g_iCustomEntityRef[i]==ref) {
                return i;
            }
        }

        return -1;
    }
    
    #define SetEntityOwner(%0,%1)		SetEntPropEnt(%0,Prop_Data,m_hOwnerEntity,%1)
    #define GetEntityOwner(%0)			GetEntPropEnt(%0,Prop_Data,m_hOwnerEntity)

    //Useful Model
    stock String:DEFAULT_LAUNCHER_MODEL[] = "models/blackout.mdl";
    #define PrecahceTsubakiLauncherModel() PrecacheModel(DEFAULT_LAUNCHER_MODEL,false)
    
    stock String:DEFAULT_INVISIBLE_TOUCHER_MODEL[] = "models/pigeon.mdl";
    #define PrecahceTsubakiInvisibleToucher() PrecacheModel(DEFAULT_INVISIBLE_TOUCHER_MODEL,false)

    stock DEFAULT_LASER_SPRITE_ID;
    stock String:DEFAULT_LASER_SPRITE[] = "materials/sprites/laserbeam.vmt";
    #define PrecahceTsubakiLaserSprite() DEFAULT_LASER_SPRITE_ID=PrecacheModel(DEFAULT_LASER_SPRITE);AddFileToDownloadsTable(DEFAULT_LASER_SPRITE)

    stock String:DEFAULT_BULLET_MODEL[] = "models/knastjunkies/soccerball.mdl";
    #define PrecacheTsubakiBulletModel() PrecacheModel(DEFAULT_BULLET_MODEL,false)

    //Useful Entity KV
    stock char CLSNAME_FOR_CUSTOM_BUILD[] = "generic_actor";
    stock char KV_TARGETNAME[] = "targetname";
    stock char KV_CLASSNAME[] = "classname";
    stock char KV_DISABLERECEIVESHADOWS[] = "disablereceiveshadows";
    stock char KV_DISABLESHADOWS[] = "disableshadows";
    stock char KV_MODELSCALE[] = "modelscale";
    stock char KV_SOLID[] = "solid";
    stock char KV_COLLISIONGROUP[] = "collisiongroup";
    stock char KV_MODEL[] = "model";
    stock char KV_RENDERMODE[] = "rendermode";
    stock char KV_RENDERFX[] = "renderfx";
    stock char KV_RENDERCOLOR[] = "rendercolor";

    //Useful Entity Prop
    stock char m_usSolidFlags[] = "m_usSolidFlags";
    stock char m_vecMins[] = "m_vecMins";
    stock char m_vecMaxs[] = "m_vecMaxs";
    stock char m_vecOrigin[] = "m_vecOrigin";
        #define GetEntityOrigin(%0,%1)  GetEntPropVector(%0, Prop_Send, m_vecOrigin, %1)
    stock char m_hOwner[] = "m_hOwner";
    stock char m_hOwnerEntity[] = "m_hOwnerEntity";
    stock char m_ModelName[] = "m_ModelName";
        #define GetEntityModel(%0,%1) GetEntPropString(%0, Prop_Data, m_ModelName, %1, sizeof(%1))
    stock char m_vecNormal[] = "m_vecNormal";
    stock char m_nModelIndex[] = "m_nModelIndex";
    stock char m_fScale[] = "m_fScale";
    stock char m_nFlags[] = "m_nFlags";
    stock char m_nRadius[] = "m_nRadius";
    stock char m_nMagnitude[] = "m_nMagnitude";
    stock char m_chMaterialType[] = "m_chMaterialType";
    stock char m_vecStartPoint[] = "m_vecStartPoint";
    stock char m_vecEndPoint[] = "m_vecEndPoint";
    stock char m_nHaloIndex[] = "m_nHaloIndex";
    stock char m_nStartFrame[] = "m_nStartFrame";
    stock char m_nFrameRate[] = "m_nFrameRate";
    stock char m_fLife[] = "m_fLife";
    stock char m_fWidth[] = "m_fWidth";
    stock char m_fEndWidth[] = "m_fEndWidth";
    stock char m_hObserverTarget[] = "m_hObserverTarget";
    stock char m_iObserverMode[] = "m_iObserverMode";
    stock char m_bDrawViewmodel[] = "m_bDrawViewmodel";
    stock char m_vecVelocity[] = "m_vecVelocity";
    stock char sz_r[] = "r";
    stock char sz_g[] = "g";
    stock char sz_b[] = "b";
    stock char sz_a[] = "a";
    stock char m_nSpeed[] = "m_nSpeed";
    stock char m_nFadeLength[] = "m_nFadeLength";
    stock char WEAPON_HEADER[] = "weapon_";
    stock char M_CLIP1[] = "m_iClip1";
    stock char M_IAMMO[] = "m_iAmmo";
    stock char M_AMMO_TYPE[] = "m_iPrimaryAmmoType";
    stock char M_RESERVE_AMMO[] = "m_iPrimaryReserveAmmoCount";
    stock char M_ACTIVE_WEAPON[] = "m_hActiveWeapon";

    stock char e_UserID[] = "userid";
    stock char e_Attacker[] = "attacker";

    //Getting Entity Prop
    #define GetEntityVelocity(%0,%1)    GetEntPropVector(%0, Prop_Data, m_vecVelocity, %1)

    //Entity SOLID
    
    #define FSOLID_CUSTOMRAYTEST 0x0001 		// Ignore solid type + always call into the entity for ray tests
    #define FSOLID_CUSTOMBOXTEST 0x0002 		// Ignore solid type + always call into the entity for swept box tests
    #define FSOLID_NOT_SOLID 0x0004 			// Are we currently not solid?
    #define FSOLID_TRIGGER 0x0008 				// This is something may be collideable but fires touch functions even when it's not collideable (when the FSOLID_NOT_SOLID flag is set)
    #define FSOLID_NOT_STANDABLE 0x0010 		// You can't stand on this
    #define FSOLID_VOLUME_CONTENTS 0x0020 		// Contains volumetric contents (like water)
    #define FSOLID_FORCE_WORLD_ALIGNED 0x0040 	// Forces the collision rep to be world-aligned even if it's SOLID_BSP or SOLID_VPHYSICS
    #define FSOLID_USE_TRIGGER_BOUNDS 0x0080 	// Uses a special trigger bounds separate from the normal OBB
    #define FSOLID_ROOT_PARENT_ALIGNED 0x0100 	// Collisions are defined in root parent's local coordinate space
    #define FSOLID_TRIGGER_TOUCH_DEBRIS 0x0200 // This trigger will touch debris objects 

    /*
    *
    * 實體生成
    *
    * @param clsname        實體classname
    * @param model_route    模組
    * @param solid_type     SOLID_*
    * @param movetype       MOVETYPE_*
    * @param gravity        重力[1.0 為基本]
    * @param scale          實體大小及模組大小比
    * @param owner          主人
    * @param r              Render 紅(0~255)
    * @param g              Render 綠(0~255)
    * @param b              Render 藍(0~255)
    * @param a              Render 強度(0~255)
    * @param render         kRender*
    * @param render_fx      kRenderFx*
    * @param min_size       hit_box negative size
    * @param min_size       hit_box positive size   
    *
    * @return -1：生成失敗
    * @return 其他：生成的實體CE Index
    */
    stock int CreateTsubakiEntity(const String:clsname[], const String:model_route[], const int solid_type=(0x8|0x4), const MoveType movetype=MOVETYPE_NOCLIP, const float gravity=0.0, const float model_scale=0.6, const int owner=0, const int r=255, const int g=255, const int b=255, const int a = 255, const RenderMode rendermode=RENDER_TRANSALPHA, const RenderFx renderfx=RENDERFX_NONE, const float min_size[3]={-16.0,-16.0,-16.0}, const float max_size[3]={16.0,16.0,16.0}) {

        static char input_color[32];
        static char color_fmt[] = "%d %d %d %d";

        input_color[0]=0;
        FormatEx(input_color, 32, color_fmt, r, g, b, a);

        int ent = CreateEntityByName(CLSNAME_FOR_CUSTOM_BUILD);
        if(ent == -1) {
            return -1;
        }

        DispatchKeyValue(ent, KV_TARGETNAME, clsname);
        DispatchKeyValue(ent, KV_CLASSNAME, clsname);
        DispatchKeyValue(ent, KV_MODEL, model_route);
        DispatchKeyValueInt(ent, KV_DISABLERECEIVESHADOWS, 1);
        DispatchKeyValueInt(ent, KV_DISABLESHADOWS, 1);
        DispatchKeyValueFloat(ent, KV_MODELSCALE, model_scale);
        DispatchKeyValueInt(ent, KV_SOLID, 5);
        DispatchKeyValueInt(ent, KV_COLLISIONGROUP, 2);
        DispatchKeyValueInt(ent, KV_RENDERMODE, view_as<int>(rendermode));
        DispatchKeyValueInt(ent, KV_RENDERFX, view_as<int>(renderfx));
        DispatchKeyValue(ent, KV_RENDERCOLOR, input_color);
        DispatchSpawn(ent); 
        ActivateEntity(ent);
        
        SetEntProp(ent, Prop_Send, m_usSolidFlags, solid_type);
        SetEntityMoveType(ent, movetype);

        SetEntPropVector(ent, Prop_Send, m_vecMins, min_size);
        SetEntPropVector(ent, Prop_Send, m_vecMaxs, max_size);

        SetEntityOwner(ent, owner);
        ent = EntIndexToEntRef(ent);

        ent = GetEmptyCustomEntityIndex(ent);

        return ent;     //Custom Entity Index
    }
    
    /*
    * 生成隱形的實體【建議只用在隱形的發射器，否則建議使用CreateEntity】
    * 如果無論如何都想其顯示，請在外面叫  set_pev(ent, pev(ent, pev_effects) & ~EF_NODRAW);
    *
    * @param clsname        實體classname
    * @param model_route    模組(使用跟子實體同樣的模組)
    * @param movetype       MOVETYPE_*
    * @param owner          主人
    *
    */
    stock CreateInvisibleLauncher(const char[] clsname, const char[] model_route, MoveType movetype=MOVETYPE_NOCLIP, owner=0, bool:get_reference=false) {
        
        int ent = CreateEntityByName(CLSNAME_FOR_CUSTOM_BUILD);
        if(ent == -1) {
            return -1;
        }

        DispatchKeyValue(ent, KV_TARGETNAME, clsname);
        DispatchKeyValue(ent, KV_CLASSNAME, clsname);
        DispatchKeyValue(ent, KV_MODEL, model_route);
        DispatchKeyValueInt(ent, KV_DISABLERECEIVESHADOWS, 1);
        DispatchKeyValueInt(ent, KV_DISABLESHADOWS, 1);
        DispatchKeyValueFloat(ent, KV_MODELSCALE, 1.0);
        DispatchKeyValueInt(ent, KV_SOLID, 0);
        DispatchKeyValueInt(ent, KV_COLLISIONGROUP, 0);
        DispatchKeyValueInt(ent, KV_RENDERMODE, view_as<int>(RENDER_NONE));
        DispatchKeyValueInt(ent, KV_RENDERFX, view_as<int>(RENDERFX_NONE));
        DispatchSpawn(ent); 
        //ActivateEntity(ent);
        
        SetEntProp(ent, Prop_Send, m_usSolidFlags, FSOLID_NOT_SOLID);
        SetEntityMoveType(ent, movetype);       

        SetEntPropVector(ent, Prop_Send, m_vecMins, {-1.0, -1.0, -1.0});
        SetEntPropVector(ent, Prop_Send, m_vecMaxs, { 1.0,  1.0,  1.0});

        SetEntityOwner(ent, owner);
        
        ent = EntIndexToEntRef(ent);        //ref

        if(get_reference)
            return ent;
        
        ent = GetEmptyCustomEntityIndex(ent);        //ce_index

        return ent;
    }

    /*
    * 刪除實體【只要是TSUBAKI的實體都建議用這function 移除】
    *
    * 製作task時請以右邊方式，15秒後刪除id為{ent}的實體：SetRemoveEntityTask(ent, 15.0) 
    * 或者 參考右邊寫法  g_hEntityRemoveTask[ce_index] = CreateTimer(15.0, RemoveTsubakiEntity, ce_index, TIMER_FLAG_NO_MAPCHANGE);
    *
    * @param clsname        實體classname
    * @param model_route    模組
    * @param movetype       MOVETYPE_*
    * @param owner          主人
    *
    */
    public Action RemoveTsubakiEntity(Handle timer, int ce_index) {
        static ref;
        ref = g_iCustomEntityRef[ce_index];
    
        if(IsValidEntity(ref)) {
            RemoveEntity(ref);
        }

        if(g_hEntityRemoveTask[ce_index] != INVALID_HANDLE) {
            KillTimer(g_hEntityRemoveTask[ce_index]);
            g_hEntityRemoveTask[ce_index] = INVALID_HANDLE;
        }

        if(g_hEntityThinkTask[ce_index] != INVALID_HANDLE) {
            KillTimer(g_hEntityThinkTask[ce_index]);
            g_hEntityThinkTask[ce_index] = INVALID_HANDLE;
        }

        g_iCustomEntityRef[ce_index] = 0;

        return Plugin_Stop;
    }

    #define SetRemoveEntityTask(%0,%1) g_hEntityRemoveTask[%0] = CreateTimer(%1, RemoveTsubakiEntity, %0, TIMER_FLAG_NO_MAPCHANGE)
    /**
     * @param %0 ce_index
     * @param %1 time
     * @param %2 Function
     * @param %3 Timer Flag
     */
    #define SetEntityNextThink(%0,%1,%2,%3) g_hEntityThinkTask[%0] = CreateTimer(%1, %2, %0, %3)
    #define ResetEntityThinkHandle(%0) g_hEntityThinkTask[%0] = INVALID_HANDLE

/*  Filtering  */

    //Hit if players and not owner
    public bool:FilterEntityWithoutPlayer(entity, contentMask, any:data)
    {
        //return entity != data;
        return (entity <= MaxClients && entity != data && entity >= 1);
    }

    //Hit if not owner
    public bool:FilterAllEntity(entity, contentMask, any:data)
    {
        return (entity != data && 1 <= entity <= MaxClients);
    }

/*  Temp Entity  */

    stock void TBKLaserTE(int sprite_id, const float start[3], const float end[3], 
        float life, float width, float end_width, 
        int r, int g, int b, int a, int speed=0)
    {
        static char BeamPoints[] = "BeamPoints";
        
        TE_Start(BeamPoints);
        TE_WriteVector(m_vecStartPoint, start);
        TE_WriteVector(m_vecEndPoint, end);
        TE_WriteNum(m_nModelIndex, sprite_id);
        TE_WriteNum(m_nHaloIndex, 0);
        TE_WriteNum(m_nStartFrame, 0);
        TE_WriteNum(m_nFrameRate, 0);
        TE_WriteFloat(m_fLife, life);
        TE_WriteFloat(m_fWidth, width);
        TE_WriteFloat(m_fEndWidth, end_width);
        TE_WriteNum(sz_r, r);
        TE_WriteNum(sz_g, g);
        TE_WriteNum(sz_b, b);
        TE_WriteNum(sz_a, a);
        TE_WriteNum(m_nSpeed, speed);
        TE_WriteNum(m_nFadeLength, 10);
        TE_SendToAll();
    }
    
    stock void TBKSquareLaserTE(sprite_id, const Float:center[3], const Float:length=400.0,
        Float:life=10.0, Float:width=5.0, 
        r=255, g=0, b=255, a=255, speed=0)
    {
        static String:BeamPoints[] = "BeamPoints", i, Float:origin[4][3];
        
        for(i=0; i<4; i++) {
            origin[i][0] = center[0] + ((i%3)?-1:1) * length/2.0;
            origin[i][1] = center[1] + ((i>=2)?-1:1) * length/2.0;
            origin[i][2] = center[2];
        }

        for(i=0; i<4; i++) {
            TE_Start(BeamPoints);
            TE_WriteVector(m_vecStartPoint, origin[i]);
            TE_WriteVector(m_vecEndPoint, origin[(i+1)%4]);
            TE_WriteNum(m_nModelIndex, sprite_id);
            TE_WriteNum(m_nHaloIndex, 0);
            TE_WriteNum(m_nStartFrame, 0);
            TE_WriteNum(m_nFrameRate, 0);
            TE_WriteFloat(m_fLife, life);
            TE_WriteFloat(m_fWidth, width);
            TE_WriteFloat(m_fEndWidth, width);
            TE_WriteNum(sz_r, r);
            TE_WriteNum(sz_g, g);
            TE_WriteNum(sz_b, b);
            TE_WriteNum(sz_a, a);
            TE_WriteNum(m_nSpeed, speed);
            TE_WriteNum(m_nFadeLength, 10);
            TE_SendToAll(0.01*i);

        }
    }

    stock void TBKCreateExplosionTE(const float pos[3], int Model, float Scale=1.0, int framerate=30, int flags=TE_EXPLFLAG_NOPARTICLES|TE_EXPLFLAG_ROTATE|TE_EXPLFLAG_NOFIREBALLSMOKE|TE_EXPLFLAG_NOSOUND, int radius=10, int magnitude=1, const float normal[3]={0.0,0.0,1.0}, int materialtype='C')
    {
        static char Explosion[] = "Explosion", 
            m_vecOrigin0[] = "m_vecOrigin[0]";

        TE_Start(Explosion);
        TE_WriteVector(m_vecOrigin0, pos);
        TE_WriteVector(m_vecNormal, normal);
        TE_WriteNum(m_nModelIndex, Model);
        TE_WriteFloat(m_fScale, Scale);
        TE_WriteNum(m_nFrameRate, framerate);
        TE_WriteNum(m_nFlags, flags);
        TE_WriteNum(m_nRadius, radius);
        TE_WriteNum(m_nMagnitude, magnitude);
        TE_WriteNum(m_chMaterialType, materialtype);
        TE_SendToAll();
    }
/*  Player  */

    #define IsClientValid(%0) (1<=%0<=MaxClients && IsClientInGame(%0))
    #define GetClientCenterOrigin(%0,%1) GetClientAbsOrigin(%0,%1); %1[2]+=40.0
    #define GetClientAimAngles(%0,%1) GetClientEyeAngles(%0,%1); %1[0]*=-1.0

    stock bool GetPlayerAimOrigin(int client, float vec[3])
    {
        if(!IsClientInGame(client))
            return false;

        float angle[3], origin[3];
        GetClientEyeAngles(client, angle);
        GetClientEyePosition(client, origin);

        //If the TraceEntityFilter is true, stop
        TR_TraceRayFilter(origin, angle, MASK_SOLID, RayType_Infinite, FilterEntityWithoutPlayer, client);
        if(TR_DidHit(INVALID_HANDLE))
            TR_GetEndPosition(vec, INVALID_HANDLE);
            
        return true;
    }
    

    /**
     * 獲取玩家指向/觀察時目標
     */
    stock int GetPlayerAimingTarget(int client)
	{
		if(!IsPlayerAlive(client))
			return GetEntPropEnt(client, Prop_Send, m_hObserverTarget);
		else
			return GetClientAimTarget(client);
	}
    
	stock StripPlayerWeapon(int client, int remove_slot=-1)
	{
		if(IsClientValid(client) && IsPlayerAlive(client))
		{
			int entity_id;
			if(remove_slot == -1)		//strip all
			{
				for(int slot=0; slot<5; slot++)
				{
					while( (entity_id=GetPlayerWeaponSlot(client, slot)) != -1)
						RemovePlayerItem(client, entity_id);
				}
			}
			else
			{
				while( (entity_id=GetPlayerWeaponSlot(client, remove_slot)) != -1 )
					RemovePlayerItem(client, entity_id);
			}
		}
	}

#pragma semicolon 0