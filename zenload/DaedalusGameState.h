#pragma once
#include "DaedalusStdlib.h"
#include "DATFile.h"
#include <utils/staticReferencedAllocator.h>
#include <vector>

const int MAX_NUM_NPCS = 2000;
const int MAX_NUM_ITEMS = 4000;

namespace ZenLoad
{
    class DaedalusVM;
}

namespace GameState
{
    typedef ZMemory::StaticReferencedAllocator<ZenLoad::GEngineClasses::C_Npc, MAX_NUM_NPCS>::Handle NpcHandle;
    typedef ZMemory::StaticReferencedAllocator<ZenLoad::GEngineClasses::C_Item, MAX_NUM_ITEMS>::Handle ItemHandle;
    typedef ZMemory::StaticReferencedAllocator<ZenLoad::GEngineClasses::C_Mission, MAX_NUM_ITEMS>::Handle MissionHandle;
    typedef ZMemory::StaticReferencedAllocator<ZenLoad::GEngineClasses::C_Focus, MAX_NUM_ITEMS>::Handle FocusHandle;
    typedef ZMemory::StaticReferencedAllocator<ZenLoad::GEngineClasses::C_ItemReact, MAX_NUM_ITEMS>::Handle ItemReactHandle;
    typedef ZMemory::StaticReferencedAllocator<ZenLoad::GEngineClasses::C_Info, MAX_NUM_ITEMS>::Handle InfoHandle;

    /**
     * Class holding the current engine-side gamestate of a daedalus-VM
     */
    class DaedalusGameState
    {
    public:

        struct RegisteredObjects
        {
            ZMemory::StaticReferencedAllocator<ZenLoad::GEngineClasses::C_Npc, MAX_NUM_NPCS> NPCs;
            ZMemory::StaticReferencedAllocator<ZenLoad::GEngineClasses::C_Item, MAX_NUM_ITEMS> items;
            ZMemory::StaticReferencedAllocator<ZenLoad::GEngineClasses::C_ItemReact, MAX_NUM_ITEMS> itemReacts;
            ZMemory::StaticReferencedAllocator<ZenLoad::GEngineClasses::C_Mission, MAX_NUM_ITEMS> missions;
            ZMemory::StaticReferencedAllocator<ZenLoad::GEngineClasses::C_Focus, MAX_NUM_ITEMS> focuses;
            ZMemory::StaticReferencedAllocator<ZenLoad::GEngineClasses::C_Info, MAX_NUM_ITEMS> infos;
        };

        DaedalusGameState(ZenLoad::DaedalusVM& vm);

        /**
         * @brief registers the games externals
         */
        void registerExternals();

        /**
         * Creates scripting relevant objects
         */
        NpcHandle createNPC();
        ItemHandle createItem();
        ItemReactHandle createItemReact();
        MissionHandle createMission();
        InfoHandle createInfo();
        FocusHandle createFocus();

        /**
         * Accessors
         */
        ZenLoad::GEngineClasses::C_Npc& getNpc(NpcHandle h){return m_RegisteredObjects.NPCs.getElement(h);};
        ZenLoad::GEngineClasses::C_Item& getItem(ItemHandle h){return m_RegisteredObjects.items.getElement(h);};
        ZenLoad::GEngineClasses::C_ItemReact& getItemReact(ItemReactHandle h){return m_RegisteredObjects.itemReacts.getElement(h);};
        ZenLoad::GEngineClasses::C_Mission& getMission(MissionHandle h){return m_RegisteredObjects.missions.getElement(h);};
        ZenLoad::GEngineClasses::C_Focus& getFocus(FocusHandle h){return m_RegisteredObjects.focuses.getElement(h);};
        ZenLoad::GEngineClasses::C_Info& getInfo(InfoHandle h){return m_RegisteredObjects.infos.getElement(h);};
        ZenLoad::GEngineClasses::Instance* getByClass(ZMemory::BigHandle h, ZenLoad::EInstanceClass instClass);
        RegisteredObjects& getRegisteredObjects(){return m_RegisteredObjects;}

    private:



        RegisteredObjects m_RegisteredObjects;

        /**
         * Daedalus-VM this is running on
         */
        ZenLoad::DaedalusVM& m_VM;
    };
}

