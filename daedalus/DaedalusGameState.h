#pragma once
#include "DaedalusStdlib.h"
#include "DATFile.h"
#include <utils/staticReferencedAllocator.h>
#include <vector>
#include <list>

const int MAX_NUM_NPCS = 1 << 16;
const int MAX_NUM_ITEMS = 1 << 16;

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

            struct GameExternals
            {
                // These will all be executed with the content already created
                std::function<void(NpcHandle, std::string)> wld_insertnpc;
                std::function<void(NpcHandle)> post_wld_insertnpc;
                std::function<void(ItemHandle)> wld_insertitem;
                std::function<void(ItemHandle, NpcHandle)> createinvitem;
                std::function<int(void)> wld_GetDay;
                std::function<void(std::string)> log_createtopic;
                std::function<void(std::string)> log_settopicstatus;
                std::function<void(std::string, std::string)> log_addentry;
            };
            void setGameExternals(const GameExternals& ext)
            {
                m_GameExternals = ext;
            }

            /**
             * Creates a new instance of the given item first and then adds it to the given NPC
             * @param itemSymbol Symbol to create the instance of
             * @param npc NPC to give the item to
             * @return Newly created handle or an old one, if the item was stackable
             */
            ItemHandle createInventoryItem(size_t itemSymbol, NpcHandle npc);

            /**
             * Adds the given item to the inventory of the given NPC
             * Note: If there already is an instance of this item and it is stackable, the old instance will
             *       be returned and the instance given in 'item' will be removed!
             * @return Handle to the item now in the inventory or an old one, if the item was stackable
             */
            ItemHandle addItemToInventory(ItemHandle item, NpcHandle npc);

            /**
             * Removes one of the items matching the given instance from the NPCs inventory
             * Note: Handle will not be valid anymore if this returns true!
             * @return True, if such an instance was found and removed, false otherwise
             */
            bool removeInventoryItem(size_t itemSymbol, NpcHandle npc);

            /**
             * Creates a new NPC-Instance, just as Wld_InsertNPC was called from script
             * @param instance Instance-Index of the NPC to create
             * @param waypoint Waypoint to place the newly created npc on
             * @return Handle to the script-instance created
             */
            NpcHandle insertNPC(size_t instance, const std::string& waypoint);
            NpcHandle insertNPC(const std::string& instance, const std::string& waypoint);

            /**
             * Creates a new item instance and initializes it (does not position it)
             * @param instance Instance to create
             * @return Handle to the created instance
             */
            ItemHandle insertItem(size_t instance);
            ItemHandle insertItem(const std::string &instance);

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

            const std::list<ItemHandle>& getInventoryOf(NpcHandle npc)
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
            std::map<NpcHandle, std::list<ItemHandle>> m_NpcInventories;

            /**
             * External-callback set by the user
             */
            GameExternals m_GameExternals;
        };
    }

}