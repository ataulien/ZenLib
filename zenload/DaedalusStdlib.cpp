//
// Created by andre on 13.05.16.
//

#include <utils/logger.h>
#include "DaedalusStdlib.h"
#include "DaedalusVM.h"

#define OFFSET(c, v) (int32_t)(((char*)v) - ((char*)c))
#define REGISTER(cname, obj, var) vm.getDATFile().getSymbolByName(cname "." #var).dataOffset = OFFSET(&obj, &obj.var);

void ZenLoad::registerDaedalusStdLib(DaedalusVM& vm)
{
    vm.registerExternalFunction("inttostring", [](ZenLoad::DaedalusVM& vm){
        int32_t x = vm.popDataValue(); LogInfo() << "x: " << x;

        vm.setReturn(std::to_string(x));
    });

    vm.registerExternalFunction("floattoint", [](ZenLoad::DaedalusVM& vm){
        float x = *reinterpret_cast<float*>(vm.popDataValue()); LogInfo() << "x: " << x;
        vm.setReturn(static_cast<int32_t>(x));
    });

    vm.registerExternalFunction("inttofloat", [](ZenLoad::DaedalusVM& vm){
        int32_t x = vm.popDataValue(); LogInfo() << "x: " << x;
        float y = static_cast<float>(x);

        vm.setReturn(*reinterpret_cast<int32_t*>(&y));
    });

    vm.registerExternalFunction("concatstrings", [](ZenLoad::DaedalusVM& vm){
        uint32_t arr1, arr2;
        int32_t s2 = vm.popVar(arr1);
        int32_t s1 = vm.popVar(arr2);

        ZenLoad::PARSymbol& sym1 = vm.getDATFile().getSymbolByIndex(s1); LogInfo() << "s1: " << s1 << ", " << *sym1.getStrAddr(arr1);
        ZenLoad::PARSymbol& sym2 = vm.getDATFile().getSymbolByIndex(s2); LogInfo() << "s2: " << s2 << ", " << *sym2.getStrAddr(arr2);

        std::string r = (*sym1.getStrAddr(arr1)) + (*sym2.getStrAddr(arr2));
        LogInfo() << "Result: " << r;

        vm.setReturn(r);
    });

    vm.registerExternalFunction("wld_insertitem", [](ZenLoad::DaedalusVM& vm){
        std::string spawnpoint = vm.popString(); LogInfo() << "spawnpoint: " << spawnpoint;
        uint32_t iteminstance = vm.popDataValue(); LogInfo() << "iteminstance: " << iteminstance;

        // TODO: Put these in a continuous array
        GEngineClasses::C_Item* item = new GEngineClasses::C_Item;
        vm.initializeInstance(item, iteminstance);

        LogInfo() << " ##### Created item: " << item->name;
    });

    vm.registerExternalFunction("wld_insertnpc", [](ZenLoad::DaedalusVM& vm){
        std::string spawnpoint = vm.popString(); LogInfo() << "spawnpoint: " << spawnpoint;
        uint32_t npcinstance = vm.popDataValue(); LogInfo() << "npcinstance: " << npcinstance;

        // TODO: Put these in a continuous array
        GEngineClasses::C_NPC* npc = new GEngineClasses::C_NPC;
        npc->wp = spawnpoint;

        vm.initializeInstance(npc, npcinstance);



        LogInfo() << " ##### Created NPC: " << npc->name[0];
    });

    vm.registerExternalFunction("createinvitem", [](ZenLoad::DaedalusVM& vm){
        int32_t itemInstance = vm.popDataValue(); LogInfo() << "itemInstance: " << itemInstance;
        uint32_t arr_n0;
        int32_t npc = vm.popVar(arr_n0); LogInfo() << "npc: " << npc;

        // TODO: Put these in a continuous array
        GEngineClasses::C_Item* item = new GEngineClasses::C_Item;
        vm.initializeInstance(item, itemInstance);

        // TODO: Put into inventory
        GEngineClasses::C_NPC* npcAddr = (GEngineClasses::C_NPC*)vm.getDATFile().getSymbolByIndex(npc).instanceData;
        LogInfo() << " ##### NPCAddr: 0x" << npcAddr;
        LogInfo() << " ##### Created item '" << item->name << "' for NPC: " << npcAddr->name[0];

    });

    vm.registerExternalFunction("hlp_getnpc", [](ZenLoad::DaedalusVM& vm){
        int32_t instancename = vm.popDataValue(); LogInfo() << "instancename: " << instancename;

        if(!vm.getDATFile().getSymbolByIndex(instancename).instanceData)
        {
            // TODO: Put these in a continuous array
            GEngineClasses::C_NPC* npc = new GEngineClasses::C_NPC;
            vm.initializeInstance(npc, instancename);

            LogInfo() << " ##### [HLP_GETNPC] Created NPC: " << npc->name[0];
        }else
        {
            LogInfo() << " #### [HLP_GETNPC] Name: " << ((GEngineClasses::C_NPC*) vm.getDATFile().getSymbolByIndex(instancename).instanceData)->name[0];
        }

        // TODO: PB returns the actual address to the npc here. But why?
        vm.setReturnVar(instancename);

    });

    //
}

void ::ZenLoad::registerGothicEngineClasses(DaedalusVM& vm)
{
    GEngineClasses::C_NPC npc;
    GEngineClasses::C_Focus focus;
    GEngineClasses::C_INFO info;
    GEngineClasses::C_ITEMREACT itemreact;
    GEngineClasses::C_Item item;
    GEngineClasses::C_Mission mission;

    REGISTER("C_NPC", npc, id);
    REGISTER("C_NPC", npc, name);
    REGISTER("C_NPC", npc, slot);
    REGISTER("C_NPC", npc, npcType);
    REGISTER("C_NPC", npc, flags);
    REGISTER("C_NPC", npc, attribute);
    REGISTER("C_NPC", npc, protection);
    REGISTER("C_NPC", npc, damage);
    REGISTER("C_NPC", npc, damagetype);
    REGISTER("C_NPC", npc, guild);
    REGISTER("C_NPC", npc, level);
    REGISTER("C_NPC", npc, mission);
    REGISTER("C_NPC", npc, fight_tactic);
    REGISTER("C_NPC", npc, weapon);
    REGISTER("C_NPC", npc, voice);
    REGISTER("C_NPC", npc, voicePitch);
    REGISTER("C_NPC", npc, bodymass);
    REGISTER("C_NPC", npc, daily_routine);
    REGISTER("C_NPC", npc, spawnPoint32_t);
    REGISTER("C_NPC", npc, spawnDelay);
    REGISTER("C_NPC", npc, senses);
    REGISTER("C_NPC", npc, senses_range);
    REGISTER("C_NPC", npc, ai);

    REGISTER("C_Focus", focus, npc_longrange);
    REGISTER("C_Focus", focus, npc_range1);
    REGISTER("C_Focus", focus, npc_range2);
    REGISTER("C_Focus", focus, npc_azi);
    REGISTER("C_Focus", focus, npc_elevdo);
    REGISTER("C_Focus", focus, npc_elevup);
    REGISTER("C_Focus", focus, npc_prio);
    REGISTER("C_Focus", focus, item_range1);
    REGISTER("C_Focus", focus, item_range2);
    REGISTER("C_Focus", focus, item_azi);
    REGISTER("C_Focus", focus, item_elevdo);
    REGISTER("C_Focus", focus, item_elevup);
    REGISTER("C_Focus", focus, item_prio);
    REGISTER("C_Focus", focus, mob_range1);
    REGISTER("C_Focus", focus, mob_range2);
    REGISTER("C_Focus", focus, mob_azi);
    REGISTER("C_Focus", focus, mob_elevdo );
    REGISTER("C_Focus", focus, mob_elevup);
    REGISTER("C_Focus", focus, mob_prio);

    REGISTER("C_Info", info, npc);
    REGISTER("C_Info", info, nr);
    REGISTER("C_Info", info, important);
    REGISTER("C_Info", info, condition);
    REGISTER("C_Info", info, information);
    REGISTER("C_Info", info, description);
    REGISTER("C_Info", info, trade);
    REGISTER("C_Info", info, permanent);

    REGISTER("C_ItemReact", itemreact, npc);
    REGISTER("C_ItemReact", itemreact, trade_item);
    REGISTER("C_ItemReact", itemreact, trade_amount);
    REGISTER("C_ItemReact", itemreact, requested_cat);
    REGISTER("C_ItemReact", itemreact, requested_item);
    REGISTER("C_ItemReact", itemreact, requested_amount);
    REGISTER("C_ItemReact", itemreact, reaction);

    REGISTER("C_Item", item, id);
    REGISTER("C_Item", item, name );
    REGISTER("C_Item", item, nameID);
    REGISTER("C_Item", item, hp);
    REGISTER("C_Item", item, hp_max);
    REGISTER("C_Item", item, mainflag );
    REGISTER("C_Item", item, flags);
    REGISTER("C_Item", item, weight );
    REGISTER("C_Item", item, value);
    REGISTER("C_Item", item, damageType);
    REGISTER("C_Item", item, damageTotal);
    REGISTER("C_Item", item, damage);
    REGISTER("C_Item", item, wear);
    REGISTER("C_Item", item, protection);
    REGISTER("C_Item", item, nutrition);
    REGISTER("C_Item", item, cond_atr);
    REGISTER("C_Item", item, cond_value);
    REGISTER("C_Item", item, change_atr);
    REGISTER("C_Item", item, change_value);
    REGISTER("C_Item", item, magic   );
    REGISTER("C_Item", item, on_equip    );
    REGISTER("C_Item", item, on_unequip  );
    REGISTER("C_Item", item, on_state);
    REGISTER("C_Item", item, owner   );
    REGISTER("C_Item", item, ownerGuild    );
    REGISTER("C_Item", item, disguiseGuild);
    REGISTER("C_Item", item, visual);
    REGISTER("C_Item", item, visual_change );
    REGISTER("C_Item", item, visual_skin);
    REGISTER("C_Item", item, scemeName);
    REGISTER("C_Item", item, material);
    REGISTER("C_Item", item, munition);
    REGISTER("C_Item", item, spell);
    REGISTER("C_Item", item, range);
    REGISTER("C_Item", item, mag_circle);
    REGISTER("C_Item", item, description);
    REGISTER("C_Item", item, text);
    REGISTER("C_Item", item, count);

    REGISTER("C_Mission", mission, name);
    REGISTER("C_Mission", mission, description);
    REGISTER("C_Mission", mission, duration);
    REGISTER("C_Mission", mission, important);
    REGISTER("C_Mission", mission, offerConditions);
    REGISTER("C_Mission", mission, offer);
    REGISTER("C_Mission", mission, successConditions);
    REGISTER("C_Mission", mission, success);
    REGISTER("C_Mission", mission, failureConditions);
    REGISTER("C_Mission", mission, failure);
    REGISTER("C_Mission", mission, obsoleteConditions);
    REGISTER("C_Mission", mission, obsolete);
    REGISTER("C_Mission", mission, running);

}



