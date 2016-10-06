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

    m_VM.registerExternalFunction("wld_insertitem", [=](Daedalus::DaedalusVM& vm){
        std::string spawnpoint = vm.popString(); if(l) LogInfo() << "spawnpoint: " << spawnpoint;
        uint32_t iteminstance = vm.popDataValue(); if(l) LogInfo() << "iteminstance: " << iteminstance;

        ItemHandle item = createItem();
        GEngineClasses::C_Item& itemData = getItem(item);
        vm.initializeInstance(ZMemory::toBigHandle(item), iteminstance, IC_Item);

        if(l) LogInfo() << " ##### Created item: " << itemData.name;

        if(m_GameExternals.wld_insertitem)
            m_GameExternals.wld_insertitem(item);
    });

    m_VM.registerExternalFunction("wld_insertnpc", [=](Daedalus::DaedalusVM& vm){
        std::string spawnpoint = vm.popString(); if(l) LogInfo() << "spawnpoint: " << spawnpoint;
        uint32_t npcinstance = vm.popDataValue(); if(l) LogInfo() << "npcinstance: " << npcinstance;

        insertNPC(npcinstance, spawnpoint);
    });

    m_VM.registerExternalFunction("createinvitem", [=](Daedalus::DaedalusVM& vm){
        int32_t itemInstance = vm.popDataValue(); if(l) LogInfo() << "itemInstance: " << itemInstance;
        uint32_t arr_n0;
        int32_t npc = vm.popVar(arr_n0);

        NpcHandle hnpc = ZMemory::handleCast<NpcHandle>(m_VM.getDATFile().getSymbolByIndex(npc).instanceDataHandle);

        ItemHandle h = createInventoryItem(itemInstance, hnpc);

        //LogInfo() << "1: " << item.name;
        //LogInfo() << "2. " << npcData.name[0];
        //LogInfo() << " ##### Created Inventory-Item '" << item.name << "' for NPC: " << npcData.name[0];
    });

    m_VM.registerExternalFunction("createinvitems", [=](Daedalus::DaedalusVM& vm){
        int32_t num = vm.popDataValue();
        int32_t itemInstance = vm.popDataValue(); if(l) LogInfo() << "itemInstance: " << itemInstance;
        uint32_t arr_n0;
        int32_t npc = vm.popVar(arr_n0);

        NpcHandle hnpc = ZMemory::handleCast<NpcHandle>(m_VM.getDATFile().getSymbolByIndex(npc).instanceDataHandle);

        for(int32_t i=0;i<num;i++)
        {
            ItemHandle h = createInventoryItem(itemInstance, hnpc);
        }

        //LogInfo() << "1: " << item.name;
        //LogInfo() << "2. " << npcData.name[0];
        //LogInfo() << " ##### Created Inventory-Item '" << item.name << "' for NPC: " << npcData.name[0];
    });

    m_VM.registerExternalFunction("hlp_getnpc", [=](Daedalus::DaedalusVM& vm){
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

    m_VM.registerExternalFunction("hlp_isvalidnpc", [=](Daedalus::DaedalusVM& vm){
        int32_t self = vm.popVar();

        if(vm.getDATFile().getSymbolByIndex(self).instanceDataHandle.isValid())
        {
            vm.setReturn(1);
        }else
        {
            vm.setReturn(0);
        }
    });

    m_VM.registerExternalFunction("Wld_GetDay", [=](Daedalus::DaedalusVM& vm){

        if(m_GameExternals.wld_GetDay)
            vm.setReturn(m_GameExternals.wld_GetDay());
        else
            vm.setReturn(0);
    });

    m_VM.registerExternalFunction("log_createtopic", [=](Daedalus::DaedalusVM& vm){
        int32_t section = vm.popDataValue();
        std::string name = vm.popString();
        m_PlayerLog[name].section = static_cast<LogTopic::ESection>(section);
        m_PlayerLog[name].topic = name;

        if(m_GameExternals.log_createtopic)
            m_GameExternals.log_createtopic(name);
    });

    m_VM.registerExternalFunction("log_settopicstatus", [=](Daedalus::DaedalusVM& vm){
        int32_t status = vm.popDataValue();
        std::string name = vm.popString();

        m_PlayerLog[name].status = static_cast<LogTopic::ELogStatus>(status);

        if(m_GameExternals.log_settopicstatus)
            m_GameExternals.log_settopicstatus(name);
    });

    m_VM.registerExternalFunction("log_addentry", [=](Daedalus::DaedalusVM& vm){
        std::string entry = vm.popString();
        std::string topic = vm.popString();

        LogInfo() << "";
        LogInfo() << " ########### New Log Entry: " << topic << " ########### ";
        LogInfo() << entry;
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

ItemHandle DaedalusGameState::createInventoryItem(size_t itemSymbol, NpcHandle npc)
{
    auto items = m_NpcInventories[npc];

    // Try to find an item of this type
    for(ItemHandle h : items)
    {
        GEngineClasses::C_Item& item = getItem(h);

        // Just add to the count here
        if(item.instanceSymbol == itemSymbol)
        {
            item.count[0]++;
            return h;
        }
    }

    // Get memory for the item
    ItemHandle h = createItem();
    GEngineClasses::C_Item& item = getItem(h);
    item.count[0] = 1;

    // Run the script-constructor
    m_VM.initializeInstance(ZMemory::toBigHandle(h), static_cast<size_t>(itemSymbol), IC_Item);

    // Put inside its inventory
    addItemToInventory(h, npc);

    return h;
}

ItemHandle DaedalusGameState::addItemToInventory(ItemHandle item, NpcHandle npc)
{
    auto items = m_NpcInventories[npc];

    // Try to find an item of this type
    for(ItemHandle h : items)
    {
        GEngineClasses::C_Item& i = getItem(h);

        // Just add to the count here
        if(i.instanceSymbol == getItem(item).instanceSymbol)
        {
            i.count[0]++;

            m_RegisteredObjects.items.removeObject(item);
            return h;
        }
    }

    m_NpcInventories[npc].push_back(item);

    if (m_GameExternals.createinvitem)
        m_GameExternals.createinvitem(item, npc);
}


bool DaedalusGameState::removeInventoryItem(size_t itemSymbol, NpcHandle npc)
{
    for(auto it = m_NpcInventories[npc].begin(); it != m_NpcInventories[npc].end(); it++)
    {
        Daedalus::GEngineClasses::C_Item& item = getItem((*it));
        if(item.instanceSymbol == itemSymbol)
        {
            m_NpcInventories[npc].erase(it);

            // Clear memory
            m_RegisteredObjects.items.removeObject(*it);

            return true;
        }
    }

    return false;
}

NpcHandle DaedalusGameState::insertNPC(size_t instance, const std::string &waypoint)
{
    NpcHandle npc = createNPC();

    GEngineClasses::C_Npc& npcData = getNpc(npc);
    npcData.wp = waypoint;

    // Setup basic class linkage. This is important here, as otherwise the wld_insertnpc-callback won't be
    // able to work properly
    npcData.instanceSymbol = instance;

    PARSymbol& s = m_VM.getDATFile().getSymbolByIndex(instance);
    s.instanceDataHandle = ZMemory::toBigHandle(npc);
    s.instanceDataClass = IC_Npc;

    if(m_GameExternals.wld_insertnpc)
        m_GameExternals.wld_insertnpc(npc, waypoint);

    m_VM.initializeInstance(ZMemory::toBigHandle(npc), instance, IC_Npc);

    // Init complete, notify the engine
    if(m_GameExternals.post_wld_insertnpc)
        m_GameExternals.post_wld_insertnpc(npc);

    return npc;
}

NpcHandle DaedalusGameState::insertNPC(const std::string &instance, const std::string& waypoint)
{
    return insertNPC(m_VM.getDATFile().getSymbolIndexByName(instance), waypoint);
}

ItemHandle DaedalusGameState::insertItem(size_t instance)
{
    // Get memory for the item
    ItemHandle h = createItem();
    GEngineClasses::C_Item& item = getItem(h);

    // Run the script-constructor
    m_VM.initializeInstance(ZMemory::toBigHandle(h), static_cast<size_t>(instance), IC_Item);

    return h;
}

ItemHandle DaedalusGameState::insertItem(const std::string &instance)
{
    return insertItem(m_VM.getDATFile().getSymbolIndexByName(instance));
}

