#pragma once
#include "DaedalusStdlib.h"
#include "DATFile.h"
#include <utils/staticReferencedAllocator.h>
#include <vector>

const int MAX_NUM_NPCS = 2000;
const int MAX_NUM_ITEMS = 4000;

namespace Daedalus
{
    class DaedalusVM;

    namespace GameState
    {
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Npc, MAX_NUM_NPCS>::Handle NpcHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Item, MAX_NUM_ITEMS>::Handle ItemHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Mission, MAX_NUM_ITEMS>::Handle MissionHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Focus, MAX_NUM_ITEMS>::Handle FocusHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_ItemReact, MAX_NUM_ITEMS>::Handle ItemReactHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Info, MAX_NUM_ITEMS>::Handle InfoHandle;

        struct LogTopic
        {
            enum ELogStatus
            {
                // TODO: Find the actual values for this
                LS_Running = 1,
                LS_Success = 2,
                LS_Failed = 3,
                LS_Obsolete = 4
            };

            enum ESection
            {
                // TODO: Find the actual values for this!
                LT_Mission = 0,
                LT_Note = 1
            };

            std::string topic;
            std::vector<std::string> entries;

            ELogStatus status;
            ESection section;
        };

        /**
		 * Class holding the current engine-side gamestate of a daedalus-VM
		 */
        class DaedalusGameState
        {
        public:

            struct RegisteredObjects
            {
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Npc, MAX_NUM_NPCS> NPCs;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Item, MAX_NUM_ITEMS> items;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_ItemReact, MAX_NUM_ITEMS> itemReacts;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Mission, MAX_NUM_ITEMS> missions;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Focus, MAX_NUM_ITEMS> focuses;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Info, MAX_NUM_ITEMS> infos;
            };

            DaedalusGameState(Daedalus::DaedalusVM &vm);

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
            Daedalus::GEngineClasses::C_Npc &getNpc(NpcHandle h)
            { return m_RegisteredObjects.NPCs.getElement(h); };

            Daedalus::GEngineClasses::C_Item &getItem(ItemHandle h)
            { return m_RegisteredObjects.items.getElement(h); };

            Daedalus::GEngineClasses::C_ItemReact &getItemReact(ItemReactHandle h)
            { return m_RegisteredObjects.itemReacts.getElement(h); };

            Daedalus::GEngineClasses::C_Mission &getMission(MissionHandle h)
            { return m_RegisteredObjects.missions.getElement(h); };

            Daedalus::GEngineClasses::C_Focus &getFocus(FocusHandle h)
            { return m_RegisteredObjects.focuses.getElement(h); };

            Daedalus::GEngineClasses::C_Info &getInfo(InfoHandle h)
            { return m_RegisteredObjects.infos.getElement(h); };

            Daedalus::GEngineClasses::Instance *getByClass(ZMemory::BigHandle h, EInstanceClass instClass);

            RegisteredObjects &getRegisteredObjects()
            { return m_RegisteredObjects; }

            const std::map<std::string, LogTopic>& getPlayerLog()
            { return m_PlayerLog; };

            const std::vector<ItemHandle>& getInventoryOf(NpcHandle npc)
            { return m_NpcInventories[npc]; }

        private:


            RegisteredObjects m_RegisteredObjects;

            /**
			 * Daedalus-VM this is running on
			 */
            Daedalus::DaedalusVM &m_VM;

            /**
             * @brief The players log. Map of Entries by Topics
             */
            std::map<std::string, LogTopic> m_PlayerLog;

            /**
             * Inventories by npc handle
             */
            std::map<NpcHandle, std::vector<ItemHandle>> m_NpcInventories;


        };
    }

}