#include <sourcemod>
#include <tsubaki_v3/tsubaki_common>
#include <tsubaki_v3/tsubaki_damage_area>

#pragma semicolon 1

StringMap gsm_EntityRefToDamageArea;

public Plugin myinfo = 
{
    name = "TsuBaKi Damage Area",
    author = "WhiteCola",
    description = "",
    version = "0.0",
    url = ""
}

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max) {
    CreateNative("CreateDamageArea", Native_CreateDamageArea);
    CreateNative("GetDamageAreaDataByRef", Native_GetDamageAreaDataByRef);
    return APLRes_Success;
}

public OnPluginStart() {
    gsm_EntityRefToDamageArea = new StringMap();
}

public OnPluginEnd() {
    delete gsm_EntityRefToDamageArea; 
}

public OnMapStart() {
    PrecahceTsubakiLaserSprite();
}

public int Native_CreateDamageArea(Handle:plugin, numParams) {
    new String:clsname[32];
    FormatEx(clsname, sizeof(clsname), "%s%s", DAMAGE_AREA_HEADER, NAME_OF_DAMAGE_AREA_TYPE[GetNativeCell(2)]);
    int ref = CreateInvisibleLauncher(.clsname=clsname, .model_route=DEFAULT_LAUNCHER_MODEL, .owner=GetNativeCell(4), .get_reference=true);

    if(ref == -1)
        return -1;

    new DamageArea:damage_area, Float:prepare_time=GetNativeCell(7), Float:wave_height;
    damage_area.laser_sprite_id = GetNativeCell(1);
    damage_area.type = GetNativeCell(2);
    damage_area.effect_last_time = GetNativeCell(8);
    damage_area.damage = GetNativeCell(9);
    damage_area.target_identity = GetNativeCell(10);
    damage_area.radius = GetNativeCell(5);
    damage_area.damagebit = GetNativeCell(12);
    damage_area.height = GetNativeCell(6);
    wave_height = FloatClamp(damage_area.height, 0.0, 35.0);

    GetNativeArray(3, damage_area.origin, 3);
    GetNativeArray(11, damage_area.colors, 4);
    
    clsname[0]=0; 
    IntToString(ref, clsname, sizeof(clsname));
    gsm_EntityRefToDamageArea.SetArray(clsname, damage_area, DAMAGE_AREA_DATA_SIZE, false);

    if(prepare_time != 0.0) {
        TBKBeamRingPoint(damage_area.laser_sprite_id, damage_area.origin, damage_area.radius*1.9, 0.0, prepare_time, wave_height, wave_height, 0.0, damage_area.colors[0], damage_area.colors[1], damage_area.colors[2], damage_area.colors[3]);
        TBKBeamRingPoint(damage_area.laser_sprite_id, damage_area.origin, damage_area.radius*1.9, damage_area.radius*1.9-0.1, prepare_time, wave_height, wave_height, 0.0, damage_area.colors[0], damage_area.colors[1], damage_area.colors[2], damage_area.colors[3]);
        
        CreateTimer(prepare_time, ProcessDamageArea, ref, TIMER_FLAG_NO_MAPCHANGE);
        CreateTimer(prepare_time+0.2, ClearDamageAreaData, ref);
    } else {
        RequestFrame(ProcessDamageAreaByFrame, ref);
        CreateTimer(0.2, ClearDamageAreaData, ref);
    }

    //FOR DEBUG
    // PrintToServer("Address of DamageArea : 0x%x", damage_area);
    // PrintToServer("Classname : %s", clsname);
    // PrintToServer("Ref : %d", ref);
    // PrintToServer("Type : %d", damage_area.type);
    // PrintToServer("Effect Last Time : %.4f", damage_area.effect_last_time);
    // PrintToServer("Damage : %.4f", damage_area.damage);
    // PrintToServer("target_identity : %b", damage_area.target_identity);
    // PrintToServer("colors : %d %d %d %d", damage_area.colors[0], damage_area.colors[1], damage_area.colors[2], damage_area.colors[3]);
    // PrintToServer("origin : %.4f %.4f %.4f", origin[0], origin[1], origin[2]);
    // PrintToServer("prepare time : %.4f", prepare_time);
    // PrintToServer("Owner : %d", GetEntityOwner(ref));
    
    return ref;
}

    public Action ProcessDamageArea(Handle timer, int ref) {
        ProcessDamageAreaByFrame(ref);
        return Plugin_Stop;
    }

    void ProcessDamageAreaByFrame(int ref) {
        new String:szRef[32], DamageArea:damage_area, Float:target_origin[3], client=1, owner=GetEntityOwner(client);
        IntToString(ref, szRef, sizeof(szRef));

        PrintToServer("Processing Dmg Area");
        
        if(!gsm_EntityRefToDamageArea.GetArray(szRef, damage_area, DAMAGE_AREA_DATA_SIZE))
            return;

        for(; client<=MaxClients; client++) {
            if( client!=owner && IsClientInGame(client) && IsPlayerAlive(client) && (damage_area.target_identity&(1<<PLY_IDENTITY(client))) ) {
						GetClientAbsOrigin(client, target_origin);

						if(FloatAbs(target_origin[2]-damage_area.origin[2])<damage_area.height && SquareRoot((target_origin[0]-damage_area.origin[0])*(target_origin[0]-damage_area.origin[0]) + (target_origin[1]-damage_area.origin[1])*(target_origin[1]-damage_area.origin[1])) < damage_area.radius){
                            SDKHooks_TakeDamage(.entity=client, .inflictor=ref, .attacker=owner, .damage=damage_area.damage, .damageType=damage_area.damagebit); 
                        }
            }
        }

        TBKBeamRingPoint(damage_area.laser_sprite_id, damage_area.origin, 0.0, damage_area.radius*1.9, 0.5, (damage_area.height>35.0)?35.0:damage_area.height, (damage_area.height>35.0)?35.0:damage_area.height, 0.0, damage_area.colors[0], damage_area.colors[1], damage_area.colors[2], damage_area.colors[3]);
    }


    public Action ClearDamageAreaData(Handle timer, int ref) {
        new String:szRef[32];
        IntToString(ref, szRef, sizeof(szRef));

        gsm_EntityRefToDamageArea.Remove(szRef);
        RemoveEntity(ref);
        PrintToServer("Removing Dmg Area Data");
        return Plugin_Stop;
    }

public Native_GetDamageAreaDataByRef(Handle handle, numParams) {
    new DamageArea:damage_area
        , ref = GetNativeCell(1)
        , String:szRef[32];

    IntToString(ref, szRef, sizeof(szRef));
    if(!gsm_EntityRefToDamageArea.GetArray(szRef, damage_area, DAMAGE_AREA_DATA_SIZE))
        return false;

    SetNativeCellRef(2, damage_area.type);
    SetNativeCellRef(3, damage_area.damage);
    SetNativeCellRef(4, damage_area.target_identity);

    return true;
}

#pragma semicolon 0