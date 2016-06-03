//
// Created by andre on 24.05.16.
//

#include <utils/logger.h>
#include "DaedalusGameState.h"
#include "DaedalusVM.h"

using namespace ZenLoad;
using namespace GameState;

DaedalusGameState::DaedalusGameState(ZenLoad::DaedalusVM& vm)
    : m_VM(vm)
{
    // Register external lib-functions
}

void DaedalusGameState::registerExternals()
{
    bool l = true;

    m_VM.registerExternalFunction("wld_insertitem", [&](ZenLoad::DaedalusVM& vm){
        std::string spawnpoint = vm.popString(); if(l) LogInfo() << "spawnpoint: " << spawnpoint;
        uint32_t iteminstance = vm.popDataValue(); if(l) LogInfo() << "iteminstance: " << iteminstance;

        ItemHandle item = createItem();
        GEngineClasses::C_Item& itemData = getItem(item);
        vm.initializeInstance(ZMemory::toBigHandle(item), iteminstance, IC_Item);

        LogInfo() << " ##### Created item: " << itemData.name;
    });

    m_VM.registerExternalFunction("wld_insertnpc", [&](ZenLoad::DaedalusVM& vm){
        std::string spawnpoint = vm.popString(); if(l) LogInfo() << "spawnpoint: " << spawnpoint;
        uint32_t npcinstance = vm.popDataValue(); if(l) LogInfo() << "npcinstance: " << npcinstance;

        NpcHandle npc = createNPC();

        LogInfo() << "Created npc idx: " << npc.index;

        GEngineClasses::C_Npc& npcData = getNpc(npc);
        npcData.wp = spawnpoint;

        vm.initializeInstance(ZMemory::toBigHandle(npc), npcinstance, IC_Npc);

        LogInfo() << " ##### Created NPC: " << npcData.name[0];
    });

    m_VM.registerExternalFunction("createinvitem", [&](ZenLoad::DaedalusVM& vm){
        int32_t itemInstance = vm.popDataValue(); if(l) LogInfo() << "itemInstance: " << itemInstance;
        uint32_t arr_n0;
        int32_t npc = vm.popVar(arr_n0); if(l) LogInfo() << "npc: " << npc;

        ItemHandle h = createItem();
        GEngineClasses::C_Item& item = getItem(h);
        vm.initializeInstance(ZMemory::toBigHandle(h), static_cast<size_t>(itemInstance), IC_Item);

        LogInfo() << "NPC Handle: " << vm.getDATFile().getSymbolByIndex(npc).instanceDataHandle.index;

        // TODO: Put into inventory
        GEngineClasses::C_Npc& npcData = getNpc(ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(npc).instanceDataHandle));

        LogInfo() << "1: " << item.name;
        LogInfo() << "2. " << npcData.name[0];
        LogInfo() << " ##### Created Inventory-Item '" << item.name << "' for NPC: " << npcData.name[0];

    });

    m_VM.registerExternalFunction("hlp_getnpc", [&](ZenLoad::DaedalusVM& vm){
        int32_t instancename = vm.popDataValue(); if(l) LogInfo() << "instancename: " << instancename;

        if(!vm.getDATFile().getSymbolByIndex(instancename).instanceDataHandle.isValid())
        {
            NpcHandle h = createNPC();
            GEngineClasses::C_Npc& npc = getNpc(h);
            vm.initializeInstance(ZMemory::toBigHandle(h), static_cast<size_t>(instancename), IC_Npc);
        }else
        {
            GEngineClasses::C_Npc& npcData = getNpc(ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(instancename).instanceDataHandle));
            if(l) LogInfo() << " [HLP_GETNPC] Name: "
                  << npcData.name[0];
        }

        // TODO: PB returns the actual address to the npc here. But why?
        vm.setReturnVar(instancename);
    });

    m_VM.registerExternalFunction("Wld_GetDay", [&](ZenLoad::DaedalusVM& vm){
        // TODO: Implement this
        vm.setReturn(0);
    });
}



ZenLoad::GEngineClasses::Instance* DaedalusGameState::getByClass(ZMemory::BigHandle h, ZenLoad::EInstanceClass instClass)
{
    if(!h.isValid()) return nullptr;

    switch(instClass)
    {
        case EInstanceClass::IC_Npc:
            return &getNpc(ZMemory::handleCast<NpcHandle>(h));

        case EInstanceClass::IC_Item:
            return &getItem(ZMemory::handleCast<ItemHandle>(h));

        case IC_Mission:
            return &getMission(ZMemory::handleCast<MissionHandle>(h));

        case IC_Info:
            return &getInfo(ZMemory::handleCast<InfoHandle>(h));

        case IC_ItemReact:
            return &getItemReact(ZMemory::handleCast<ItemReactHandle>(h));

        case IC_Focus:
            return &getFocus(ZMemory::handleCast<FocusHandle>(h));

        default:
            return nullptr;

    }
}

NpcHandle DaedalusGameState::createNPC()
{
    NpcHandle h = m_RegisteredObjects.NPCs.createObject();
    LogInfo() << "Created NPC: " << h.index;
    return h;
}

ItemHandle DaedalusGameState::createItem()
{
    return m_RegisteredObjects.items.createObject();
}

ItemReactHandle DaedalusGameState::createItemReact()
{
    return m_RegisteredObjects.itemReacts.createObject();
}

MissionHandle DaedalusGameState::createMission()
{
    return m_RegisteredObjects.missions.createObject();
}

InfoHandle DaedalusGameState::createInfo()
{
    return m_RegisteredObjects.infos.createObject();
}

FocusHandle DaedalusGameState::createFocus()
{
    return m_RegisteredObjects.focuses.createObject();
}

















