#if defined _tsubaki_damage_area
    #endinput
#endif
#define _tsubaki_damage_area

#include <sdkhooks>

stock String:DAMAGE_AREA_HEADER[] = "DamageArea_";

enum DamageAreaType {
    DamageArea_Normal
    , DamageArea_Fire
    , DamageArea_Freeze
    , DamageArea_Repluse
    , DamageArea_Attractive
    , DamageArea_Last
}


#define DAMAGE_AREA_DATA_SIZE   14
enum struct DamageArea {
    int laser_sprite_id;
    DamageAreaType type;
    float effect_last_time;
    float damage;
    int target_identity;
    int damagebit
    float radius;
    float height;
    float origin[3];
    int colors[4];
}

#define TOTAL_DAMAGE_AREA_TYPE   5
stock String:NAME_OF_DAMAGE_AREA_TYPE[TOTAL_DAMAGE_AREA_TYPE][] = {
    "Normal"
    , "Fire"
    , "Freeze"
    , "Repluse"
    , "Attractive"
}

stock DEFAULT_DAMAGE_AREA_COLORS[TOTAL_DAMAGE_AREA_TYPE][4] = {
    {200, 200, 200, 255}
    ,{204, 0, 0, 255}
    ,{0, 204, 204, 255}
    ,{255, 178, 102, 255}
    ,{153, 0, 153, 255}

}


native int CreateDamageArea(laser_sprite_id, DamageAreaType:type, Float:origin[3], owner=0, Float:radius=300.0, Float:height=200.0,
                                Float:prepare_time=3.0, Float:effect_last_time=5.0, Float:damage=5.0, int target_identity=-1, 
                                colors[4]={200,200,200,255}, damagebit=DMG_ENERGYBEAM);

native bool GetDamageAreaDataByRef(ref, &DamageAreaType:type, &Float:damage, &target_identity);