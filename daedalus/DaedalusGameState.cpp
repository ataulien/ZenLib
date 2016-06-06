//
// Created by andre on 24.05.16.
//

#include <utils/logger.h>
#include "DaedalusGameState.h"
#include "DaedalusVM.h"
#include "DaedalusDialogManager.h"

using namespace ZenLoad;
using namespace Daedalus;
using namespace GameState;


DaedalusGameState::DaedalusGameState(Daedalus::DaedalusVM& vm)
    : m_VM(vm)
{
    // Register external lib-functions
}

void DaedalusGameState::registerExternals()
{
    bool l = false;

    m_VM.registerExternalFunction("wld_insertitem", [&](Daedalus::DaedalusVM& vm){
        std::string spawnpoint = vm.popString(); if(l) LogInfo() << "spawnpoint: " << spawnpoint;
        uint32_t iteminstance = vm.popDataValue(); if(l) LogInfo() << "iteminstance: " << iteminstance;

        ItemHandle item = createItem();
        GEngineClasses::C_Item& itemData = getItem(item);
        vm.initializeInstance(ZMemory::toBigHandle(item), iteminstance, IC_Item);

        LogInfo() << " ##### Created item: " << itemData.name;

        if(m_GameExternals.wld_insertitem)
            m_GameExternals.wld_insertitem(item);
    });

    m_VM.registerExternalFunction("wld_insertnpc", [&](Daedalus::DaedalusVM& vm){
        std::string spawnpoint = vm.popString(); if(l) LogInfo() << "spawnpoint: " << spawnpoint;
        uint32_t npcinstance = vm.popDataValue(); if(l) LogInfo() << "npcinstance: " << npcinstance;

        NpcHandle npc = createNPC();

        LogInfo() << "Created npc idx: " << npc.index;

        GEngineClasses::C_Npc& npcData = getNpc(npc);
        npcData.wp = spawnpoint;

        // Setup basic class linkage. This is important here, as otherwise the wld_insertnpc-callback won't be
        // able to work properly
        npcData.instanceSymbol = npcinstance;

        PARSymbol& s = vm.getDATFile().getSymbolByIndex(npcinstance);
        s.instanceDataHandle = ZMemory::toBigHandle(npc);
        s.instanceDataClass = IC_Npc;

        LogInfo() << "NpcInstance: " << npcinstance << ", " << s.name;

        if(m_GameExternals.wld_insertnpc)
            m_GameExternals.wld_insertnpc(npc, spawnpoint);

        vm.initializeInstance(ZMemory::toBigHandle(npc), npcinstance, IC_Npc);

        LogInfo() << " ##### Created NPC: " << npcData.name[0];
    });

    m_VM.registerExternalFunction("createinvitem", [&](Daedalus::DaedalusVM& vm){
        int32_t itemInstance = vm.popDataValue(); if(l) LogInfo() << "itemInstance: " << itemInstance;
        uint32_t arr_n0;
        int32_t npc = vm.popVar(arr_n0); if(l) LogInfo() << "npc: " << npc;

        ItemHandle h = createItem();
        GEngineClasses::C_Item& item = getItem(h);
        vm.initializeInstance(ZMemory::toBigHandle(h), static_cast<size_t>(itemInstance), IC_Item);

        //LogInfo() << "NPC Handle: " << vm.getDATFile().getSymbolByIndex(npc).instanceDataHandle.index;

        // TODO: Put into inventory
        NpcHandle hnpc = ZMemory::handleCast<NpcHandle>(vm.getDATFile().getSymbolByIndex(npc).instanceDataHandle);
        GEngineClasses::C_Npc& npcData = getNpc(hnpc);

        m_NpcInventories[hnpc].push_back(h);


        //LogInfo() << "1: " << item.name;
        //LogInfo() << "2. " << npcData.name[0];
        //LogInfo() << " ##### Created Inventory-Item '" << item.name << "' for NPC: " << npcData.name[0];
    });

    m_VM.registerExternalFunction("hlp_getnpc", [&](Daedalus::DaedalusVM& vm){
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

    m_VM.registerExternalFunction("Wld_GetDay", [&](Daedalus::DaedalusVM& vm){

        if(m_GameExternals.wld_GetDay)
            vm.setReturn(m_GameExternals.wld_GetDay());
        else
            vm.setReturn(0);
    });

    m_VM.registerExternalFunction("log_createtopic", [&](Daedalus::DaedalusVM& vm){
        int32_t section = vm.popDataValue();
        std::string name = vm.popString();
        m_PlayerLog[name].section = static_cast<LogTopic::ESection>(section);
        m_PlayerLog[name].topic = name;

        if(m_GameExternals.log_createtopic)
            m_GameExternals.log_createtopic(name);
    });

    m_VM.registerExternalFunction("log_settopicstatus", [&](Daedalus::DaedalusVM& vm){
        int32_t status = vm.popDataValue();
        std::string name = vm.popString();

        m_PlayerLog[name].status = static_cast<LogTopic::ELogStatus>(status);

        if(m_GameExternals.log_settopicstatus)
            m_GameExternals.log_settopicstatus(name);
    });

    m_VM.registerExternalFunction("log_addentry", [&](Daedalus::DaedalusVM& vm){
        std::string entry = vm.popString();
        std::string topic = vm.popString();

        LogInfo() << "";
        LogInfo() << " ########### New Log Entry: " << topic << " ########### ";
        LogInfo() << "";

        m_PlayerLog[topic].entries.push_back(entry);

        if(m_GameExternals.log_addentry)
            m_GameExternals.log_addentry(topic, entry);
    });
}



Daedalus::GEngineClasses::Instance* DaedalusGameState::getByClass(ZMemory::BigHandle h, EInstanceClass instClass)
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
    getNpc(h).userPtr = nullptr;
    return h;
}

ItemHandle DaedalusGameState::createItem()
{
    auto h = m_RegisteredObjects.items.createObject();

    getItem(h).userPtr = nullptr;

    return h;
}

ItemReactHandle DaedalusGameState::createItemReact()
{
    auto h = m_RegisteredObjects.itemReacts.createObject();
    getItemReact(h).userPtr = nullptr;
    return h;
}

MissionHandle DaedalusGameState::createMission()
{
    auto h = m_RegisteredObjects.missions.createObject();
    getMission(h).userPtr = nullptr;
    return h;
}

InfoHandle DaedalusGameState::createInfo()
{
    auto h = m_RegisteredObjects.infos.createObject();
    getInfo(h).userPtr = nullptr;
    return h;
}

FocusHandle DaedalusGameState::createFocus()
{
    auto h = m_RegisteredObjects.focuses.createObject();
    getFocus(h).userPtr = nullptr;
    return h;
}
