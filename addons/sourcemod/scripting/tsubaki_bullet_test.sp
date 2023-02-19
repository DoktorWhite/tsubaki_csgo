#include <tsubaki_v2/tsubaki_common>
#include <tsubaki_v2/tsubaki_bullet>

stock char LAUNCHER_MODEL[] = "models/blackout.mdl";
stock char BULLET_MODEL[] = "models/knastjunkies/soccerball.mdl";

stock int EXPLOSION_SPRITE_ID;
stock char EXPLOSION_SOUND[] = "tsubaki/effects/explosion.mp3";

stock int LASER_BEAM_SPRITE_ID;

stock float gfLastTest[MAXPLAYERS];

public Plugin myinfo = 
{
    name = "Bullet Test",
    author = "WhiteCola",
    description = "Kano-Bi CSGO Version",
    version = "0.0",
    url = ""
}

public void OnPluginStart() {

    PrecacheModel(LAUNCHER_MODEL);
    PrecacheModel(BULLET_MODEL);
    AddFileToDownloadsTable("models/knastjunkies/soccerball.mdl");
    AddFileToDownloadsTable("models/knastjunkies/SoccerBall.dx90.vtx");
    AddFileToDownloadsTable("models/knastjunkies/SoccerBall.phy");
    AddFileToDownloadsTable("models/knastjunkies/soccerball.vvd");
    AddFileToDownloadsTable("materials/knastjunkies/Material__0.vmt");
    AddFileToDownloadsTable("materials/knastjunkies/Material__1.vmt");
    
    new String:file_path[256];
    FormatEx(file_path, sizeof(file_path), "sound/%s", EXPLOSION_SOUND);
    AddFileToDownloadsTable(file_path);
    PrecacheSound(EXPLOSION_SOUND);

    EXPLOSION_SPRITE_ID = PrecacheModel("models/sprites/sprite_fire01.vmt");		
    AddFileToDownloadsTable("models/sprites/sprite_fire01.vmt");
    AddFileToDownloadsTable("models/sprites/sprite_fire01.vtf");

    LASER_BEAM_SPRITE_ID = PrecacheModel("materials/sprites/laserbeam.vmt");
    AddFileToDownloadsTable("materials/sprites/laserbeam.vmt");

    RegConsoleCmd("test", TestFx);
    RegConsoleCmd("hp", TestAddHp);
    RegConsoleCmd("respawn", TestRespawn);
}

public OnMapStart() {
    for(new i=0; i<MAX_ENTITY; i++) {
        g_hEntityRemoveTask[i] = g_hEntityThinkTask[i] = INVALID_HANDLE;
    }

    ServerCommand("bot_kick");
}

public OnClientDisconnect_Post(int client) {
    gfLastTest[client] = 0.0;
}

public Action CS_OnTerminateRound() {
    return Plugin_Stop;
}

public Action TestFx(int client, int args) {
    DisplayDebugBulletSkillMenu(client, 0);

    return Plugin_Handled;
}

public Action TestRespawn(int client, int args) {
    if(!IsPlayerAlive(client)) {
        CS_RespawnPlayer(client);
    }
    return Plugin_Handled;
}

public Action TestAddHp(int client, int args) {

    SetEntityHealth(client, GetCmdArgInt(1));

    return Plugin_Handled;
}

DisplayDebugBulletSkillMenu(int client, int item) {
    static Menu menu;

    if(menu == null) {
        menu = new Menu(DebugBulletSkillMenu, MenuAction_Select);

        menu.SetTitle("【DEBUG】弾幕パタン");
        menu.AddItem(EMPTY, "AverageBullet");
        menu.AddItem(EMPTY, "FireNonPenetrateBullet");
        menu.AddItem(EMPTY, "AverageChangeSpeedBullet");
        menu.AddItem(EMPTY, "MoveAndSpinOnlyBullet");
        menu.AddItem(EMPTY, "SpinAwayBullet");
        menu.AddItem(EMPTY, "SpinAwaySplitBullet");
        menu.AddItem(EMPTY, "FullRandomBullet");
        menu.AddItem(EMPTY, "AllRandomMachineBullet");
        menu.AddItem(EMPTY, "RotateMachineBulletLauncherThink");
        menu.AddItem(EMPTY, "LaunchSlowChangeDirBullet");
        menu.AddItem(EMPTY, "RoundSpinBullet");
        menu.AddItem(EMPTY, "SpinBackBullet");
        menu.AddItem(EMPTY, "ShapeDrawingBullet");
        menu.AddItem(EMPTY, "SplitStraightBullet");
        menu.AddItem(EMPTY, "AppearAndDisapperBullet");
        menu.AddItem(EMPTY, "SplitBackwardBullet");
        menu.AddItem(EMPTY, "ExplosiveBullet");
        menu.AddItem(EMPTY, "SwirlBullet");
        menu.AddItem(EMPTY, "TracerBullet");
    }

    menu.DisplayAt(client, (item/6)*6, MENU_TIME_FOREVER);
}

    public int DebugBulletSkillMenu(Menu menu, MenuAction action, int client, int key) {
        if(action == MenuAction_Select) {
            if(gfLastTest[client] >= GetGameTime())
            {
                SetHudTextParams(-1.0, 0.6, 0.5, 255, 0, 0, 255, 1, 0.0, 0.0, 0.0);
                ShowHudText(client, 1, "等等");
                DisplayDebugBulletSkillMenu(client, key);
                return 1;
            }

            gfLastTest[client] = GetGameTime() + 0.2;

            float origin[3];
            GetPlayerAimOrigin(client, origin);
            origin[2] += 50.0;

            switch(key) {
                case 0:AverageBullet(BULLET_MODEL, origin);
                case 1:{
                    float angles[3];
                    GetClientAbsOrigin(client, origin);
                    origin[2] += 40.0;
                    GetClientEyeAngles(client, angles);
                    angles[0] *= -1.0;
                    FireNonPenetrateBullet(BULLET_MODEL, origin, angles, .owner=client);
                }
                case 2:AverageChangeSpeedBullet(BULLET_MODEL, origin);
                case 3:MoveAndSpinOnlyBullet(BULLET_MODEL, origin);
                case 4:SpinAwayBullet(BULLET_MODEL, origin);
                case 5:SpinAwaySplitBullet(BULLET_MODEL, origin);
                case 6:FullRandomBullet(BULLET_MODEL, origin);
                case 7:AllRandomMachineBullet(BULLET_MODEL, origin);
                case 8:RotateMachineBullet(BULLET_MODEL, origin);
                case 9:LaunchSlowChangeDirBullet(BULLET_MODEL, origin);
                case 10:RoundSpinBullet(BULLET_MODEL, origin);
                case 11:SpinBackBullet(BULLET_MODEL, origin);
                case 12:ShapeDrawingBullet(BULLET_MODEL, origin);
                case 13:SplitStraightBullet(BULLET_MODEL, origin);
                case 14:AppearAndDisapperBullet(BULLET_MODEL, origin, .bullet_amount=12, .angle_difference_min=360.0/12, .angle_difference_max=360.0/12);
                case 15:SplitBackwardBullet(BULLET_MODEL, origin);
                case 16:ExplosiveBullet(BULLET_MODEL, EXPLOSION_SPRITE_ID, origin, .explosive_bullet_sound=EXPLOSION_SOUND);
                case 17:SwirlBullet(BULLET_MODEL, origin);
                case 18:TracerBullet(BULLET_MODEL, origin, .target=client);
                //case :(BULLET_MODEL, origin);
            }

            DisplayDebugBulletSkillMenu(client, key);
        }

        return 1;
    }