#if defined _tsubaki_bullet
    #endinput
#endif
#define _tsubaki_bullet

#include <tsubaki_v3/tsubaki_common>

#pragma semicolon 1

/*  Bullet Properties  */
    #define BULLET_DMG_TYPE DMG_GENERIC
    #define DEFAULT_BULLET_DMG          8.0
    #define DEFAULT_BULLET_THINKTIME    0.1
    #define DEFAULT_BULLET_SOLID        FSOLID_NOT_SOLID|FSOLID_TRIGGER

    stock float BULLET_MIN_SIZE[3] = {-16.0,-16.0,-16.0};
    stock float BULLET_MAX_SIZE[3] = { 16.0, 16.0, 16.0};

    stock char EXPLOSIVE_BULLET_SOUND[258];

/*  Bullet Classes  */

    
    stock char NORMAL_BULLET[] = "Normal_Bullet";
    stock char AVERAGE_CHANGE_SPEED_BULLET[] = "Average_Change_Speed_Bullet";
    stock char MOVE_AND_SPIN_ONLY_BULLET[] = "Move_And_Spin_Only_Bullet";
    stock char SPIN_AWAY_BULLET[] = "Spin_Away_Bullet";
    stock char SPIN_AWAY_SPLIT_BULLET[] = "Spin_Away_Split_Bullet";
    stock char RANDOM_MACHINE_BULLET_LAUNCHER[] = "Random_Machine_Bullet_Launcher";
    stock char ROUND_SPIN_BULLET[] = "Round_Spin_Bullet";
    stock char CHANGE_DEST_BULLET[] = "Change_Dest_Bullet";
    stock char CHANGE_ANGLE_BULLET[] = "Change_Angle_Bullet";
    stock char ROTATE_MACHINE_BULLET_LAUNCHER[] = "Rotate_Machine_Bullet_Launcher";
    stock char SPLIT_STRAIGHT_BULLET[] = "Split_Straight_Bullet";
    stock char APPEAR_AND_DISAPPER_BULLET[] = "Appear_And_Disapper_Bullet";
    stock char NON_PENETRATE_BULLET[] = "Non_Penetrate_Bullet";
    stock char SPLIT_BACKWARD_BULLET[] = "Split_Backward_Bullet";
    stock char SLOW_CHANGE_DIR_BULLET_LAUNCHER[] = "Slow_Change_Dir_Bullet_Launcher";
    stock char SLOW_CHANGE_DIR_BULLET[] = "Slow_Change_Dir_Bullet";
    stock char SWIRL_BULLET[] = "Swirl_Bullet";
    stock char TRACER_BULLET[] = "Tracer_Bullet";
    stock char SNAKE_PROTECT_BULLET[] = "Snake_Protect_Bullet";
    stock char SNAKE_PROTECT_CONTROLLER[] = "Snake_Protect_Controller";

    stock char EXPLOSIVE_BULLET[] = "Explosive_Bullet";
    #define EXPLOSIVE_BULLET_THINKTIME 0.25

/*  Touch Functions  */

    public NormalBulletTouch(int ent, int client) {
        static owner, ce_index;

        if( 1<=client<=MaxClients && ((owner=GetEntityOwner(ent)) != client) && (owner==0 || GetClientTeam(owner)!=GetClientTeam(client)) ) {
            ce_index = EntRefToCustomEntityIndex(EntIndexToEntRef(ent));

            if(ce_index == -1) {
                RemoveEntity(ent);
                return;
            }
            
            SDKHooks_TakeDamage(client, ent, owner, GetTsubakiEntityDamage(ce_index), BULLET_DMG_TYPE, .bypassHooks=false);
            RemoveTsubakiEntity(INVALID_HANDLE, ce_index);
        }
    }
    
    public NonPenetrateBulletTouch(int ent, int client) {
        static owner, ce_index;
        
        if((owner=GetEntityOwner(ent))==client)
            return;
            
        ce_index = EntRefToCustomEntityIndex(EntIndexToEntRef(ent));
        if(ce_index == -1) {
            RemoveEntity(ent);
            return;
        }

        if((1<=client<=MaxClients) && (owner==0 || GetClientTeam(client)!=GetClientTeam(owner))) {
            SDKHooks_TakeDamage(client, ent, owner, GetTsubakiEntityDamage(ent), DMG_CLUB, .bypassHooks=false);
        }
        
        RemoveTsubakiEntity(INVALID_HANDLE, ce_index);
    }


/*  Non-Penetrate Bullet  */

  
    /*
    * 發射一粒不穿透的的彈幕
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param angles 運動角度 (Pitch, Yaw, Roll) (degree)
    * @param owner 主人
    * @param speed 彈幕速度
    * @param last_distance 移動距離
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    */  
    stock FireNonPenetrateBullet(const char[] bullet_model, float origin[3], float angles[3], owner, float speed=200.0, float last_distance=0.0, 
                                            colors[4]={200,200,0,255}, float damage=DEFAULT_BULLET_DMG)
    {        
        float velocity[3];
        int ce_index = CreateTsubakiEntity(NON_PENETRATE_BULLET
                                        ,bullet_model
                                        ,.solid_type=DEFAULT_BULLET_SOLID
                                        ,.movetype=MOVETYPE_FLY
                                        ,.owner=owner 
                                        ,.r=colors[0] 
                                        ,.g=colors[1] 
                                        ,.b=colors[2]
                                        ,.a=colors[3]
                                        ,.min_size=BULLET_MIN_SIZE
                                        ,.max_size=BULLET_MAX_SIZE);

        if(ce_index == -1)
            return;

        angles[0] = ToRadian(angles[0]);
        angles[1] = ToRadian(angles[1]);

        velocity[0] = speed * Cosine(angles[0]) * Cosine(angles[1]);
        velocity[1] = speed * Cosine(angles[0]) *   Sine(angles[1]);
        velocity[2] = speed *   Sine(angles[0]);

        TeleportEntity(g_iCustomEntityRef[ce_index], origin, NULL_VECTOR, velocity);
        SetTsubakiEntityDamage(ce_index, damage);
        SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NonPenetrateBulletTouch);
        

        SetRemoveEntityTask(ce_index, (last_distance>0.0)?last_distance/speed:20.0);
    }



/*  Average Bullet  */

    /*
    * 以平均角度生成以{speed}移動的彈幕
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param bullet_amount 生成數
    * @param owner 主人
    * @param speed 彈幕速度
    * @param last_time 存在時間
    * @param angle_offset 生成角度偏差(degree)
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    */  
    stock AverageBullet(const char[] bullet_model, float origin[3], bullet_amount=8, owner=0, 
        float speed=100.0, float last_time=4.0, float angle_offset=0.0, colors[4]={200, 0, 0, 255}, float damage=DEFAULT_BULLET_DMG)
    {
        int ce_index, i;
        float velocity[3];
        
        angle_offset = ToRadian(angle_offset);

        for(i=0; i<bullet_amount; i++)
        {
            ce_index = CreateTsubakiEntity(NORMAL_BULLET
                                        ,bullet_model
                                        ,.solid_type=DEFAULT_BULLET_SOLID
                                        ,.movetype=MOVETYPE_NOCLIP
                                        ,.owner=owner 
                                        ,.r=colors[0] 
                                        ,.g=colors[1] 
                                        ,.b=colors[2]
                                        ,.a=colors[3]
                                        ,.min_size=BULLET_MIN_SIZE
                                        ,.max_size=BULLET_MAX_SIZE);

            if(ce_index == -1)
                continue;
            
            velocity[0] = speed * Cosine(angle_offset);
            velocity[1] = speed *   Sine(angle_offset);

            TeleportEntity(g_iCustomEntityRef[ce_index], origin, NULL_VECTOR, velocity);
            SetTsubakiEntityDamage(ce_index, damage);

            SetRemoveEntityTask(ce_index, last_time);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);

            angle_offset += M_2PI / bullet_amount;
        }
    }


/*  Avarage Change Speed Bullet  */

    /*
    * 生成時以{spd1} 移動，{delay}秒後以{spd2}移動
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 生成數
    * @param spd1 起始速度
    * @param spd2 終止速度
    * @param angle_offset 生成角度偏差(degree)
    * @param delay N秒後速度改為 spd2
    * @param last_time 存在時間
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    */
    stock AverageChangeSpeedBullet(const char[] bullet_model, Float:origin[3], owner=0, bullet_amount=8,
                                        Float:spd1=50.0, Float:spd2=300.0, Float:angle_offset=0.0,
                                        Float:delay=2.0, Float:last_time=7.0,
                                        colors[4]={0,0,255,255}, Float:damage=DEFAULT_BULLET_DMG)
    {
        new ce_index, i=0;
        new Float:velocity[3] = {0.0, ...};
        angle_offset = ToRadian(angle_offset);

        for(; i<bullet_amount; i++)
        {
            ce_index = CreateTsubakiEntity(AVERAGE_CHANGE_SPEED_BULLET
                                        ,bullet_model
                                        ,.solid_type=DEFAULT_BULLET_SOLID
                                        ,.movetype=MOVETYPE_NOCLIP
                                        ,.owner=owner
                                        ,.r=colors[0]
                                        ,.g=colors[1]
                                        ,.b=colors[2]
                                        ,.a=colors[3]
                                        ,.min_size=BULLET_MIN_SIZE
                                        ,.max_size=BULLET_MAX_SIZE);

            if(ce_index == -1)
                continue;

            velocity[0] = spd1 * Cosine(angle_offset);
            velocity[1] = spd1 *   Sine(angle_offset);

            tbk_vuser[ce_index][0][0] = spd2 * Cosine(angle_offset);
            tbk_vuser[ce_index][0][1] = spd2 *   Sine(angle_offset);
            tbk_vuser[ce_index][0][2] = 0.0;

            TeleportEntity(g_iCustomEntityRef[ce_index], origin, NULL_VECTOR, velocity);
            SetTsubakiEntityDamage(ce_index, damage);

            SetEntityNextThink(ce_index, delay, AverageChangeSpeedBulletThink, TIMER_FLAG_NO_MAPCHANGE);
            SetRemoveEntityTask(ce_index, last_time);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);

            angle_offset += M_2PI/bullet_amount;
        }
    }								

        public Action AverageChangeSpeedBulletThink(Handle timer, ce_index) {
            TeleportEntity(g_iCustomEntityRef[ce_index], NULL_VECTOR, NULL_VECTOR, tbk_vuser[ce_index][0]);

            ResetEntityThinkHandle(ce_index);
            return Plugin_Stop;
        }

/*  Move And Spin Only Bullet */
   
    /*
    * 生成後在{time_to_radius}秒後移動至{target_radius}距離，再以{angular_spd} 旋轉
    * 彈幕消失時間為{time_to_radius}+{last_time}
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 生成數
    * @param target_radius 半徑
    * @param angle_offset 生成角度偏差(degree)
    * @param angular_spd 每秒角速度(degree)
    * @param time_to_radius 抵達半徑所需時間
    * @param last_time 存在時間
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    */
    stock MoveAndSpinOnlyBullet(const char[] bullet_model, Float:origin[3], owner=0, bullet_amount=8,
                                    Float:target_radius=100.0, Float:angle_offset=0.0, Float:angular_spd=45.0,
                                    Float:time_to_radius=1.0, Float:last_time=8.0, 
                                    colors[4]={30,144,255,255}, Float:damage=DEFAULT_BULLET_DMG )
    {
        new i=0, ce_index;
        target_radius /= time_to_radius;		//target_radius become spd
        angle_offset = ToRadian(angle_offset);
        angular_spd = ToRadian(angular_spd);

        for(; i<bullet_amount; i++)
        {
            ce_index=CreateTsubakiEntity(MOVE_AND_SPIN_ONLY_BULLET
                                    ,bullet_model
                                    ,.solid_type=DEFAULT_BULLET_SOLID
                                    ,.movetype=MOVETYPE_NOCLIP
                                    ,.owner=owner
                                    ,.r=colors[0]
                                    ,.g=colors[1]
                                    ,.b=colors[2]
                                    ,.a=colors[3]
                                    ,.min_size=BULLET_MIN_SIZE
                                    ,.max_size=BULLET_MAX_SIZE);
            
            if(ce_index==-1)
                continue;

            tbk_vuser[ce_index][0][0] = target_radius * Cosine(angle_offset);
            tbk_vuser[ce_index][0][1] = target_radius *   Sine(angle_offset);
            tbk_vuser[ce_index][0][2] = 0.0;

            tbk_fuser[ce_index][0] = angle_offset + M_PI/2.0;		//current_angle
            tbk_fuser[ce_index][1] = angular_spd;			//angular_spd
            tbk_fuser[ce_index][2] = target_radius;		//spd

            TeleportEntity(g_iCustomEntityRef[ce_index], origin, NULL_VECTOR, tbk_vuser[ce_index][0]);
            SetTsubakiEntityDamage(ce_index, damage);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);

            SetEntityNextThink(ce_index, time_to_radius-DEFAULT_BULLET_THINKTIME, MoveAndSpinOnlyBulletPrepare, TIMER_FLAG_NO_MAPCHANGE);
            SetRemoveEntityTask(ce_index, last_time);

            angle_offset += M_2PI / bullet_amount;
        }
    }

        public Action MoveAndSpinOnlyBulletPrepare(Handle timer, int ce_index)
        {
            SetEntityNextThink(ce_index, DEFAULT_BULLET_THINKTIME, MoveAndSpinOnlyBulletThink, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);

            return Plugin_Stop;
        }

        public Action MoveAndSpinOnlyBulletThink(Handle timer, int ce_index)
        {
            tbk_vuser[ce_index][0][0] = tbk_fuser[ce_index][2] * tbk_fuser[ce_index][1] * Cosine(tbk_fuser[ce_index][0]);
            tbk_vuser[ce_index][0][1] = tbk_fuser[ce_index][2] * tbk_fuser[ce_index][1] *   Sine(tbk_fuser[ce_index][0]);

            tbk_fuser[ce_index][0] += tbk_fuser[ce_index][1] * DEFAULT_BULLET_THINKTIME;
            TeleportEntity(g_iCustomEntityRef[ce_index], NULL_VECTOR, NULL_VECTOR, tbk_vuser[ce_index][0]);

            return Plugin_Continue;
        }


/* Spin Away Bullet */
   
    /*
    * 生成後以 {angular_spd}旋轉，並以每秒{radius_increase_per_second}單位距離遠離中心點
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 生成數
    * @param initial_radius 生成時與中心點半徑距離
    * @param radius_increase_per_second 每秒遠離中心點的單位距離
    * @param angle_offset 生成角度偏差(degree)
    * @param angular_spd 每秒旋轉角速度(degree)
    * @param last_time 存在時間
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    *
    */
    stock SpinAwayBullet(const char[] bullet_model, Float:origin[3], owner=0, bullet_amount=8, 
                            Float:initial_radius=50.0, Float:radius_increase_per_second=50.0,
                            Float:angle_offset=0.0, Float:angular_spd=30.0, Float:last_time=8.0,
                            colors[4]={64,224,208,255}, Float:damage=DEFAULT_BULLET_DMG)
    {
        new i=0, ce_index;
        float velocity[3];

        angle_offset = ToRadian(angle_offset);
        angular_spd = ToRadian(angular_spd);
        radius_increase_per_second *= DEFAULT_BULLET_THINKTIME;

        for(; i<bullet_amount; i++)
        {
            ce_index=CreateTsubakiEntity(SPIN_AWAY_BULLET
                                    ,bullet_model
                                    ,.solid_type=DEFAULT_BULLET_SOLID
                                    ,.movetype=MOVETYPE_NOCLIP, .owner=owner
                                    ,.r=colors[0]
                                    ,.g=colors[1]
                                    ,.b=colors[2]
                                    ,.a=colors[3]
                                    ,.min_size=BULLET_MIN_SIZE
                                    ,.max_size=BULLET_MAX_SIZE);
            
            
            if(ce_index==-1)
                continue;
                
            velocity[0] = initial_radius * angular_spd * Cosine(angle_offset);
            velocity[1] = initial_radius * angular_spd *   Sine(angle_offset);

            tbk_fuser[ce_index][0] = initial_radius;
            tbk_fuser[ce_index][1] = radius_increase_per_second;		//already *= bullet_thinktime
            tbk_fuser[ce_index][2] = angle_offset;
            tbk_fuser[ce_index][3] = angular_spd;

            TeleportEntity(g_iCustomEntityRef[ce_index], origin, NULL_VECTOR, velocity);
            SetTsubakiEntityDamage(ce_index, damage);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);


            SetEntityNextThink(ce_index, DEFAULT_BULLET_THINKTIME, SpinAwayBulletThink, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
            SetRemoveEntityTask(ce_index, last_time);

            angle_offset += M_2PI / bullet_amount;
        }
    }

        public Action SpinAwayBulletThink(Handle timer, int ce_index)
        {
            tbk_fuser[ce_index][0] += tbk_fuser[ce_index][1];
            tbk_fuser[ce_index][2] += tbk_fuser[ce_index][3] * DEFAULT_BULLET_THINKTIME;

            tbk_vuser[ce_index][0][0] = tbk_fuser[ce_index][0] * tbk_fuser[ce_index][3] * Cosine(tbk_fuser[ce_index][2]);
            tbk_vuser[ce_index][0][1] = tbk_fuser[ce_index][0] * tbk_fuser[ce_index][3] *   Sine(tbk_fuser[ce_index][2]);
            tbk_vuser[ce_index][0][2] = 0.0;
            
            TeleportEntity(g_iCustomEntityRef[ce_index], NULL_VECTOR, NULL_VECTOR, tbk_vuser[ce_index][0]);
            return Plugin_Continue;
        }

/*  Spin Away Split Bullet  */
    /*
    * 生成後以 {angular_spd}旋轉，並以每秒{radius_increase_per_second}單位距離遠離中心點
    * 同時，每{split_interval}物分裂出{split_amount}粒維持{split_bullet_last_time}秒的彈幕
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 生成數
    * @param initial_radius 生成時與中心點半徑距離
    * @param radius_increase_per_second 每秒遠離中心點的單位距離
    * @param angle_offset 生成角度偏差(degree)
    * @param angular_spd 每秒旋轉角速度(degree)
    * @param last_time 存在時間
    * @param split_amount 每次分裂出來的彈幕數
    * @param split_angle 分裂的彈幕與母體的運動角度偏差(degree)
    * @param split_bullet_spd 分裂的彈幕速度
    * @param split_bullet_last_time 分裂的彈幕存在時間
    * @param split_interval 每次分裂所需時間
    * @param colors 母體彈幕顏色
    * @param split_colors 分裂彈幕的顏色
    * @param damage 母體傷害
    * @param split_damage 分裂彈幕傷害
    *
    */
    stock SpinAwaySplitBullet(const char[] bullet_model, Float:origin[3], owner=0, bullet_amount=3, 
                                    Float:initial_radius=10.0, Float:radius_increase_per_second=100.0, 
                                    Float:angle_offset=0.0, Float:angular_spd=45.0, Float:last_time=12.0,
                                    split_amount=1, Float:split_angle=-90.0, Float:split_bullet_spd=150.0, 
                                    Float:split_bullet_last_time=7.0, Float:split_interval=1.0, 
                                    colors[4]={153,0,153,255}, split_colors[4]={153,0,153,255},
                                    Float:damage=DEFAULT_BULLET_DMG, 
                                    Float:split_damage=DEFAULT_BULLET_DMG)
    {
        new i=0, ce_index, rgba = CreateRGBAInt(split_colors[0], split_colors[1], split_colors[2], split_colors[3]);
        new Float:spawn_origin[3], Float:velocity[3], Float:split_bullet_data[3];

        angular_spd = ToRadian(angular_spd);
        angle_offset = ToRadian(angle_offset);
        radius_increase_per_second *= DEFAULT_BULLET_THINKTIME;

        split_bullet_data[0] = ToRadian(split_angle);
        split_bullet_data[1] = split_bullet_spd;
        split_bullet_data[2] = split_bullet_last_time;

        for(i=0; i<bullet_amount; i++)
        {
            ce_index=CreateTsubakiEntity(SPIN_AWAY_SPLIT_BULLET
                                ,bullet_model
                                ,.solid_type=DEFAULT_BULLET_SOLID
                                ,.movetype=MOVETYPE_NOCLIP
                                ,.owner=owner
                                ,.r=colors[0]
                                ,.g=colors[1]
                                ,.b=colors[2]
                                ,.a=colors[3]
                                ,.min_size=BULLET_MIN_SIZE
                                ,.max_size=BULLET_MAX_SIZE);
            
            if(ce_index==-1)
                continue;

            spawn_origin[0] = origin[0] + initial_radius * Cosine(angle_offset);
            spawn_origin[1] = origin[1] + initial_radius *   Sine(angle_offset);
            spawn_origin[2] = origin[2];

            velocity[0] = initial_radius * angular_spd * Cosine(angle_offset);
            velocity[1] = initial_radius * angular_spd *   Sine(angle_offset);

            tbk_iuser[ce_index][0] = split_amount;
            tbk_iuser[ce_index][1] = rgba;
            tbk_iuser[ce_index][2] = 0;          //End Counter
            tbk_iuser[ce_index][3] = RoundFloat(split_interval/DEFAULT_BULLET_THINKTIME);

            tbk_fuser[ce_index][0] = initial_radius;
            tbk_fuser[ce_index][1] = radius_increase_per_second;		//Already *= thinktime
            tbk_fuser[ce_index][2] = angle_offset;
            tbk_fuser[ce_index][3] = angular_spd;
            tbk_fuser[ce_index][4] = split_damage;

            CopyVector(tbk_vuser[ce_index][0], split_bullet_data);

            TeleportEntity(g_iCustomEntityRef[ce_index], spawn_origin, NULL_VECTOR, velocity);
            SetTsubakiEntityDamage(ce_index, damage);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);

            SetEntityNextThink(ce_index, DEFAULT_BULLET_THINKTIME, SpinAwaySplitBulletThink, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
            SetRemoveEntityTask(ce_index, last_time);

            angle_offset += M_2PI / bullet_amount;
        }
    }

        public Action SpinAwaySplitBulletThink(Handle timer, int ce_index)
        {
            tbk_fuser[ce_index][0] += tbk_fuser[ce_index][1];
            tbk_fuser[ce_index][2] += tbk_fuser[ce_index][3]*DEFAULT_BULLET_THINKTIME;

            if((++tbk_iuser[ce_index][2])%tbk_iuser[ce_index][3] == 0)
            {
                new sub_ce_index, i=0, owner=GetEntityOwner(g_iCustomEntityRef[ce_index]), amount=tbk_iuser[ce_index][0];
                new r = GetRGB_R(tbk_iuser[ce_index][1]), g = GetRGB_G(tbk_iuser[ce_index][1]), b = GetRGB_B(tbk_iuser[ce_index][1]), a = GetRGBA_A(tbk_iuser[ce_index][1]);
                new Float:origin[3], Float:velocity[3], Float:angle = tbk_fuser[ce_index][2]+tbk_vuser[ce_index][0][0] , Float:spd = tbk_vuser[ce_index][0][1], Float:lasttime = tbk_vuser[ce_index][0][2], Float:damage = tbk_fuser[ce_index][4];
                new String:model_route[128];

                GetEntityModel(g_iCustomEntityRef[ce_index], model_route);
                GetEntityOrigin(g_iCustomEntityRef[ce_index], origin);

                for(; i<amount; i++)
                {
                    sub_ce_index=CreateTsubakiEntity(NORMAL_BULLET
                                            ,model_route
                                            ,.solid_type=DEFAULT_BULLET_SOLID
                                            ,.movetype=MOVETYPE_NOCLIP
                                            ,.owner=owner
                                            ,.r=r
                                            ,.g=g
                                            ,.b=b
                                            ,.a=a
                                            ,.min_size=BULLET_MIN_SIZE
                                            ,.max_size=BULLET_MAX_SIZE);
                    
                    if(sub_ce_index==-1)
                        continue;

                    velocity[0] = spd * Cosine(angle);
                    velocity[1] = spd *   Sine(angle);

                    TeleportEntity(g_iCustomEntityRef[sub_ce_index], origin, NULL_VECTOR, velocity);
                    SetTsubakiEntityDamage(sub_ce_index, damage);
                    SDKHook(g_iCustomEntityRef[sub_ce_index], SDKHook_Touch, NormalBulletTouch);

                    SetRemoveEntityTask(sub_ce_index, lasttime);

                    angle += M_2PI / amount;
                }
            }

            tbk_vuser[ce_index][1][0] = tbk_fuser[ce_index][0] * tbk_fuser[ce_index][3] * Cosine(tbk_fuser[ce_index][2]);
            tbk_vuser[ce_index][1][1] = tbk_fuser[ce_index][0] * tbk_fuser[ce_index][3] *   Sine(tbk_fuser[ce_index][2]);
            tbk_vuser[ce_index][1][2] = 0.0;

            TeleportEntity(g_iCustomEntityRef[ce_index], NULL_VECTOR, NULL_VECTOR, tbk_vuser[ce_index][1]);
            return Plugin_Continue;
        }

/*  Full Random Bullet  */

    /*
    * 生成隨機角度的方向前進及以隨機速度前進的彈幕【一斉発射】
    *　
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 生成數
    * @param min_spd 隨機速度最少值
    * @param min_spd 隨機速度最大值
    * @param last_distance 彈幕最大移動距離
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    */
    stock FullRandomBullet(const String:bullet_model[], Float:origin[3], owner=0, bullet_amount=64, Float:min_spd=100.0, Float:max_spd=600.0,
                                Float:last_distance=1000.0, colors[4]={100,0,255,255}, Float:damage=5.0)
    {
        new i, ce_index;
        new Float:spd, Float:angle, Float:velocity[3], Float:spawn_origin[3];

        for(i=0; i<bullet_amount; i++)
        {
            ce_index=CreateTsubakiEntity(NORMAL_BULLET
                                ,bullet_model
                                ,.solid_type=DEFAULT_BULLET_SOLID
                                ,.movetype=MOVETYPE_NOCLIP
                                ,.owner=owner
                                ,.r=colors[0]
                                ,.g=colors[1]
                                ,.b=colors[2]
                                ,.a=colors[3]
                                ,.min_size=BULLET_MIN_SIZE
                                ,.max_size=BULLET_MAX_SIZE);
            
            if( ce_index==-1 )
                continue;

            angle = GetRandomFloat(0.0, M_2PI);
            spd = GetRandomFloat(min_spd, max_spd);

            spawn_origin[0] = origin[0] + 50.0 * Cosine(angle);
            spawn_origin[1] = origin[1] + 50.0 *   Sine(angle);
            spawn_origin[2] = origin[2];

            velocity[0] = spd * Cosine(angle);
            velocity[1] = spd *   Sine(angle);

            TeleportEntity(g_iCustomEntityRef[ce_index], origin, NULL_VECTOR, velocity);
            SetTsubakiEntityDamage(ce_index, damage);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);

            SetRemoveEntityTask(ce_index, last_distance/spd);
        }
    }



/*  All Random Machine Bullet  */

    /*
    * 生成隨機角度的方向前進及以隨機速度前進的彈幕【連続発射】
    *　
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param min_bullet_amount 每次生成最小數
    * @param max_bullet_amount 每次生成最大數
    * @param min_spd 隨機速度最少值
    * @param min_spd 隨機速度最大值
    * @param last_distance 彈幕最大移動距離
    * @param fire_rate 每 n 秒發射一次(最小0.1)
    * @param last_time 發射總時間
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    */
    stock AllRandomMachineBullet(const String:model_route[], Float:origin[3], owner=0, 
        min_bullet_amount=2, max_bullet_amount=6, 
        Float:min_spd=100.0, Float:max_spd=500.0, Float:bullet_last_distance=1000.0,
        Float:fire_rate=0.1, Float:last_time=8.0,
        colors[4]={100,0,255,255}, Float:damage=DEFAULT_BULLET_DMG)
    {
        new ce_index = CreateInvisibleLauncher(RANDOM_MACHINE_BULLET_LAUNCHER, 
                                            model_route,
                                            .owner=owner);

        if(ce_index==-1)
            return;

        tbk_iuser[ce_index][0] = min_bullet_amount;
        tbk_iuser[ce_index][1] = max_bullet_amount;
        tbk_iuser[ce_index][2] = CreateRGBAInt(colors[0], colors[1], colors[2], colors[3]);

        tbk_fuser[ce_index][0] = min_spd;
        tbk_fuser[ce_index][1] = max_spd;
        tbk_fuser[ce_index][2] = bullet_last_distance;
        tbk_fuser[ce_index][3] = fire_rate;

        TeleportEntity(g_iCustomEntityRef[ce_index], origin, NULL_VECTOR, NULL_VECTOR);
        SetEntityNextThink(ce_index, fire_rate, AllRandomMachineBulletLaunch, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
        SetTsubakiEntityDamage(ce_index, damage);

        SetRemoveEntityTask(ce_index, last_time);
    }

        public Action AllRandomMachineBulletLaunch(Handle timer, int ce_index)
        {
            new sub_ce_index, owner=GetEntityOwner(g_iCustomEntityRef[ce_index]);
            new r=GetRGB_R(tbk_iuser[ce_index][2]), g=GetRGB_G(tbk_iuser[ce_index][2]), b=GetRGB_B(tbk_iuser[ce_index][2]), a=GetRGBA_A(tbk_iuser[ce_index][2]);
            new i=0, bullet_amount = GetRandomInt(tbk_iuser[ce_index][0], tbk_iuser[ce_index][1]);
            new Float:origin[3], Float:angle, Float:velocity[3], Float:speed, Float:lasttime, Float:damage=GetTsubakiEntityDamage(ce_index);
            new String:bullet_model[128];
            GetEntityModel(g_iCustomEntityRef[ce_index], bullet_model);
            GetEntityOrigin(g_iCustomEntityRef[ce_index], origin);

            for(; i<bullet_amount; i++)
            {
                sub_ce_index=CreateTsubakiEntity(NORMAL_BULLET
                                        ,bullet_model
                                        ,.solid_type=DEFAULT_BULLET_SOLID
                                        ,.movetype=MOVETYPE_NOCLIP
                                        ,.owner=owner
                                        ,.r=r
                                        ,.g=g
                                        ,.b=b
                                        ,.a=a
                                        ,.min_size=BULLET_MIN_SIZE
                                        ,.max_size=BULLET_MAX_SIZE);
                if(sub_ce_index == -1)
                    continue;

                angle = GetRandomFloat(0.0, M_2PI);
                speed = GetRandomFloat(tbk_fuser[ce_index][0], tbk_fuser[ce_index][1]);
                lasttime = tbk_fuser[ce_index][2] / speed;

                velocity[0] = speed * Cosine(angle);
                velocity[1] = speed *   Sine(angle);
                velocity[2] = 0.0;

                TeleportEntity(g_iCustomEntityRef[sub_ce_index], origin, NULL_VECTOR, velocity);
                SetTsubakiEntityDamage(sub_ce_index, damage);
                SDKHook(g_iCustomEntityRef[sub_ce_index], SDKHook_Touch, NormalBulletTouch);
                
                SetRemoveEntityTask(sub_ce_index, lasttime);
            }

            return Plugin_Continue;
        }


/* Rotate Machine Bullet */

    /*
    * 連続発射直線前進的彈幕
    *　
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_per_fire 每次生成數
    * @param fire_interval 每 n 秒發射一次(最小0.1)
    * @param bullet_speed 彈幕速度
    * @param angle_offset 生成時角度偏差
    * @param angular_spd 每次發射角度變化
    * @param total_rotation 總角度變化
    * @param last_time 彈幕維持時間
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    */
    stock RotateMachineBullet(const String:bullet_model[], Float:origin[3], owner=0, 
        bullet_per_fire=2, Float:fire_interval = 0.1, Float:bullet_speed=180.0,
        Float:angle_offset=0.0, Float:angular_spd=10.0, Float:total_rotation=180.0, 
        Float:last_time=4.0, colors[4]={255,0,255,255}, Float:damage=DEFAULT_BULLET_DMG)
    {
        new ce_index = CreateInvisibleLauncher(.clsname=ROTATE_MACHINE_BULLET_LAUNCHER, 
                                                .model_route=bullet_model, 
                                                .owner=owner);

        if(ce_index == -1)
            return;

        tbk_iuser[ce_index][0] = bullet_per_fire;
        tbk_iuser[ce_index][1] = CreateRGBAIntByArray(colors);
        tbk_iuser[ce_index][2] = 0;
        tbk_iuser[ce_index][3] = Abs(RoundFloat(total_rotation/angular_spd));

        tbk_fuser[ce_index][0] = fire_interval;
        tbk_fuser[ce_index][1] = bullet_speed;
        tbk_fuser[ce_index][2] = last_time;
        tbk_fuser[ce_index][3] = ToRadian(angle_offset);
        tbk_fuser[ce_index][4] = ToRadian(angular_spd);

        TeleportEntity(g_iCustomEntityRef[ce_index], origin, NULL_VECTOR, NULL_VECTOR);
        SetTsubakiEntityDamage(ce_index, damage);
        
        RotateMachineBulletLauncherThink(INVALID_HANDLE, ce_index);
        SetEntityNextThink(ce_index, tbk_fuser[ce_index][0], RotateMachineBulletLauncherThink, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
    }

        public Action RotateMachineBulletLauncherThink(Handle timer, int ce_index)
        {
            new sub_ce_index, i=0, String:bullet_model[128], Float:damage=GetTsubakiEntityDamage(ce_index), Float:origin[3], Float:velocity[3], Float:angle, owner=GetEntityOwner(g_iCustomEntityRef[ce_index]);
            GetEntityModel(g_iCustomEntityRef[ce_index], bullet_model);
            GetEntityOrigin(g_iCustomEntityRef[ce_index], origin);

            for(; i<tbk_iuser[ce_index][0]; i++)
            {
                sub_ce_index=CreateTsubakiEntity(NORMAL_BULLET
                                ,bullet_model
                                ,.solid_type=DEFAULT_BULLET_SOLID
                                ,.movetype=MOVETYPE_NOCLIP
                                ,.owner=owner
                                ,.r= GetRGB_R(tbk_iuser[ce_index][1])
                                ,.g= GetRGB_G(tbk_iuser[ce_index][1])
                                ,.b= GetRGB_B(tbk_iuser[ce_index][1])
                                ,.a=GetRGBA_A(tbk_iuser[ce_index][1])
                                ,.min_size=BULLET_MIN_SIZE
                                ,.max_size=BULLET_MAX_SIZE);
                
                if(sub_ce_index == -1)
                    continue;

                angle = tbk_fuser[ce_index][3] + M_2PI * i / tbk_iuser[ce_index][0];

                //Velocity
                velocity[0] = tbk_fuser[ce_index][1] * Cosine(angle);
                velocity[1] = tbk_fuser[ce_index][1] *   Sine(angle);
                velocity[2] = 0.0;
                
                TeleportEntity(g_iCustomEntityRef[sub_ce_index], origin, NULL_VECTOR, velocity);
                SetTsubakiEntityDamage(sub_ce_index, damage);
                SDKHook(g_iCustomEntityRef[sub_ce_index], SDKHook_Touch, NormalBulletTouch);

                SetRemoveEntityTask(sub_ce_index, tbk_fuser[ce_index][2]);
            }

            if(++tbk_iuser[ce_index][2] < tbk_iuser[ce_index][3]) {       //Still Rotate?
                tbk_fuser[ce_index][3] += tbk_fuser[ce_index][4];             //Add Angle
            }
            else {
                ResetEntityThinkHandle(ce_index);
                RemoveTsubakiEntity(INVALID_HANDLE, ce_index);
                return Plugin_Stop;
            }

            return Plugin_Continue;
        }


/*  Machine Slow Change Dir Bullet  */


    /*
    * 連続発射直線前進，並以每秒{deceleration}減速的彈幕
    * 當彈幕速度歸0時，
    * 向本來運動角度相差90度~270度的隨機角度以每秒{deceleration}加速
    *　
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_per_fire 每次生成數
    * @param total_fire 總發射次數
    * @param initial_angle 生成時角度
    * @param angular_spd 每次發射角度變化
    * @param fire_rate 每 n 秒發射一次(最小0.1)
    * @param spd 彈幕初速
    * @param deceleration 每秒減速・每秒加速
    * @param bullet_last_time 彈幕維持時間
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    */
    stock LaunchSlowChangeDirBullet(const String:bullet_model[], Float:origin[3], owner=0, 
                                        bullet_per_fire=2, total_fire=36,
                                        Float:initial_angle=0.0, Float:angular_spd=10.0,
                                        Float:fire_rate=0.1, Float:spd=300.0, Float:deceleration=80.0, Float:bullet_last_time=10.0, 
                                        colors[4]={255,178,102,255}, Float:damage=DEFAULT_BULLET_DMG)
    {
        new ce_index = CreateInvisibleLauncher(SLOW_CHANGE_DIR_BULLET_LAUNCHER, bullet_model, .owner=owner);

        if(ce_index == -1)
            return;

        tbk_iuser[ce_index][0] = 0;
        tbk_iuser[ce_index][1] = total_fire;
        tbk_iuser[ce_index][2] = CreateRGBAIntByArray(colors);
        tbk_iuser[ce_index][3] = RoundFloat(fire_rate*1000);
        tbk_iuser[ce_index][4] = bullet_per_fire;

        tbk_fuser[ce_index][0] = spd;
        tbk_fuser[ce_index][1] = deceleration * DEFAULT_BULLET_THINKTIME;
        tbk_fuser[ce_index][2] = bullet_last_time;
        tbk_fuser[ce_index][3] = ToRadian(initial_angle);
        tbk_fuser[ce_index][4] = ToRadian(angular_spd);

        SetTsubakiEntityDamage(ce_index, damage);

        TeleportEntity(g_iCustomEntityRef[ce_index], origin, NULL_VECTOR, NULL_VECTOR);
        SlowChangeDirBulletLauncherThink(INVALID_HANDLE, ce_index);
        SetEntityNextThink(ce_index, DEFAULT_BULLET_THINKTIME, SlowChangeDirBulletLauncherThink, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
        
    }

        public Action SlowChangeDirBulletLauncherThink(Handle timer, int ce_index)
        {
            new sub_ce_index, r=GetRGB_R(tbk_iuser[ce_index][2]), g=GetRGB_G(tbk_iuser[ce_index][2]), b=GetRGB_B(tbk_iuser[ce_index][2]), a=GetRGBA_A(tbk_iuser[ce_index][2]), owner=GetEntityOwner(g_iCustomEntityRef[ce_index]);
            new i=0, bullet_amount=tbk_iuser[ce_index][4], String:bullet_model[128];
            new Float:origin[3], Float:velocity[3], Float:spd=tbk_fuser[ce_index][0], Float:angle=tbk_fuser[ce_index][3]+tbk_iuser[ce_index][0]*tbk_fuser[ce_index][4], Float:damage=GetTsubakiEntityDamage(ce_index);
            
            GetEntityModel(g_iCustomEntityRef[ce_index], bullet_model);

            for(; i<bullet_amount; i++) {
                
                sub_ce_index=CreateTsubakiEntity(SLOW_CHANGE_DIR_BULLET
                                            ,bullet_model
                                            ,.solid_type=DEFAULT_BULLET_SOLID
                                            ,.movetype=MOVETYPE_NOCLIP
                                            ,.owner=owner
                                            ,.r=r
                                            ,.g=g
                                            ,.b=b
                                            ,.a=a
                                            ,.min_size=BULLET_MIN_SIZE
                                            ,.max_size=BULLET_MAX_SIZE);

                if(sub_ce_index == -1)
                    continue;

                GetEntityOrigin(g_iCustomEntityRef[ce_index], origin);
                velocity[0] = spd * Cosine(angle);
                velocity[1] = spd *   Sine(angle);

                tbk_iuser[sub_ce_index][0] = 0;		//phase

                tbk_fuser[sub_ce_index][0] = spd;
                tbk_fuser[sub_ce_index][1] = tbk_fuser[ce_index][1];		//Deceleration
                tbk_fuser[sub_ce_index][2] = angle;

                TeleportEntity(g_iCustomEntityRef[sub_ce_index], origin, NULL_VECTOR, velocity);
                SetTsubakiEntityDamage(sub_ce_index, damage);
                SDKHook(g_iCustomEntityRef[sub_ce_index], SDKHook_Touch, NormalBulletTouch);

                SetEntityNextThink(sub_ce_index, DEFAULT_BULLET_THINKTIME, SlowChangeDirBulletThink, TIMER_FLAG_NO_MAPCHANGE);
                SetRemoveEntityTask(sub_ce_index, tbk_fuser[ce_index][2]);
            }

            if(++tbk_iuser[ce_index][0] < tbk_iuser[ce_index][1]) {
                return Plugin_Continue;
            }
            else{
                ResetEntityThinkHandle(ce_index);
                RemoveTsubakiEntity(INVALID_HANDLE, ce_index);
                return Plugin_Stop;
            }
        }

        public Action SlowChangeDirBulletThink(Handle timer, int ce_index)
        {
            new Float:velocity[3], Float:spd, Float:deceleration, Float:angle;

            spd = tbk_fuser[ce_index][0];
            deceleration = tbk_fuser[ce_index][1];
            angle = tbk_fuser[ce_index][2];

            if(tbk_iuser[ce_index][0] == 0)
            {
                if((spd-=deceleration) < 0.0)
                {
                    tbk_iuser[ce_index][0] = 1;
                    spd = 0.0;
                    tbk_fuser[ce_index][2] += M_PI + GetRandomFloat(-M_1OVER2PI, M_1OVER2PI);
                    
                    SetEntityNextThink(ce_index, 0.5, SlowChangeDirBulletThink, TIMER_FLAG_NO_MAPCHANGE);
                } else {
                    SetEntityNextThink(ce_index, DEFAULT_BULLET_THINKTIME, SlowChangeDirBulletThink, TIMER_FLAG_NO_MAPCHANGE);
                }
            }
            else
            {
                spd += deceleration;
                SetEntityNextThink(ce_index, DEFAULT_BULLET_THINKTIME, SlowChangeDirBulletThink, TIMER_FLAG_NO_MAPCHANGE);
            }

            tbk_fuser[ce_index][0] = spd;
            velocity[0] = spd * Cosine(angle);
            velocity[1] = spd *   Sine(angle);

            TeleportEntity(g_iCustomEntityRef[ce_index], .velocity=velocity);
            return Plugin_Stop;
        }

/* Round Spin Bullet */

    /*
    * 生成以速度{speed1}直線前進，
    * {rotate_delay_time}秒後以半徑{radius}及角速度{agnular_spd}圓周運動
    * 最後以{speed2}直線前進的彈幕【連続発射】
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 彈幕生成數
    * @param angle_offset 生成時角度偏差(degree)
    * @param speed1 圓周運動前速難難
    * @param rotate_delay_time n秒後進入圓周運動
    * @param radius 圓周運動時半徑
    * @param angular_spd 圓周運動時角速度(degree)
    * @param total_total_rotate 圓周運動總角度(degree)
    * @param speed2 圓周運動後速度
    * @param last_time 彈幕維持時間
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    */
    stock RoundSpinBullet(const String:bullet_model[], Float:origin[3], owner=0, bullet_amount=8, 
        Float:angle_offset=0.0, Float:speed1=70.0, 
        Float:rotate_delay_time=2.0, 
        Float:radius=500.0, Float:angular_spd=120.0, Float:total_rotate=360.0, 
        Float:speed2=200.0,
        Float:last_time=9.0, colors[4]={0,255,0,255}, Float:damage=DEFAULT_BULLET_DMG)
    {
        angular_spd = ToRadian(angular_spd) * DEFAULT_BULLET_THINKTIME;
        total_rotate = ToRadian(total_rotate);
        angle_offset = ToRadian(angle_offset);

        new ce_index, i=0;
        new Float:angle, Float:velocity[3];

        for(; i<bullet_amount; i++)
        {
            ce_index=CreateTsubakiEntity(ROUND_SPIN_BULLET
                                    ,bullet_model
                                    ,.solid_type=DEFAULT_BULLET_SOLID
                                    ,.movetype=MOVETYPE_NOCLIP
                                    ,.owner=owner
                                    ,.r=colors[0]
                                    ,.g=colors[1]
                                    ,.b=colors[2]
                                    ,.a=colors[3]
                                    ,.min_size=BULLET_MIN_SIZE
                                    ,.max_size=BULLET_MAX_SIZE);
            
            if(ce_index == -1)
                continue;

            angle = angle_offset + i * M_2PI / bullet_amount;
            velocity[0] = speed1 * Cosine(angle);
            velocity[1] = speed1 *   Sine(angle);

            tbk_iuser[ce_index][0] = 0;
        
            tbk_fuser[ce_index][0] = angle;
            tbk_fuser[ce_index][1] = angle + total_rotate;
            tbk_fuser[ce_index][2] = radius;
            tbk_fuser[ce_index][3] = angular_spd;
            tbk_fuser[ce_index][4] = speed2;

            tbk_vuser[ce_index][0][0] = tbk_vuser[ce_index][0][1] = tbk_vuser[ce_index][0][2] = 0.0;

            TeleportEntity(g_iCustomEntityRef[ce_index], origin, .velocity=velocity);
            SetTsubakiEntityDamage(ce_index, damage);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);
            
            SetEntityNextThink(ce_index, rotate_delay_time, RoundSpinBulletThink, TIMER_FLAG_NO_MAPCHANGE);
            SetRemoveEntityTask(ce_index, last_time);
        }
    }

        public Action RoundSpinBulletThink(Handle timer, int ce_index)
        {
            tbk_fuser[ce_index][0] += tbk_fuser[ce_index][3];

            if(FloatAbs(tbk_fuser[ce_index][0]) < FloatAbs(tbk_fuser[ce_index][1]))
            {
                tbk_vuser[ce_index][0][0] = tbk_fuser[ce_index][2] * tbk_fuser[ce_index][3] * Cosine(tbk_fuser[ce_index][0]);
                tbk_vuser[ce_index][0][1] = tbk_fuser[ce_index][2] * tbk_fuser[ce_index][3] *   Sine(tbk_fuser[ce_index][0]);
                TeleportEntity(g_iCustomEntityRef[ce_index], .velocity=tbk_vuser[ce_index][0]);

                if(!tbk_iuser[ce_index][0])
                {
                    SetEntityNextThink(ce_index, DEFAULT_BULLET_THINKTIME, RoundSpinBulletThink, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
                    tbk_iuser[ce_index][0] = 1;
                }

                return Plugin_Continue;
            }
            else
            {
                tbk_vuser[ce_index][0][0] = tbk_fuser[ce_index][4] * Cosine(tbk_fuser[ce_index][1]);
                tbk_vuser[ce_index][0][1] = tbk_fuser[ce_index][4] *   Sine(tbk_fuser[ce_index][1]);
                TeleportEntity(g_iCustomEntityRef[ce_index], .velocity=tbk_vuser[ce_index][0]);

                ResetEntityThinkHandle(ce_index);
                return Plugin_Stop;
            }
        }

/*  Change Dest Bullet  */

    /*
    * 在{origin}生成一個直線移動，
    * 在{delay}秒後向{dest_origin}移動的彈幕
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param dest_origin 目的地位置
    * @param owner 主人
    * @param angle 初速的移動方位角(degree)
    * @param start_spd 初速
    * @param end_spd 向目的地移動時的速度
    * @param delay N秒後開始向目的地移動時
    * @param last_time 彈幕維持時間
    * @param colors 彈幕顏色
    * @param end_colors 向目的地移動時彈幕顏色
    * @param damage 彈幕傷害
    *
    */  
    stock ChangeDestBullet(const String:bullet_model[], Float:origin[3], Float:dest_origin[3], owner=0, Float:angle=0.0, Float:start_spd=200.0, Float:end_spd=250.0, 
                            Float:delay=3.0, Float:last_time=8.0, 
                            colors[4]={204,0,102,255}, end_colors[4]={102,255,0,255}, 
                            Float:damage=DEFAULT_BULLET_DMG)
    {
        new ce_index=CreateTsubakiEntity(CHANGE_DEST_BULLET
                                ,bullet_model
                                ,.solid_type=DEFAULT_BULLET_SOLID
                                ,.movetype=MOVETYPE_NOCLIP
                                ,.owner=owner
                                ,.r=colors[0]
                                ,.g=colors[1]
                                ,.b=colors[2]
                                ,.a=colors[3]
                                ,.min_size=BULLET_MIN_SIZE
                                ,.max_size=BULLET_MAX_SIZE);
        
        if(ce_index==-1)
            return;
            
        new Float:velocity[3];

        velocity[0] = start_spd * Cosine(angle);
        velocity[1] = start_spd * 	Sine(angle);
        velocity[2] = 0.0;

        tbk_iuser[ce_index][0] = CreateRGBAInt(end_colors[0], end_colors[1], end_colors[2], end_colors[3]);

        tbk_fuser[ce_index][0] = end_spd;		//Speed

        CopyVector(tbk_vuser[ce_index][0], dest_origin);

        TeleportEntity(g_iCustomEntityRef[ce_index], origin, .velocity=velocity);
        SetTsubakiEntityDamage(ce_index, damage);
        SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);

        SetEntityNextThink(ce_index, delay, ChangeDestBulletThink, TIMER_FLAG_NO_MAPCHANGE);
        SetRemoveEntityTask(ce_index, last_time);
    }
    
        public Action ChangeDestBulletThink(Handle timer, int ce_index)
        {
            GetEntityOrigin(g_iCustomEntityRef[ce_index], tbk_vuser[ce_index][1]);	//origin

            SubtractVectors(tbk_vuser[ce_index][0], tbk_vuser[ce_index][1], tbk_vuser[ce_index][0]);
            GetVectorAngles(tbk_vuser[ce_index][0], tbk_vuser[ce_index][0]);

            tbk_vuser[ce_index][0][1] = ToRadian(tbk_vuser[ce_index][0][1]);		//GetVectorAngles Return Degree

            tbk_vuser[ce_index][4][0] = tbk_fuser[ce_index][0] * Cosine(tbk_vuser[ce_index][0][1]);
            tbk_vuser[ce_index][4][1] = tbk_fuser[ce_index][0] *   Sine(tbk_vuser[ce_index][0][1]);
            tbk_vuser[ce_index][4][2] = 0.0;

            TeleportEntity(g_iCustomEntityRef[ce_index], .velocity=tbk_vuser[ce_index][4]);
            SetEntityRenderColor(GetRGB_R(tbk_iuser[ce_index][0]), GetRGB_G(tbk_iuser[ce_index][0]), GetRGB_B(tbk_iuser[ce_index][0]), GetRGBA_A(tbk_iuser[ce_index][0]));

            ResetEntityThinkHandle(ce_index);
            return Plugin_Stop;
        }

/*  Change Angle Bullet  */

    /*
    * 在{origin}生成一個直線移動，
    * 每{switch_delay}後運動角度增加{angle_change_per_phase}的彈幕
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param spd 初速
    * @param speed_diff 每次運動變化時上昇速度
    * @param initial_angle 生成時運動角度
    * @param angle_change_per_phase 每次運動變化時上昇角度(degree)
    * @param switch_delay 運動變化時間
    * @param last_time 彈幕維持時間
    * @param colors 彈幕顏色
    * @param colors_increase 每次角度變化時彈幕顏色變化
    * @param damage 彈幕傷害
    *
    */ 
    stock ChangeAngleBullet(const String:bullet_model[], Float:origin[3], owner=0, Float:spd=200.0, Float:speed_diff=0.0, Float:initial_angle=0.0, 
                                    Float:angle_change_per_phase=90.0, Float:switch_delay=2.0, Float:last_time=8.0,
                                    colors[4]={100,200,55,255}, colors_increase[4]={25,50,0,0},
                                    Float:damage=DEFAULT_BULLET_DMG)
    {
        initial_angle = ToRadian(initial_angle);
        angle_change_per_phase = ToRadian(angle_change_per_phase);
        new ce_index=CreateTsubakiEntity(CHANGE_ANGLE_BULLET
                                    ,bullet_model
                                    ,.solid_type=DEFAULT_BULLET_SOLID
                                    ,.movetype=MOVETYPE_NOCLIP
                                    ,.owner=owner
                                    ,.r=colors[0]
                                    ,.g=colors[1]
                                    ,.b=colors[2]
                                    ,.a=colors[3]
                                    ,.min_size=BULLET_MIN_SIZE
                                    ,.max_size=BULLET_MAX_SIZE);

        if(ce_index == -1)
            return;

        tbk_vuser[ce_index][0][0] = spd * Cosine(initial_angle);
        tbk_vuser[ce_index][0][1] = spd *   Sine(initial_angle);
        tbk_vuser[ce_index][0][2] = 0.0;

        tbk_iuser[ce_index][0] = CreateRGBAInt(colors[0], colors[1], colors[2], colors[3]);
        tbk_iuser[ce_index][1] = CreateRGBAInt(colors_increase[0], colors_increase[1], colors_increase[2], colors_increase[3]);

        tbk_fuser[ce_index][0] = spd;
        tbk_fuser[ce_index][1] = speed_diff;
        tbk_fuser[ce_index][2] = initial_angle;
        tbk_fuser[ce_index][3] = angle_change_per_phase;
        tbk_fuser[ce_index][4] = switch_delay;

        TeleportEntity(g_iCustomEntityRef[ce_index], origin, .velocity=tbk_vuser[ce_index][0]);
        SetTsubakiEntityDamage(ce_index, damage);
        SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);

        SetEntityNextThink(ce_index, switch_delay, ChangeAngleBulletThink, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
        SetRemoveEntityTask(ce_index, last_time);
    }

        public Action ChangeAngleBulletThink(Handle timer, int ce_index)
        {
            tbk_fuser[ce_index][0] += tbk_fuser[ce_index][1];     //spd diff
            tbk_fuser[ce_index][2] += tbk_fuser[ce_index][3];     //angle diff

            tbk_vuser[ce_index][0][0] = tbk_fuser[ce_index][0] * Cosine(tbk_fuser[ce_index][2]);
            tbk_vuser[ce_index][0][1] = tbk_fuser[ce_index][0] *   Sine(tbk_fuser[ce_index][2]);

            tbk_iuser[ce_index][0] = CreateRGBAInt(
                 (GetRGB_R(tbk_iuser[ce_index][0]) + GetRGB_R(tbk_iuser[ce_index][1]))%256,
                 (GetRGB_G(tbk_iuser[ce_index][0]) + GetRGB_G(tbk_iuser[ce_index][1]))%256,
                 (GetRGB_B(tbk_iuser[ce_index][0]) + GetRGB_B(tbk_iuser[ce_index][1]))%256, 
                 (GetRGBA_A(tbk_iuser[ce_index][0]) + GetRGBA_A(tbk_iuser[ce_index][1]))%256
            );

            TeleportEntity(g_iCustomEntityRef[ce_index], .velocity=tbk_vuser[ce_index][0]);
            SetEntityRenderColor(g_iCustomEntityRef[ce_index], GetRGB_R(tbk_iuser[ce_index][0]), GetRGB_G(tbk_iuser[ce_index][0]), GetRGB_B(tbk_iuser[ce_index][0]), GetRGBA_A(tbk_iuser[ce_index][0]));

            return Plugin_Continue;
        }			


/*  Spin Back Bullet  */
    /*
    * 在{origin}生成直線移動，
    * 在{delay}秒後改向生成位置移動的彈幕
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 彈幕數
    * @param angle_offset 生成時運動角度偏差
    * @param spawn_radius 生成時與{origin}距離
    * @param spd 初速
    * @param comeback_spd 向生成位置移動時速度
    * @param comeback_radius 目的地半徑偏差
    * @param comeback_angle_shift 目的地半徑角度偏差
    * @param delay 運動變化時間
    * @param last_time 彈幕維持時間
    * @param colors 彈幕顏色
    * @param end_colors 運動變化後顏色
    * @param damage 彈幕傷害
    *
    */ 
    stock SpinBackBullet(const String:bullet_model[], Float:origin[3], owner=0, bullet_amount=16, Float:angle_offset=0.0, Float:spawn_radius=50.0, 
                                Float:spd=200.0, Float:comeback_spd=250.0, 
                                Float:comeback_radius=100.0, Float:comeback_angle_shift=45.0,
                                Float:delay=3.0, Float:last_time=8.0,
                                colors[4]={204,0,102,255}, 
                                end_colors[4]={0,102,204,255},
                                Float:damage=DEFAULT_BULLET_DMG)
    {
        new Float:spawn_origin[3], 
            Float:dest_origin[3], 
            Float:angle=ToRadian(angle_offset);
        
        new i=0;
        
        comeback_angle_shift=ToRadian(comeback_angle_shift);

        for(; i<bullet_amount; i++)
        {
            spawn_origin[0] = origin[0] + spawn_radius * Cosine(angle);
            spawn_origin[1] = origin[1] + spawn_radius *   Sine(angle);
            spawn_origin[2] = origin[2];

            dest_origin[0] = origin[0] + comeback_radius * Cosine(angle + comeback_angle_shift);
            dest_origin[1] = origin[1] + comeback_radius *   Sine(angle + comeback_angle_shift);
            dest_origin[2] = origin[2];

            ChangeDestBullet(.bullet_model=bullet_model
                        ,.origin=spawn_origin
                        ,.dest_origin=dest_origin
                        ,.owner=owner
                        ,.angle=angle
                        ,.start_spd=spd
                        ,.end_spd=comeback_spd
                        ,.delay=delay
                        ,.last_time=last_time
                        ,.colors=colors
                        ,.end_colors=end_colors
                        ,.damage=damage);

            angle += M_2PI / bullet_amount;
        }
    }

/*  Shape Drawing Bullet  */


    /*
    * 在{origin}生成直線移動，
    * 以畫{side_amount}邊形的方式移動{draw_side}/{side_amount}次
    * 例如 side_amount=4, draw_side=12 時就畫3次正方形
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 彈幕數
    * @param side_amount 運動時圖形的邊數
    * @param side_distance 每一邊長度
    * @param spd 運動速度
    * @param speed_diff 每次轉邊時速度變化
    * @param draw_side 總邊數
    * @param colors 彈幕顏色
    * @param colors_increase 每次角度變化時彈幕顏色變化
    * @param damage 彈幕傷害
    *
    */ 
    stock ShapeDrawingBullet(const String:bullet_model[], Float:origin[3], owner=0, Float:angle_offset=0.0, bullet_amount=8, side_amount=4,
                                    Float:side_distance=400.0, Float:spd=200.0, Float:speed_diff=0.0, draw_side=8,
                                    colors[4]={100,200,55,255}, colors_increase[4]={25,50,0,0},
                                    Float:damage=DEFAULT_BULLET_DMG)
    {
        new Float:angle_diff=M_FULL_ANGLE/side_amount,      //Angle diff per phase
            Float:switch_delay=side_distance/spd,
            Float:last_time=switch_delay*(draw_side)-0.1;		

        for(new i=0; i<bullet_amount; i++)
        {
            ChangeAngleBullet(.bullet_model=bullet_model
                        ,.origin=origin
                        ,.owner=owner
                        ,.initial_angle=angle_offset
                        ,.angle_change_per_phase=angle_diff
                        ,.spd=spd
                        ,.speed_diff=speed_diff
                        ,.switch_delay=switch_delay
                        ,.last_time=last_time
                        ,.colors=colors
                        ,.colors_increase=colors_increase
                        ,.damage=damage
                        );

            angle_offset += M_FULL_ANGLE/bullet_amount;
        }
    }

/*  Split Straight Bullet  */


    /*
    * 在{origin}生成直線移動的彈幕。
    * 該彈幕在{split_interval}後分裂成{total_split_direction}個方向，每個方向{total_split}粒彈幕
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 彈幕數
    * @param spd 初速
    * @param angle_offset 生成時角度偏差
    * @param angle_difference_min 每粒母體生成角度偏差最小值
    * @param angle_difference_max 每粒母體生成角度偏差最大值(均分時請和angle_difference_min分配同樣值)
    * @param split_bullet_max_spd 分裂後彈幕最大速度
    * @param total_split 每個方向分裂數
    * @param total_split_direction 分裂方向數
    * @param split_bullet_last_time 分裂彈幕維持時間
    * @param split_angle_offset 分裂彈幕生成時與運動角度偏差
    * @param split_interval 分裂時間
    * @param colors 彈幕顏色
    * @param split_colors 分裂彈幕顏色
    * @param damage 母體彈幕傷害
    * @param split_damage 分裂後彈幕傷害
    *
    */ 
    stock SplitStraightBullet(const String:bullet_model[], Float:origin[3], owner=0, bullet_amount=3, Float:spd=150.0,
        Float:angle_offset=0.0, Float:angle_difference_min=45.0, Float:angle_difference_max=135.0, 
        Float:split_bullet_max_spd=300.0, total_split=5, total_split_direction=4, Float:split_bullet_last_time=7.0, 
        Float:split_angle_offset=0.0, Float:split_interval=5.0,
        colors[4]={255,51,255,255}, split_colors[4]={255,51,255,255}, Float:damage=20.0, Float:split_damage=DEFAULT_BULLET_DMG)
    {
        new i=0, ce_index;
        new Float:angle = ToRadian(angle_offset), Float:velocity[3];

        angle_difference_min = ToRadian(angle_difference_min);
        angle_difference_max = ToRadian(angle_difference_max);
        bullet_amount = (bullet_amount<=5)?bullet_amount:5;
        split_angle_offset = ToRadian(split_angle_offset);

        for(; i<bullet_amount; i++, angle += GetRandomFloat(angle_difference_min, angle_difference_max))
        {
            ce_index=CreateTsubakiEntity(SPLIT_STRAIGHT_BULLET
                                    ,bullet_model
                                    ,.solid_type=DEFAULT_BULLET_SOLID
                                    ,.movetype=MOVETYPE_NOCLIP
                                    ,.owner=owner
                                    ,.r=colors[0]
                                    ,.g=colors[1]
                                    ,.b=colors[2]
                                    ,.a=colors[3]
                                    ,.min_size=BULLET_MIN_SIZE
                                    ,.max_size=BULLET_MAX_SIZE);

            if(ce_index == -1)
                continue;

            velocity[0] = spd * Cosine(angle);
            velocity[1] = spd *   Sine(angle);
            velocity[2] = 0.0;

            tbk_iuser[ce_index][0] = total_split;
            tbk_iuser[ce_index][1] = total_split_direction;
            tbk_iuser[ce_index][2] = CreateRGBAIntByArray(split_colors);

            tbk_fuser[ce_index][0] = split_bullet_max_spd;
            tbk_fuser[ce_index][1] = split_bullet_last_time;
            tbk_fuser[ce_index][2] = split_angle_offset;
            tbk_fuser[ce_index][3] = split_damage;
            
            TeleportEntity(g_iCustomEntityRef[ce_index], origin, .velocity=velocity);
            SetTsubakiEntityDamage(ce_index, damage);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);

            SetEntityNextThink(ce_index, split_interval, SplitStraightBulletThink, TIMER_FLAG_NO_MAPCHANGE);
        }
    }

        public Action SplitStraightBulletThink(Handle timer, int ce_index)
        {
            new owner = GetEntityOwner(g_iCustomEntityRef[ce_index]), r=GetRGB_R(tbk_iuser[ce_index][2]), g=GetRGB_G(tbk_iuser[ce_index][2]), b=GetRGB_B(tbk_iuser[ce_index][2]), a=GetRGBA_A(tbk_iuser[ce_index][2]), i=0, j=0, sub_ce_index, String:bullet_model[128];
            new Float:spd = tbk_fuser[ce_index][0], Float:split_bullet_max_spd=tbk_fuser[ce_index][0], Float:split_bullet_last_time = tbk_fuser[ce_index][1], Float:angle, Float:origin[3], Float:velocity[3], Float:damage=GetTsubakiEntityDamage(ce_index);
            
            GetEntityModel(g_iCustomEntityRef[ce_index], bullet_model);
            GetEntityOrigin(g_iCustomEntityRef[ce_index], origin);

            for(; i<tbk_iuser[ce_index][0]; i++)
            {
                angle = tbk_fuser[ce_index][2];			//angle = angle_offset

                for(j=0; j<tbk_iuser[ce_index][1]; j++, angle += M_2PI / tbk_iuser[ce_index][1])
                {
                    sub_ce_index=CreateTsubakiEntity(NORMAL_BULLET
                                                ,bullet_model
                                                ,.solid_type=DEFAULT_BULLET_SOLID
                                                ,.movetype=MOVETYPE_NOCLIP
                                                ,.owner=owner
                                                ,.r=r
                                                ,.g=g
                                                ,.b=b
                                                ,.a=a
                                                ,.min_size=BULLET_MIN_SIZE
                                                ,.max_size=BULLET_MAX_SIZE);

                    if(sub_ce_index == -1)
                        continue;

                    velocity[0] = spd * Cosine(angle);
                    velocity[1] = spd *   Sine(angle);
                    velocity[2] = 0.0;

                    TeleportEntity(g_iCustomEntityRef[sub_ce_index], origin, .velocity=velocity);
                    SetTsubakiEntityDamage(sub_ce_index, damage);
                    SDKHook(g_iCustomEntityRef[sub_ce_index], SDKHook_Touch, NormalBulletTouch);

                    SetRemoveEntityTask(sub_ce_index, split_bullet_last_time);
                }
                
                spd -= split_bullet_max_spd / tbk_iuser[ce_index][0];
            }
            ResetEntityThinkHandle(ce_index);
            RemoveTsubakiEntity(INVALID_HANDLE, ce_index);
            return Plugin_Stop;
        }

    /*
    * 在{origin}生成直線移動的彈幕。
    * 該彈幕在每{phase_interval}秒後隱形，然後每{phase_interval}秒重現，LOOP
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 彈幕數
    * @param spd 初速
    * @param angle_offset 生成時角度偏差
    * @param angle_difference_min 每粒母體生成角度偏差最小值
    * @param angle_difference_max 每粒母體生成角度偏差最大值(均分時請和angle_difference_min分配同樣值)
    * @param last_time 彈幕維持時間
    * @param phase_interval 每N秒進行一次變化
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    */ 
    stock AppearAndDisapperBullet(const String:bullet_model[], Float:origin[3], owner=0, bullet_amount=3, Float:spd=150.0, 
        Float:angle_offset=0.0, Float:angle_difference_min=120.0, Float:angle_difference_max=120.0,
        Float:last_time=8.0, Float:phase_interval=1.0, 
        colors[4]={150,205,25,255}, Float:damage=DEFAULT_BULLET_DMG)
    {
        new i=0, ce_index;
        new Float:velocity[3], Float:angle=angle_offset;

        angle_difference_min = ToRadian(angle_difference_min);
        angle_difference_max = ToRadian(angle_difference_max);
        
        for(; i<bullet_amount; i++, angle += GetRandomFloat(angle_difference_min, angle_difference_max))
        {
            ce_index=CreateTsubakiEntity(APPEAR_AND_DISAPPER_BULLET
                                    ,bullet_model
                                    ,.solid_type=DEFAULT_BULLET_SOLID
                                    ,.movetype=MOVETYPE_NOCLIP
                                    ,.owner=owner
                                    ,.r=colors[0]
                                    ,.g=colors[1]
                                    ,.b=colors[2]
                                    ,.a=colors[3]
                                    ,.min_size=BULLET_MIN_SIZE
                                    ,.max_size=BULLET_MAX_SIZE);

            if(ce_index == -1)
                continue;
            
            tbk_iuser[ce_index][0] = 0;

            tbk_fuser[ce_index][0] = phase_interval;

            velocity[0] = spd * Cosine(angle);
            velocity[1] = spd *   Sine(angle);
            velocity[2] = 0.0;

            TeleportEntity(g_iCustomEntityRef[ce_index], origin, .velocity=velocity);
            SetTsubakiEntityDamage(ce_index, damage);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);

            SetEntityNextThink(ce_index, phase_interval, AppearAndDisapperBulletThink, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
            SetRemoveEntityTask(ce_index, last_time);
        }		

    }
        
        public Action AppearAndDisapperBulletThink(Handle timer, int ce_index)
        {
            switch(tbk_iuser[ce_index][0]++ % 2)
            {	
                case 0:			//Disapper
                {
                    SetEntProp(g_iCustomEntityRef[ce_index], Prop_Send, m_usSolidFlags, FSOLID_NOT_SOLID);
                    SetEntityRenderMode(g_iCustomEntityRef[ce_index], RENDER_NONE);
                }
                case 1:			//Appear
                {
                    SetEntProp(g_iCustomEntityRef[ce_index], Prop_Send, m_usSolidFlags, DEFAULT_BULLET_SOLID);
                    SetEntityRenderMode(g_iCustomEntityRef[ce_index], RENDER_TRANSALPHA);
                }
            }

            return Plugin_Continue;
        }

/* Split Backward Bullet */

    /*
    * 生成直線移動的彈幕
    * 該彈粒每{split_interval}秒從移動角度相差{split_base_angle}度生成{toatl_split_direction}粒彈幕
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 彈幕數
    * @param bullet_spd 母體速度
    * @param angle_offset 母體生成時角度偏差
    * @param angle_diff_min 每個母體生成角度差距最小值
    * @param angle_diff_max 每個母體生成角度差距最大值
    * @param split_interval 每次分裂間隔時間
    * @param split_bullet_spd 分裂彈幕的移動速度
    * @param total_split_direction 分裂數
    * @param split_base_angle 最初分裂的角度與母體運動角度差距
    * @param angle_per_each_split 每個分裂彈幕生成角度差距最大值
    * @param bullet_last_time 母體存在時間
    * @param split_bullet_last_time 分裂彈幕存在時間
    * @param last_distance 彈幕維持距離
    * @param colors 彈幕顏色
    * @param split_colors 分裂彈幕顏色
    * @param damage 彈幕傷害
    *
    */ 
    stock SplitBackwardBullet(const String:bullet_model[], Float:origin[3], owner=0, bullet_amount=3,
                                Float:bullet_spd=150.0,
                                Float:angle_offset=0.0, Float:angle_diff_min=15.0, Float:angle_diff_max=70.0, 
                                Float:split_interval=2.0,  
                                Float:split_bullet_spd=250.0, total_split_direction=4, 
                                Float:split_base_angle=90.0, Float:angle_per_each_split = 60.0,
                                Float:bullet_last_time=7.0, Float:split_bullet_last_time=5.0,
                                colors[4]={0,204,0,255}, split_colors[4]={255,51,255,255}, Float:damage=DEFAULT_BULLET_DMG)
    {
        new i=0, ce_index;
        new Float:velocity[3];

        angle_offset = ToRadian(angle_offset);
        split_base_angle = ToRadian(split_base_angle);
        angle_per_each_split = ToRadian(angle_per_each_split);
        angle_diff_min = ToRadian(angle_diff_min);
        angle_diff_max = ToRadian(angle_diff_max);

        bullet_amount = Clamp(bullet_amount, 1, 8);
        total_split_direction = Clamp(total_split_direction, 1, 5);

        for(; i<bullet_amount; i++, angle_offset+=GetRandomFloat(angle_diff_min, angle_diff_max))
        {
            ce_index = CreateTsubakiEntity(SPLIT_BACKWARD_BULLET
                                        ,bullet_model
                                        ,.solid_type=DEFAULT_BULLET_SOLID
                                        ,.movetype=MOVETYPE_NOCLIP
                                        ,.owner=owner
                                        ,.r=colors[0]
                                        ,.g=colors[1]
                                        ,.b=colors[2]
                                        ,.a=colors[3]
                                        ,.min_size=BULLET_MIN_SIZE
                                        ,.max_size=BULLET_MAX_SIZE);
            
            if(ce_index == -1)
                continue;

            velocity[0] = bullet_spd * Cosine(angle_offset);
            velocity[1] = bullet_spd *   Sine(angle_offset);

            tbk_iuser[ce_index][0] = total_split_direction;
            tbk_iuser[ce_index][1] = CreateRGBAIntByArray(split_colors);

            tbk_fuser[ce_index][0] = split_bullet_spd;
            tbk_fuser[ce_index][1] = split_bullet_last_time;
            tbk_fuser[ce_index][2] = angle_offset + split_base_angle;
            tbk_fuser[ce_index][3] = angle_per_each_split;
            tbk_fuser[ce_index][4] = split_interval;

            TeleportEntity(g_iCustomEntityRef[ce_index], origin, .velocity=velocity);
            SetTsubakiEntityDamage(ce_index, damage);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);

            SetEntityNextThink(ce_index, split_interval, SplitBackwardBulletThink, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
            SetRemoveEntityTask(ce_index, bullet_last_time);
        }
    }

        public Action SplitBackwardBulletThink(Handle timer, ce_index)
        {
            new i=0, sub_ce_index, r=GetRGB_R(tbk_iuser[ce_index][1]), g=GetRGB_G(tbk_iuser[ce_index][1]), b=GetRGB_B(tbk_iuser[ce_index][1]), a=GetRGBA_A(tbk_iuser[ce_index][1]), total_split_direction=tbk_iuser[ce_index][0], owner=GetEntityOwner(g_iCustomEntityRef[ce_index]);
            new Float:origin[3], Float:velocity[3], Float:angle=tbk_fuser[ce_index][2], Float:speed = tbk_fuser[ce_index][0], Float:last_time = tbk_fuser[ce_index][1], Float:damage=GetTsubakiEntityDamage(ce_index);
            new String:bullet_model[128];
            GetEntityModel(g_iCustomEntityRef[ce_index], bullet_model);
            GetEntityOrigin(g_iCustomEntityRef[ce_index], origin);

            for(; i<total_split_direction; i++)
            {
                sub_ce_index = CreateTsubakiEntity(NORMAL_BULLET
                                                ,bullet_model
                                                ,.solid_type=DEFAULT_BULLET_SOLID
                                                ,.movetype=MOVETYPE_NOCLIP
                                                ,.owner=owner
                                                ,.r=r
                                                ,.g=g
                                                ,.b=b
                                                ,.a=a
                                                ,.min_size=BULLET_MIN_SIZE
                                                ,.max_size=BULLET_MAX_SIZE);
                
                if( sub_ce_index==-1 )
                    continue;

                velocity[0] = speed * Cosine(angle);
                velocity[1] = speed *   Sine(angle);

                TeleportEntity(g_iCustomEntityRef[sub_ce_index], origin, .velocity=velocity);
                SetTsubakiEntityDamage(sub_ce_index, damage);
                SDKHook(g_iCustomEntityRef[sub_ce_index], SDKHook_Touch, NormalBulletTouch);

                SetRemoveEntityTask(sub_ce_index, last_time);

                angle += tbk_fuser[ce_index][3];
            }

            return Plugin_Continue;
        }


    /*
    * 從{origin}發射觸碰後會爆炸的彈幕
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 彈幕數
    * @param bullet_spd 速度
    * @param angle_offset 生成時角度偏差
    * @param last_time 彈幕存在時間
    * @param damage_radius 傷害半徑，傷害計算式為 damage * (radius-distance) / radius
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    */ 
    stock ExplosiveBullet(const String:bullet_model[], explosion_sprite_id, Float:origin[3], owner=0, bullet_amount=3, Float:bullet_spd=150.0,
                            Float:angle_offset=0.0, Float:last_time=7.0, 
                            Float:damage_radius=300.0, colors[4]={200,0,0,255}, Float:damage=35.0, const String:explosive_bullet_sound[]="")
    {
        if(EXPLOSIVE_BULLET_SOUND[0]==0 && strlen(explosive_bullet_sound))
        {
            FormatEx(EXPLOSIVE_BULLET_SOUND, sizeof(EXPLOSIVE_BULLET_SOUND), explosive_bullet_sound);
        }

        new i=0, ce_index;
        new Float:velocity[3];

        angle_offset = ToRadian(angle_offset);

        for(; i<bullet_amount; i++)
        {
            ce_index=CreateTsubakiEntity(EXPLOSIVE_BULLET
                                    ,bullet_model
                                    ,.solid_type=DEFAULT_BULLET_SOLID
                                    ,.movetype=MOVETYPE_NOCLIP
                                    ,.owner=owner
                                    ,.r=colors[0]
                                    ,.g=colors[1]
                                    ,.b=colors[2]
                                    ,.a=colors[3]
                                    ,.min_size=BULLET_MIN_SIZE
                                    ,.max_size=BULLET_MAX_SIZE);
            
            if(ce_index==-1)
                continue;
                    
            tbk_iuser[ce_index][0] = 0;
            tbk_iuser[ce_index][1] = CreateRGBAIntByArray(colors);
            tbk_iuser[ce_index][2] = explosion_sprite_id;

            tbk_fuser[ce_index][0] = damage_radius;
            tbk_fuser[ce_index][1] = damage;

            velocity[0] = bullet_spd * Cosine(angle_offset);
            velocity[1] = bullet_spd *   Sine(angle_offset);
            
            TeleportEntity(g_iCustomEntityRef[ce_index], origin, .velocity=velocity);
            SetTsubakiEntityDamage(ce_index, damage);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, OnExplosiveBulletTouch);

            SetEntityNextThink(ce_index, EXPLOSIVE_BULLET_THINKTIME, ExplosiveBulletThink, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
            SetRemoveEntityTask(ce_index, last_time);
        
            angle_offset += M_2PI / bullet_amount;
        }
    }

        public Action ExplosiveBulletThink(Handle timer, int ce_index)
        {
            switch((++tbk_iuser[ce_index][0])%2)
            {
                case 1:SetEntityRenderColor(g_iCustomEntityRef[ce_index], 255, 255, 255, 255);
                case 0:SetEntityRenderColor(g_iCustomEntityRef[ce_index], GetRGB_R(tbk_iuser[ce_index][1]), GetRGB_G(tbk_iuser[ce_index][1]), GetRGB_B(tbk_iuser[ce_index][1]), GetRGBA_A(tbk_iuser[ce_index][1]));
            }

            return Plugin_Continue;
        }
        
        public OnExplosiveBulletTouch(ent, target)
        {
            static owner, ce_index;
            owner=GetEntityOwner(ent);

            if(IsClientValid(target) && target!=owner)
            {
                ce_index = EntRefToCustomEntityIndex(EntIndexToEntRef(ent));
                if(ce_index == -1) {
                    RemoveEntity(ent);
                    return;
                }

                new i=1;
                new Float:target_origin[3], Float:origin[3], Float:distance, Float:max_radius=tbk_fuser[ce_index][0], Float:damage=GetTsubakiEntityDamage(ce_index);
                GetEntityOrigin(ent, origin);
                for(; i<=MaxClients; i++)
                {
                    if( IsClientInGame(i) && IsPlayerAlive(i) && (owner==0 || (GetClientTeam(owner)!=GetClientTeam(i))) )
                    {
                        GetEntityOrigin(i, target_origin);
                        distance = GetVectorDistance(origin, target_origin);
                        if(distance < max_radius) {
                            SDKHooks_TakeDamage(i, ent, owner, damage*(max_radius-distance)/max_radius, DMG_BLAST|DMG_ALWAYSGIB, .bypassHooks=false);
                        }
                    }
                }
                TBKCreateExplosionTE(origin, tbk_iuser[ce_index][2]);
                if(EXPLOSIVE_BULLET_SOUND[0]!=0) {
                    EmitSoundToAll(EXPLOSIVE_BULLET_SOUND, ent, SNDCHAN_WEAPON, SNDLEVEL_NONE, SND_NOFLAGS, 0.45, SNDPITCH_NORMAL, 0);
                }
                RemoveTsubakiEntity(INVALID_HANDLE, ce_index);
            }
        }


/*  Swirl Bullet  */

    /*
    * 從{origin}生成轉一圈回到{origin}的彈幕
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param owner 主人
    * @param bullet_amount 彈幕數
    * @param angle_offset 生成時角度偏差
    * @param bullet_spd 速度
    * @param angular_spd 角速度(degree)
    * @param last_time 彈幕存在時間
    * @param damage 傷害半徑，傷害計算式為 damage * (radius-distance) / radius
    * @param colors 彈幕顏色
    * @param damage 彈幕傷害
    *
    */ 
    stock SwirlBullet(const String:bullet_model[], Float:origin[3], owner=0, bullet_amount=6, 
                        Float:angle_offset=0.0, Float:spd=200.0, Float:angular_spd=60.0, 
                        Float:last_time=12.0, colors[4]={255,20,147,255}, Float:damage=6.0)
    {
        new ce_index, i=0;
        new Float:velocity[3];
        angle_offset = ToRadian(angle_offset);

        for(; i<bullet_amount; i++)
        {
            ce_index=CreateTsubakiEntity(SWIRL_BULLET
                                    ,bullet_model
                                    ,.solid_type=DEFAULT_BULLET_SOLID
                                    ,.movetype=MOVETYPE_NOCLIP
                                    ,.owner=owner
                                    ,.r=colors[0]
                                    ,.g=colors[1]
                                    ,.b=colors[2]
                                    ,.a=colors[3]
                                    ,.min_size=BULLET_MIN_SIZE
                                    ,.max_size=BULLET_MAX_SIZE);
            
            if(ce_index == -1)
                continue;
                
            velocity[0] = spd * Cosine(angle_offset);
            velocity[1] = spd *   Sine(angle_offset);

            tbk_fuser[ce_index][0] = spd;
            tbk_fuser[ce_index][1] = angle_offset;
            tbk_fuser[ce_index][2] = DEFAULT_BULLET_THINKTIME * ToRadian(angular_spd);

            //For Think velocity
            tbk_vuser[ce_index][0][2] = 0.0;

            TeleportEntity(g_iCustomEntityRef[ce_index], origin, .velocity=velocity);
            SetTsubakiEntityDamage(ce_index, damage);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);

            SetEntityNextThink(ce_index, DEFAULT_BULLET_THINKTIME, SwirlBulletThink, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
            SetRemoveEntityTask(ce_index, last_time);

            angle_offset += M_2PI/bullet_amount;
        }
    }

        public Action SwirlBulletThink(Handle timer, int ce_index)
        {
            tbk_vuser[ce_index][0][0] = tbk_fuser[ce_index][0] * Cosine(tbk_fuser[ce_index][1]);
            tbk_vuser[ce_index][0][1] = tbk_fuser[ce_index][0] *   Sine(tbk_fuser[ce_index][1]);

            tbk_fuser[ce_index][1] += tbk_fuser[ce_index][2];
            TeleportEntity(g_iCustomEntityRef[ce_index], .velocity=tbk_vuser[ce_index][0]);

            return Plugin_Continue;
        }

/* Tracer Bullet */

    /*
    * 從{origin}生成隨機移動<->休息
    * 最後向{target}移動的彈幕
    *
    * @param bullet_model 彈幕模組
    * @param origin 生成位置
    * @param target 目標
    * @param owner 主人
    * @param bullet_amount 彈幕數
    * @param stage 移動<->休息的次數
    * @param min_radius 隨機移動時，目的地距離生成位置的最小半徑
    * @param max_radius 隨機移動時，目的地距離生成位置的最大半徑
    * @param switch=delay 速度
    * @param last_time 彈幕存在時間
    * @param damage 彈幕傷害
    * @param colors 彈幕顏色
    * @param colors_final 向目標移動時彈幕顏色
    * @param damage 彈幕傷害
    *
    */ 
    stock TracerBullet(const String:bullet_model[], Float:origin[3], target, owner=0, bullet_amount=6, stages=2,
                        Float:min_radius=250.0, Float:max_radius=400.0, 
                        Float:switch_delay=2.0, Float:last_time=3.0,
                        colors[4]={0,102,0,255}, colors_final[4]={255,185,15,255}, Float:damage=7.0)
    {
        new ce_index, i, int_colors=CreateRGBAIntByArray(colors_final);
        new Float:spd, Float:velocity[3]={0.0, ...}, Float:angle;

        for(; i<bullet_amount; i++)
        {
            ce_index=CreateTsubakiEntity(TRACER_BULLET
                                    ,bullet_model
                                    ,.solid_type=DEFAULT_BULLET_SOLID
                                    ,.movetype=MOVETYPE_NOCLIP
                                    ,.owner=owner
                                    ,.r=colors[0]
                                    ,.g=colors[1]
                                    ,.b=colors[2]
                                    ,.a=colors[3]
                                    ,.min_size=BULLET_MIN_SIZE
                                    ,.max_size=BULLET_MAX_SIZE);
            
            if(ce_index == -1)
                continue;

            angle = GetRandomFloat(0.0, M_2PI);
            spd = GetRandomFloat(max_radius, max_radius) / switch_delay;

            velocity[0] = spd * Cosine(angle);
            velocity[1] = spd *   Sine(angle);

            tbk_iuser[ce_index][0] = 0;
            tbk_iuser[ce_index][1] = 2*stages;
            tbk_iuser[ce_index][2] = int_colors;
            tbk_iuser[ce_index][3] = target;

            tbk_fuser[ce_index][0] = angle + GetRandomFloat(M_PI/2.0, M_PI*3.0/2.0);
            tbk_fuser[ce_index][1] = min_radius;
            tbk_fuser[ce_index][2] = max_radius;
            tbk_fuser[ce_index][3] = switch_delay;
            tbk_fuser[ce_index][4] = last_time;


            //Used for velocity
            tbk_vuser[ce_index][1][0] = 0.0;
            tbk_vuser[ce_index][1][1] = 0.0;
            tbk_vuser[ce_index][1][2] = 0.0;

            CopyVector(tbk_vuser[ce_index][0], origin);
            
            TeleportEntity(g_iCustomEntityRef[ce_index], origin, .velocity=velocity);
            SetTsubakiEntityDamage(ce_index, damage);
            SDKHook(g_iCustomEntityRef[ce_index], SDKHook_Touch, NormalBulletTouch);

            SetEntityNextThink(ce_index, switch_delay, TracerBulletThink, TIMER_FLAG_NO_MAPCHANGE);
            SetRemoveEntityTask(ce_index, (1+stages*2)*switch_delay+last_time);
        }
    }

        public Action TracerBulletThink(Handle timer, int ce_index)
        {
            if(tbk_iuser[ce_index][0] >= tbk_iuser[ce_index][1]-1)
            {
                //Trace Player
                if(IsValidEntity(tbk_iuser[ce_index][3]))
                {
                    GetClientAbsOrigin(tbk_iuser[ce_index][3], tbk_vuser[ce_index][0]);
                    tbk_vuser[ce_index][0][2] += 40.0;
                }

                SetEntityRenderColor(g_iCustomEntityRef[ce_index], GetRGB_R(tbk_iuser[ce_index][2]), GetRGB_G(tbk_iuser[ce_index][2]), GetRGB_B(tbk_iuser[ce_index][2]), GetRGBA_A(tbk_iuser[ce_index][2]));
                GetEntityOrigin(g_iCustomEntityRef[ce_index], tbk_vuser[ce_index][2]);
                tbk_fuser[ce_index][4] = GetVectorDistance(tbk_vuser[ce_index][0], tbk_vuser[ce_index][2])*2.0/tbk_fuser[ce_index][4];			//Speed
                SubtractVectors(tbk_vuser[ce_index][0], tbk_vuser[ce_index][2], tbk_vuser[ce_index][0]);
                GetVectorAngles(tbk_vuser[ce_index][0], tbk_vuser[ce_index][2]);
                tbk_vuser[ce_index][2][1] = ToRadian(tbk_vuser[ce_index][2][1]);

                tbk_vuser[ce_index][0][0] = tbk_fuser[ce_index][4] * Cosine(tbk_vuser[ce_index][2][1]);
                tbk_vuser[ce_index][0][1] = tbk_fuser[ce_index][4] *   Sine(tbk_vuser[ce_index][2][1]);

                TeleportEntity(g_iCustomEntityRef[ce_index], .velocity=tbk_vuser[ce_index][0]);
                ResetEntityThinkHandle(ce_index);

            }
            else
            {
                if(++tbk_iuser[ce_index][0]%2)
                {
                    tbk_vuser[ce_index][1][0] = 0.0;
                    tbk_vuser[ce_index][1][1] = 0.0;
                    tbk_vuser[ce_index][1][2] = 0.0;
                    
                }
                else
                {
                    new Float:spd = GetRandomFloat(tbk_fuser[ce_index][1], tbk_fuser[ce_index][2]) / tbk_fuser[ce_index][3];		//Speed
                    tbk_vuser[ce_index][1][0] = spd * Cosine(tbk_fuser[ce_index][0]);
                    tbk_vuser[ce_index][1][1] = spd *   Sine(tbk_fuser[ce_index][0]);
                    tbk_fuser[ce_index][0] = tbk_fuser[ce_index][0] + GetRandomFloat(M_PI/2.0, M_PI*3.0/2.0);
                }

                SetEntityNextThink(ce_index, tbk_fuser[ce_index][3], TracerBulletThink, TIMER_FLAG_NO_MAPCHANGE);
                TeleportEntity(g_iCustomEntityRef[ce_index], .velocity=tbk_vuser[ce_index][1]);
            }
            
            return Plugin_Stop;
        }

#pragma semicolon 0