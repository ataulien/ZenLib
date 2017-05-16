#pragma once
#include "DaedalusStdlib.h"
#include "DATFile.h"
#include <utils/staticReferencedAllocator.h>
#include <vector>
#include <list>

constexpr int MAX_NUM_NPCS = 12000;
constexpr int MAX_NUM_ITEMS = 12000;
constexpr int MAX_NUM_MISSIONS = 512;
constexpr int MAX_NUM_INFO = 16000;
constexpr int MAX_NUM_MISC = 1024;
constexpr int MAX_NUM_SFX = 4096; // G2 has 1700
constexpr int MAX_NUM_PFX = 1024;

template <class T>
constexpr size_t MAX_NUM();

template<> constexpr size_t MAX_NUM<Daedalus::GEngineClasses::C_Npc>() { return MAX_NUM_NPCS; }
template<> constexpr size_t MAX_NUM<Daedalus::GEngineClasses::C_Item>() { return MAX_NUM_ITEMS; }


namespace Daedalus
{
    class DaedalusVM;

    namespace GameState
    {
        template <class T>
        using CAllocator = ZMemory::StaticReferencedAllocator<T, MAX_NUM<T>()>;
        template <typename C_Class>
        using CHandle = typename CAllocator<C_Class>::Handle;

        typedef CHandle<Daedalus::GEngineClasses::C_Npc> NpcHandle;
        typedef CHandle<Daedalus::GEngineClasses::C_Item> ItemHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Mission, MAX_NUM_MISSIONS>::Handle MissionHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Focus, MAX_NUM_MISC>::Handle FocusHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_ItemReact, MAX_NUM_MISC>::Handle ItemReactHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Info, MAX_NUM_INFO>::Handle InfoHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Menu, MAX_NUM_MISC>::Handle MenuHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Menu_Item, MAX_NUM_MISC>::Handle MenuItemHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_SFX, MAX_NUM_SFX>::Handle SfxHandle;
        typedef ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_ParticleFX, MAX_NUM_PFX>::Handle PfxHandle;

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
                CAllocator<Daedalus::GEngineClasses::C_Npc> NPCs;
                CAllocator<Daedalus::GEngineClasses::C_Item> items;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_ItemReact, MAX_NUM_MISC> itemReacts;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Mission, MAX_NUM_MISSIONS> missions;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Focus, MAX_NUM_MISC> focuses;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Info, MAX_NUM_INFO> infos;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Menu, MAX_NUM_MISC> menus;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_Menu_Item, MAX_NUM_MISC> menuItems;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_SFX, MAX_NUM_SFX> sfx;
                ZMemory::StaticReferencedAllocator<Daedalus::GEngineClasses::C_ParticleFX, MAX_NUM_PFX> pfx;
                template <class T>
                CAllocator<T>& get(); //  { static_assert(false, "unimplemented template spezialization getter"); }
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
             * @param count How many to give
             * @return Newly created handle or an old one, if the item was stackable
             */
            ItemHandle createInventoryItem(size_t itemSymbol, NpcHandle npc, unsigned int count = 1);

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
            bool removeInventoryItem(size_t itemSymbol, NpcHandle npc, unsigned int count = 1);

            /**
             * Removes the data behind the given item handle
             * @param item Item to remove
             */
            void removeItem(ItemHandle item);
            void removeMenu(MenuHandle menu);
            void removeMenuItem(MenuItemHandle menuItem);

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

            SfxHandle insertSFX(size_t instance);
            SfxHandle insertSFX(const std::string& instance);

            /**
			 * Creates scripting relevant objects
			 */
            template <typename C_Class>
            CHandle<C_Class> create();
            NpcHandle createNPC();
            ItemHandle createItem();
            ItemReactHandle createItemReact();
            MissionHandle createMission();
            InfoHandle createInfo();
            FocusHandle createFocus();
            MenuHandle createMenu();
            MenuItemHandle createMenuItem();
            SfxHandle createSfx();
            PfxHandle createPfx();

            /**
			 * Accessors
			 */
            template <typename C_Class>
            C_Class &get(CHandle<C_Class> h)
            { return m_RegisteredObjects.get<C_Class>().getElement(h); };

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

            Daedalus::GEngineClasses::C_Menu &getMenu(MenuHandle h)
            { return m_RegisteredObjects.menus.getElement(h); };

            Daedalus::GEngineClasses::C_Menu_Item &getMenuItem(MenuItemHandle h)
            { return m_RegisteredObjects.menuItems.getElement(h); };

            Daedalus::GEngineClasses::C_SFX &getSfx(SfxHandle h)
            { return m_RegisteredObjects.sfx.getElement(h); };

            Daedalus::GEngineClasses::C_ParticleFX &getPfx(PfxHandle h)
            { return m_RegisteredObjects.pfx.getElement(h); };

            Daedalus::GEngineClasses::Instance *getByClass(ZMemory::BigHandle h, EInstanceClass instClass);

            RegisteredObjects &getRegisteredObjects()
            { return m_RegisteredObjects; }

            std::map<std::string, LogTopic>& getPlayerLog()
            { return m_PlayerLog; };

            const std::list<ItemHandle>& getInventoryOf(NpcHandle npc)
            { return m_NpcInventories[npc]; }

            /**
             * @param callback Function to be called right after an instance was created
             */
            void setOnInstanceCreatedCallback(const std::function<void(ZMemory::BigHandle, EInstanceClass)>& callback)
            {
                m_OnInstanceCreated = callback;
            }

            /**
             * @param callback Function to be called right before the actual instance gets removed
             */
            void setOnInstanceRemovedCallback(const std::function<void(ZMemory::BigHandle, EInstanceClass)>& callback)
            {
                m_OnInstanceRemoved = callback;
            }
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

            /**
             * Function called when an instance was created/removed
             */
            std::function<void(ZMemory::BigHandle, EInstanceClass)> m_OnInstanceCreated;
            std::function<void(ZMemory::BigHandle, EInstanceClass)> m_OnInstanceRemoved;
        };
        template<> inline CAllocator<Daedalus::GEngineClasses::C_Npc>&
        DaedalusGameState::RegisteredObjects::get<Daedalus::GEngineClasses::C_Npc>() { return NPCs; };
        template<> inline CAllocator<Daedalus::GEngineClasses::C_Item>&
        DaedalusGameState::RegisteredObjects::get<Daedalus::GEngineClasses::C_Item>() { return items; };
    }
}
