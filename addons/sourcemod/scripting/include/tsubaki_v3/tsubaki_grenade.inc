#if defined _tsubaki_grenade
    #endinput
#endif
#define _tsubaki_grenade

#include <tsubaki_v3/tsubaki_trap>
#include <tsubaki_v3/tsubaki_damage_area>

#define CVAR_CAN_CUSTOMIZE_GRENADE "can_customize_grenade"
#define CAN_CUSTOMIZE_GRENADE   1


#define TOTAL_GRENADE_EXPLODE_TYPE  4
enum GrenadeExplodeType {
    GrenadeExplodeType_Normal
    , GrenadeExplodeType_Touch
    , GrenadeExplodeType_Time
    , GrenadeExplodeType_ZeroVelocity
    , GrenadeExplodeType_Last
}

#define TOTAL_GRENADE_TYPE  3
enum GrenadeType {
    GrenadeType_Normal
    , GrenadeType_Damage_Area
    , GrenadeType_Trap
    , GrenadeType_Last
}

enum struct Grenade {
    bool enable;
    GrenadeExplodeType grenade_explode_type;
    GrenadeType grenade_type;
    TrapType trap_type;
    DamageAreaType damage_area_type;
    float grenade_trigger_time;
    int laser_sprite_id;
}

native SwitchGrenadeExplodeType(client, GrenadeExplodeType grenade_explode_type, any data=0);

native SwitchGrenadeType(client, GrenadeType grenade_type, any data=0);

native SwitchGrenadeLaserSprite(client, laser_sprite_id);

native SwitchCustomGrenadeOnOff(client, bool onoff);