//
// Created by andre on 13.05.16.
//

#include <utils/logger.h>
#include "DaedalusStdlib.h"

#define OFFSET(c, v) (int32_t)(((char*)c) - ((char*)v))
#define REGISTER(obj, var) vm.getDATFile().getSymbolByName("C_NPC." #var).classOffset = OFFSET(&obj, &obj.var);

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
    //
}

void ::ZenLoad::registerGothicEngineClasses(DaedalusVM& vm)
{
    GEngineClasses::C_NPC npc;
    REGISTER(npc, id);
    REGISTER(npc, name);
    REGISTER(npc, slot);
    REGISTER(npc, npcType);
    REGISTER(npc, flags);
    REGISTER(npc, attribute);
    REGISTER(npc, protection);
    REGISTER(npc, damage);
    REGISTER(npc, damagetype);
    REGISTER(npc, guild);
    REGISTER(npc, level);
    REGISTER(npc, mission);
    REGISTER(npc, fight_tactic);
    REGISTER(npc, weapon);
    REGISTER(npc, voice);
    REGISTER(npc, voicePitch);
    REGISTER(npc, bodymass);
    REGISTER(npc, daily_routine);
    REGISTER(npc, spawnPoint32_t);
    REGISTER(npc, spawnDelay);
    REGISTER(npc, senses);
    REGISTER(npc, senses_range);
    REGISTER(npc, ai);
    
}



