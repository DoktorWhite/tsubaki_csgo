#include <sourcemod>

#include <tsubaki_v3/tsubaki_common>
#include <tsubaki_v3/tsubaki_trap>

#pragma semicolon 1

StringMap gsm_EntityRefToTrap;

public Plugin myinfo = 
{
    name = "TsuBaKi Trap",
    author = "WhiteCola",
    description = "",
    version = "0.0",
    url = ""
}

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max) {
    CreateNative("CreateTrap", Native_CreateTrap);
    CreateNative("GetTrapDataByRef", Native_GetTrapDataByRef);
    return APLRes_Success;
}

public OnMapStart() {
    gsm_EntityRefToTrap = new StringMap();
}

public OnMapEnd() {
    delete gsm_EntityRefToTrap;
}

public Native_CreateTrap(Handle handle, numParams)
{
    new Trap:trap, String:model_route[256], String:clsname[32], Float:last_time=GetNativeCell(11);
    trap.sprite_id = GetNativeCell(1);
    GetNativeString(2, model_route, sizeof(model_route));
    trap.type = GetNativeCell(3);
    GetNativeArray(4, trap.origin, 3);
    trap.owner = GetNativeCell(5);
    trap.length = GetNativeCell(6);
    trap.width = GetNativeCell(7);
    trap.height = GetNativeCell(8);
    trap.speed = GetNativeCell(9);
    trap.damage = GetNativeCell(10);
    trap.last_time_after_trigger = GetNativeCell(12);
    trap.process_interval = GetNativeCell(13);
    trap.counter = 0;
    trap.total_process_amount = RoundFloat(trap.last_time_after_trigger/trap.process_interval);
    GetNativeArray(14, trap.colors, 4);
    GetNativeArray(15, trap.colors_after_trigger, 4);
    trap.damagebit = GetNativeCell(16);
    FormatEx(clsname, 32, "%s%s", TRAP_HEADER, NAME_OF_TRAP);

    int ref = CreateInvisibleLauncher(clsname
                                        , model_route
                                        , .owner=trap.owner
                                        , .get_reference=true);

    if(ref==-1)
        return -1;

    TeleportEntity(ref, trap.origin);
    trap.think_timer = CreateTimer(DEFAULT_TRAP_THINKTIME, OnTrapThink, ref, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
    
    //szRef
    model_route[0] = 0;
    IntToString(ref, model_route, sizeof(model_route));

    if(last_time != 0.0) {
        trap.remove_timer = CreateTimer(last_time, RemoveTrapData, ref, TIMER_FLAG_NO_MAPCHANGE);
    }

    gsm_EntityRefToTrap.SetArray(model_route, trap, sizeof(trap), true);

    OnTrapThink(INVALID_HANDLE, ref);

    return ref;
}

    public Action OnTrapThink(Handle timer, int ref) {
        static String:szRef[32], Trap:trap, target, Float:target_origin[3], bool:target_found;

        szRef[0] = 0;
        IntToString(ref, szRef, sizeof(szRef));

        if(!gsm_EntityRefToTrap.GetArray(szRef, trap, sizeof(trap)))
        {
            if(IsValidEntity(ref))
                RemoveEntity(ref);

            return Plugin_Stop;
        }

        for(target=1, target_found=false; target<=MaxClients; target++) {
            if(IsClientInGame(target) && IsPlayerAlive(target) && (trap.owner==0 || GetClientTeam(trap.owner)!=GetClientTeam(target))) {
                GetEntityOrigin(target, target_origin);
                target_origin[0] -= trap.origin[0];
                target_origin[1] -= trap.origin[1];

                //When someone steps inside the trap
                if(SquareRoot(target_origin[0]*target_origin[0]+target_origin[1]*target_origin[1]) < trap.length && Abs(target_origin[2]-trap.origin[2]) < trap.height) {
                    target_found = true;
                    break;
                }
            }
        }

        if(target_found) {
            if(trap.remove_timer != INVALID_HANDLE) {
                KillTimer(trap.remove_timer);
            }

            trap.think_timer = CreateTimer(trap.process_interval, TriggeredTrapThink, ref, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);

            TBKSquareLaserTE(.sprite_id=trap.sprite_id
                                ,.center=trap.origin
                                ,.length=trap.length
                                ,.width=trap.width
                                ,.life=trap.last_time_after_trigger
                                ,.r=trap.colors_after_trigger[0]
                                ,.g=trap.colors_after_trigger[1]
                                ,.b=trap.colors_after_trigger[2]
                                ,.a=trap.colors_after_trigger[3]
                                ,.speed=trap.speed
                            );
                            
            gsm_EntityRefToTrap.SetArray(szRef, trap, sizeof(trap), true);

            return Plugin_Stop;
        }

        //Trap Render
        TBKSquareLaserTE(.sprite_id=trap.sprite_id
                            ,.center=trap.origin
                            ,.length=trap.length
                            ,.width=trap.width
                            ,.life=DEFAULT_TRAP_THINKTIME
                            ,.r=trap.colors[0]
                            ,.g=trap.colors[1]
                            ,.b=trap.colors[2]
                            ,.a=trap.colors[3]
                            ,.speed=0
                            );

        return Plugin_Continue;
    }

    public Action TriggeredTrapThink(Handle timer, int ref) {

        static String:szRef[32], Trap:trap, target, Float:target_origin[3];
        szRef[0] = 0;
        IntToString(ref, szRef, sizeof(szRef));

        if(!gsm_EntityRefToTrap.GetArray(szRef, trap, sizeof(trap))) {
            return Plugin_Stop;
        }
        
        trap.counter++;
        gsm_EntityRefToTrap.SetArray(szRef, trap, sizeof(trap), true);

        for(target=1; target<=MaxClients; target++) {
            if(IsClientInGame(target) && IsPlayerAlive(target) && (trap.owner==0 || GetClientTeam(trap.owner)!=GetClientTeam(target))) {
                GetEntityOrigin(target, target_origin);
                target_origin[0] -= trap.origin[0];
                target_origin[1] -= trap.origin[1];

                //When someone steps inside the trap
                if(SquareRoot(target_origin[0]*target_origin[0]+target_origin[1]*target_origin[1]) < trap.length && Abs(target_origin[2]-trap.origin[2]) < trap.height) {
                    SDKHooks_TakeDamage(.entity=target, .inflictor=ref, .attacker=trap.owner, .damage=trap.damage, .damageType=trap.damagebit);
                }
            }
        }

        if(trap.counter >= trap.total_process_amount) {
            RemoveTrapData(INVALID_HANDLE, ref);
            return Plugin_Stop;
        }

        return Plugin_Continue;
    }

    public Action RemoveTrapData(Handle timer, int ref) {
        new String:szRef[32], Trap:trap;
        IntToString(ref, szRef, sizeof(szRef));

        if(gsm_EntityRefToTrap.GetArray(szRef, trap, sizeof(trap))) {
            KillTimer(trap.think_timer);
            gsm_EntityRefToTrap.Remove(szRef);
            RemoveEntity(ref);
        }

        return Plugin_Stop;
    }

public Native_GetTrapDataByRef(Handle:handle, numParams) {

    new Trap:trap, String:szRef[32];
    IntToString(GetNativeCell(1), szRef, sizeof(szRef));

    if(!gsm_EntityRefToTrap.GetArray(szRef, trap, sizeof(trap))) {
        return false;
    }

    SetNativeCellRef(2, trap.type);
    SetNativeCellRef(3, trap.owner);
    SetNativeCellRef(4, trap.damage);

    return true;
}