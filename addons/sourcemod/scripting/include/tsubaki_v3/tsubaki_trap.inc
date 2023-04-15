#if defined _tsubaki_trap
    #endinput
#endif
#define _tsubaki_trap

#include <sdkhooks>

/*  Trap Properties  */

    #define DEFAULT_TRAP_LENGTH         200.0
    #define DEFAULT_TRAP_LASER_WIDTH    8.0
    #define DEFAULT_TRAP_DETECH_HEIGHT  80.0
    #define DEFAULT_TRAP_SPEED          10
    #define DEFAULT_TRAP_THINKTIME      0.2
    #define DEFAULT_TRAP_HEIGHT_CHECK   100.0
    #define DEFAULT_TRAP_DMG            5.0

/*  Trap Type  */

stock String:TRAP_HEADER[] = "Trap_";

#define TOTAL_TRAP_TYPE 3
stock String:NAME_OF_TRAP[TOTAL_TRAP_TYPE][] = {
    "Fire"
    , "Freeze"
    , "Slap"
};

stock DEFAULT_TRAP_COLORS[TOTAL_TRAP_TYPE][4] = {
    {204, 0, 0, 255}
    , {0, 204, 204, 255}
    , {200, 200, 200, 255}
}

enum TrapType {
    TrapType_Fire
    , TrapType_Freeze
    , TrapType_Slap
    , TrapType_Last
}

enum struct Trap {
    int sprite_id
    TrapType type;
    int owner;
    float damage;
    float length;
    float width;
    float height;
    int speed;
    float origin[3];
    int colors[4];
    int colors_after_trigger[4];
    float last_time_after_trigger;
    float process_interval;
    int counter;
    int total_process_amount;
    int damagebit;
    Handle think_timer;
    Handle remove_timer;
}

native CreateTrap(const sprite_id
                    , const String:model_route[]
                    , TrapType type
                    , Float:origin[3]
                    , owner=0
                    , Float:length=DEFAULT_TRAP_LENGTH
                    , Float:width=DEFAULT_TRAP_LASER_WIDTH
                    , Float:height=DEFAULT_TRAP_DETECH_HEIGHT
                    , laser_speed=DEFAULT_TRAP_SPEED
                    , Float:damage=DEFAULT_TRAP_DMG
                    , Float:last_time=5.0
                    , Float:last_time_after_trigger=5.0
                    , float process_interval=0.3
                    , colors[4]={125,0,255,255}
                    , colors_after_trigger[4]={255,0,0,255}
                    , damagebit=DMG_PARALYZE);

native bool GetTrapDataByRef(int ref, &TrapType:type, &owner, &Float:damage);