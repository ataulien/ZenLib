#pragma once
#include "DaedalusStdlib.h"
#include "DATFile.h"
#include <utils/staticReferencedAllocator.h>
#include <vector>
#include <list>


namespace Daedalus
{
    class DaedalusVM;

    namespace GameState
    {
        constexpr int MAX_NUM_MISC = 1024;

        constexpr int MAX_NUM_NPCS = 12000;
        constexpr int MAX_NUM_ITEMS = 12000;
        constexpr int MAX_NUM_MISSIONS = 512;
        constexpr int MAX_NUM_FOCUS = MAX_NUM_MISC;
        constexpr int MAX_NUM_ITEMREACT = MAX_NUM_MISC;
        constexpr int MAX_NUM_INFO = 16000;
        constexpr int MAX_NUM_MENU = MAX_NUM_MISC;
        constexpr int MAX_NUM_MENUITEM = MAX_NUM_MISC;
        constexpr int MAX_NUM_SFX = 4096; // G2 has 1700
        constexpr int MAX_NUM_PFX = 1024;
        
        template <class T>
        constexpr size_t MAX_NUM();

        template<> constexpr size_t MAX_NUM<Daedalus::GEngineClasses::C_Npc>() { return MAX_NUM_NPCS; }
        template<> constexpr size_t MAX_NUM<Daedalus::GEngineClasses::C_Item>() { return MAX_NUM_ITEMS; }
        template<> constexpr size_t MAX_NUM<Daedalus::GEngineClasses::C_Mission>() { return MAX_NUM_MISSIONS; }
        template<> constexpr size_t MAX_NUM<Daedalus::GEngineClasses::C_Focus>() { return MAX_NUM_FOCUS; }
        template<> constexpr size_t MAX_NUM<Daedalus::GEngineClasses::C_ItemReact>() { return MAX_NUM_ITEMREACT; }
        template<> constexpr size_t MAX_NUM<Daedalus::GEngineClasses::C_Info>() { return MAX_NUM_INFO; }
        template<> constexpr size_t MAX_NUM<Daedalus::GEngineClasses::C_Menu>() { return MAX_NUM_MENU; }
        template<> constexpr size_t MAX_NUM<Daedalus::GEngineClasses::C_Menu_Item>() { return MAX_NUM_MENUITEM; }
        template<> constexpr size_t MAX_NUM<Daedalus::GEngineClasses::C_SFX>() { return MAX_NUM_SFX; }
        template<> constexpr size_t MAX_NUM<Daedalus::GEngineClasses::C_ParticleFX>() { return MAX_NUM_PFX; }

        template <typename C_Class>
        using CHandle = typename ZMemory::StaticReferencedAllocator<C_Class, MAX_NUM<C_Class>()>::Handle;

        typedef CHandle<Daedalus::GEngineClasses::C_Npc> NpcHandle;
        typedef CHandle<Daedalus::GEngineClasses::C_Item> ItemHandle;
        typedef CHandle<Daedalus::GEngineClasses::C_Mission> MissionHandle;
        typedef CHandle<Daedalus::GEngineClasses::C_Focus> FocusHandle;
        typedef CHandle<Daedalus::GEngineClasses::C_ItemReact> ItemReactHandle;
        typedef CHandle<Daedalus::GEngineClasses::C_Info> InfoHandle;
        typedef CHandle<Daedalus::GEngineClasses::C_Menu> MenuHandle;
        typedef CHandle<Daedalus::GEngineClasses::C_Menu_Item> MenuItemHandle;
        typedef CHandle<Daedalus::GEngineClasses::C_SFX> SfxHandle;
        typedef CHandle<Daedalus::GEngineClasses::C_ParticleFX> PfxHandle;

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

        struct RegisteredObjects
        {
            template <class T, size_t n = MAX_NUM<T>()>
            using CAllocator = ZMemory::StaticReferencedAllocator<T, n>;

            CAllocator<GEngineClasses::C_Npc> NPCs;
            CAllocator<GEngineClasses::C_Item> items;
            CAllocator<GEngineClasses::C_ItemReact> itemReacts;
            CAllocator<GEngineClasses::C_Mission> missions;
            CAllocator<GEngineClasses::C_Focus> focuses;
            CAllocator<GEngineClasses::C_Info> infos;
            CAllocator<GEngineClasses::C_Menu> menus;
            CAllocator<GEngineClasses::C_Menu_Item> menuItems;
            CAllocator<GEngineClasses::C_SFX> sfx;
            CAllocator<GEngineClasses::C_ParticleFX> pfx;

            /**
             * generic getter for the Engine containers NPCs, items, ...
             * @tparam C_Class i.e. C_Npc C_Item, ...
             */
            template <class C_Class, size_t n = MAX_NUM<C_Class>()>
            CAllocator<C_Class, n>& get() { C_Class::missing_template_function_spezialization; };
        };

        /**
         * getter spezializations
         */
        template<> inline RegisteredObjects::CAllocator<GEngineClasses::C_Npc>&
        RegisteredObjects::get<GEngineClasses::C_Npc, MAX_NUM<GEngineClasses::C_Npc>()>() { return NPCs; };
        template<> inline RegisteredObjects::CAllocator<GEngineClasses::C_Item>&
        RegisteredObjects::get<GEngineClasses::C_Item, MAX_NUM<GEngineClasses::C_Item>()>() { return items; };
        template<> inline RegisteredObjects::CAllocator<GEngineClasses::C_Mission>&
        RegisteredObjects::get<GEngineClasses::C_Mission, MAX_NUM<GEngineClasses::C_Mission>()>() { return missions; };
        template<> inline RegisteredObjects::CAllocator<GEngineClasses::C_Focus>&
        RegisteredObjects::get<GEngineClasses::C_Focus, MAX_NUM<GEngineClasses::C_Focus>()>() { return focuses; };
        template<> inline RegisteredObjects::CAllocator<GEngineClasses::C_ItemReact>&
        RegisteredObjects::get<GEngineClasses::C_ItemReact, MAX_NUM<GEngineClasses::C_ItemReact>()>() { return itemReacts; };
        template<> inline RegisteredObjects::CAllocator<GEngineClasses::C_Info>&
        RegisteredObjects::get<GEngineClasses::C_Info, MAX_NUM<GEngineClasses::C_Info>()>() { return infos; };
        template<> inline RegisteredObjects::CAllocator<GEngineClasses::C_Menu>&
        RegisteredObjects::get<GEngineClasses::C_Menu, MAX_NUM<GEngineClasses::C_Menu>()>() { return menus; };
        template<> inline RegisteredObjects::CAllocator<GEngineClasses::C_Menu_Item>&
        RegisteredObjects::get<GEngineClasses::C_Menu_Item, MAX_NUM<GEngineClasses::C_Menu_Item>()>() { return menuItems; };
        template<> inline RegisteredObjects::CAllocator<GEngineClasses::C_SFX>&
        RegisteredObjects::get<GEngineClasses::C_SFX, MAX_NUM<GEngineClasses::C_SFX>()>() { return sfx; };
        template<> inline RegisteredObjects::CAllocator<GEngineClasses::C_ParticleFX>&
        RegisteredObjects::get<GEngineClasses::C_ParticleFX, MAX_NUM<GEngineClasses::C_ParticleFX>()>() { return pfx; };

        /**
		 * Class holding the current engine-side gamestate of a daedalus-VM
         */
        class DaedalusGameState
        {
        public:

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
                std::function<void(NpcHandle)> wld_removenpc;
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
            void removeNPC(NpcHandle npc);
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
            inline C_Class& get(CHandle<C_Class> h)
            { return m_RegisteredObjects.get<C_Class>().getElement(h); };

            Daedalus::GEngineClasses::C_Npc& getNpc(NpcHandle h)
            { return get<Daedalus::GEngineClasses::C_Npc>(h); };

            Daedalus::GEngineClasses::C_Item& getItem(ItemHandle h)
            { return get<Daedalus::GEngineClasses::C_Item>(h); };

            Daedalus::GEngineClasses::C_ItemReact& getItemReact(ItemReactHandle h)
            { return get<Daedalus::GEngineClasses::C_ItemReact>(h); };

            Daedalus::GEngineClasses::C_Mission& getMission(MissionHandle h)
            { return get<Daedalus::GEngineClasses::C_Mission>(h); };

            Daedalus::GEngineClasses::C_Focus& getFocus(FocusHandle h)
            { return get<Daedalus::GEngineClasses::C_Focus>(h); };

            Daedalus::GEngineClasses::C_Info& getInfo(InfoHandle h)
            { return get<Daedalus::GEngineClasses::C_Info>(h); };

            Daedalus::GEngineClasses::C_Menu& getMenu(MenuHandle h)
            { return get<Daedalus::GEngineClasses::C_Menu>(h); };

            Daedalus::GEngineClasses::C_Menu_Item& getMenuItem(MenuItemHandle h)
            { return get<Daedalus::GEngineClasses::C_Menu_Item>(h); };

            Daedalus::GEngineClasses::C_SFX& getSfx(SfxHandle h)
            { return get<Daedalus::GEngineClasses::C_SFX>(h); };

            Daedalus::GEngineClasses::C_ParticleFX& getPfx(PfxHandle h)
            { return get<Daedalus::GEngineClasses::C_ParticleFX>(h); };

            Daedalus::GEngineClasses::Instance *getByClass(ZMemory::BigHandle h, EInstanceClass instClass);

            RegisteredObjects& getRegisteredObjects()
            { return m_RegisteredObjects; }

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
    }
}