#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string.h>

namespace Daedalus
{
    class DaedalusVM;

    namespace GEngineClasses
    {
        const int MAX_CHAPTER = 5;
        const int MAX_MISSIONS = 5;
        const int ATR_INDEX_MAX = 8;
        const int ITM_TEXT_MAX = 6;

        const int DAM_INDEX_BARRIER = 0;        //				 nur der Vollstandigkeit und Transparenz wegen hier definiert ( _NICHT_ verwenden )
        const int DAM_INDEX_BLUNT = DAM_INDEX_BARRIER + 1;
        const int DAM_INDEX_EDGE = DAM_INDEX_BLUNT + 1;
        const int DAM_INDEX_FIRE = DAM_INDEX_EDGE + 1;
        const int DAM_INDEX_FLY = DAM_INDEX_FIRE + 1;
        const int DAM_INDEX_MAGIC = DAM_INDEX_FLY + 1;
        const int DAM_INDEX_POINT = DAM_INDEX_MAGIC + 1;
        const int DAM_INDEX_FALL = DAM_INDEX_POINT +
                                   1;        //				 nur der Vollstandigkeit und Transparenz wegen hier definiert ( _NICHT_ verwenden )
        const int DAM_INDEX_MAX = DAM_INDEX_FALL + 1;

        const int PROT_BARRIER = DAM_INDEX_BARRIER;
        const int PROT_BLUNT = DAM_INDEX_BLUNT;
        const int PROT_EDGE = DAM_INDEX_EDGE;
        const int PROT_FIRE = DAM_INDEX_FIRE;
        const int PROT_FLY = DAM_INDEX_FLY;
        const int PROT_MAGIC = DAM_INDEX_MAGIC;
        const int PROT_POINT = DAM_INDEX_POINT;
        const int PROT_FALL = DAM_INDEX_FALL;
        const int PROT_INDEX_MAX = DAM_INDEX_MAX;

        namespace MenuConstants
        {
            const int MAX_USERSTRINGS = 10;
            const int MAX_ITEMS = 150;
            const int MAX_EVENTS = 10;
            const int MAX_SEL_ACTIONS = 5;
            const int MAX_USERVARS = 4;
        }

        struct Instance
        {
			Instance()
			{
				userPtr = nullptr;
				instanceSymbol = 0;
			}

            size_t instanceSymbol;

            /**
             * Space for the user to save something. Will not be touched at all by ZenLib.
             * Note: This is set to nullptr after creation.
             */
            void* userPtr;
        };

        struct C_Menu : Instance
        {
            enum EFlags : int32_t
            {
                MENU_OVERTOP		= 1,
                MENU_EXCLUSIVE		= 2,
                MENU_NOANI			= 4,
                MENU_DONTSCALE_DIM	= 8,
                MENU_DONTSCALE_POS	= 16,
                MENU_ALIGN_CENTER	= 32,
                MENU_SHOW_INFO		= 64,
            };

            C_Menu()
            {
                posx = 0;
                posy = 0;
                alpha = 0;
                eventTimerMSec = 0;
                flags = 0;
                defaultOutGame = 0;
                defaultInGame = 0;
            }

            std::string 	backPic;
            std::string  	backWorld;
            int32_t    	    posx,posy;
            int32_t    	    dimx,dimy;
            int32_t    	    alpha;
            std::string  	musicTheme;
            int32_t    	    eventTimerMSec;
            std::string  	items[MenuConstants::MAX_ITEMS];
            int32_t    	    flags;
            int32_t 	    defaultOutGame;
            int32_t 	    defaultInGame;
        };

        struct C_Menu_Item : Instance
        {
            enum EFlags : int32_t
            {
                IT_CHROMAKEYED		= 1, 
                IT_TRANSPARENT		= 2, 
                IT_SELECTABLE		= 4, 
                IT_MOVEABLE			= 8, 
                IT_TXT_CENTER		= 16, 
                IT_DISABLED			= 32, 
                IT_FADE				= 64, 
                IT_EFFECTS_NEXT		= 128, 
                IT_ONLY_OUT_GAME	= 256, 
                IT_ONLY_IN_GAME		= 512, 
                IT_PERF_OPTION		= 1024, 
                IT_MULTILINE		= 2048, 
                IT_NEEDS_APPLY 		= 4096,  
                IT_NEEDS_RESTART	= 8192,  
                IT_EXTENDED_MENU	= 16384, 
            };

            enum EType : int32_t
            {
                MENU_ITEM_UNDEF		    = 0,
                MENU_ITEM_TEXT		    = 1,
                MENU_ITEM_SLIDER		= 2,
                MENU_ITEM_INPUT		    = 3,
                MENU_ITEM_CURSOR		= 4,
                MENU_ITEM_CHOICEBOX	    = 5,
                MENU_ITEM_BUTTON		= 6,
                MENU_ITEM_LISTBOX		= 7,
            };

            C_Menu_Item()
            {
                alpha = 0;
                type = 0;
                memset(onSelAction, 0, sizeof(onSelAction));
                memset(onEventAction, 0, sizeof(onEventAction));
                posx = 0;
                posy = 0;
                dimx = 0;
                dimy = 0;
                sizeStartScale = 0.0f;
                flags = 0;
                openDelayTime = 0;
                openDuration = 0;
                memset(userfloat, 0, sizeof(userfloat));
                frameSizeX = 0;
                frameSizeY = 0;
            }

            std::string 	fontName;
            std::string 	text[MenuConstants::MAX_USERSTRINGS];
            std::string 	backPic;
            std::string 	alphaMode;
            int32_t    	    alpha;
            int32_t    	    type;
            int32_t    	    onSelAction[MenuConstants::MAX_SEL_ACTIONS];
            std::string 	onSelAction_S[MenuConstants::MAX_SEL_ACTIONS];
            std::string 	onChgSetOption;
            std::string 	onChgSetOptionSection;

            int32_t   	    onEventAction[MenuConstants::MAX_EVENTS];
            int32_t    	    posx,posy;
            int32_t    	    dimx,dimy;		// -1 = AUTODETECT (FONTWISE)
            float  	        sizeStartScale;
            int32_t    	    flags;
            float  	        openDelayTime;
            float  	        openDuration;
            float  	        userfloat [MenuConstants::MAX_USERVARS];
            std::string 	userString[MenuConstants::MAX_USERVARS];
            int32_t		    frameSizeX;
            int32_t		    frameSizeY;
        };

        struct C_Npc : Instance
        {
            enum EAttributes : int32_t
            {
                EATR_HITPOINTS = 0,
                EATR_HITPOINTSMAX = 1,
                EATR_MANA = 2,
                EATR_MANAMAX = 3,
                EATR_STRENGTH = 4,
                EATR_DEXTERITY = 5,
                EATR_REGENERATEHP = 6,
                EATR_REGENERATEMANA = 7,

                EATR_MAX = 8
            };

            enum ENPCFlag : int32_t
            {
                EFLAG_NONE = 0,
                EFLAG_FRIENDS = 1 << 0,
                EFLAG_IMMORTAL = 1 << 1,
                EFLAG_GHOST = 1 << 2,
                EFLAG_PROTECTED = 1 << 10,
            };

			C_Npc()
			{
				id = 0;
				npcType = 0;
				flags = EFLAG_NONE;
				memset(attribute, 0, sizeof(attribute));
				memset(protection, 0, sizeof(protection));
				memset(damage, 0, sizeof(damage));
				memset(mission, 0, sizeof(mission));
				damagetype = 0;
				guild = 0;
				level = 0;
				fight_tactic = 0;
				weapon = 0;
				voice = 0;
				voicePitch = 0;
				bodymass = 0;
				daily_routine = 0;
				start_aistate = 0;
				spawnDelay = 0;
				senses = 0;
				senses_range = 0;
				memset(ai, 0, sizeof(ai));
				exp = 0;
				exp_next = 0;
				lp = 0;
			}

            int32_t id;
            std::string name[5];
            std::string slot;
            int32_t npcType;
            ENPCFlag flags;
            int32_t attribute[EATR_MAX];
            int32_t protection[PROT_INDEX_MAX];
            int32_t damage[DAM_INDEX_MAX];
            int32_t damagetype;
            int32_t guild, level;
            uint32_t mission[MAX_MISSIONS];
            int32_t fight_tactic;
            int32_t weapon;

            int32_t voice;
            int32_t voicePitch;
            int32_t bodymass;
            uint32_t daily_routine;
            uint32_t start_aistate;

            // **********************
            // Spawn
            // **********************
            std::string spawnPoint;
            int32_t spawnDelay;

            // **********************
            // SENSES
            // **********************
            int32_t senses;
            int32_t senses_range;

            // **********************
            // Feel free to use
            // **********************
            int32_t ai[50];
            std::string wp;

            // **********************
            // Experience dependant
            // **********************
            int32_t exp;
            int32_t exp_next;
            int32_t lp;
        };

        struct C_Mission : Instance
        {
            std::string name;        //	Name des Auftrages
            std::string description;
            int32_t duration;        //	Max. Dauer in Tageszeiten
            int32_t important;

            uint32_t offerConditions;
            uint32_t offer;
            uint32_t successConditions;
            uint32_t success;
            uint32_t failureConditions;
            uint32_t failure;
            uint32_t obsoleteConditions;
            uint32_t obsolete;
            uint32_t running;
        };

        struct C_Item : Instance
        {
            // Categories, found in the "mainflag"-field
            enum Categories
            {
                ITM_CAT_NONE			= 1<<0,
                ITM_CAT_NF			    = 1<<1,
                ITM_CAT_FF			    = 1<<2,
                ITM_CAT_MUN			    = 1<<3,
                ITM_CAT_ARMOR			= 1<<4,
                ITM_CAT_FOOD			= 1<<5,
                ITM_CAT_DOCS			= 1<<6,
                ITM_CAT_POTION		    = 1<<7,
                ITM_CAT_LIGHT			= 1<<8,
                ITM_CAT_RUNE			= 1<<9,
                ITM_CAT_MAGIC			= 1<<31,
                ITM_CAT_EQUIPABLE       = ITM_CAT_NF | ITM_CAT_FF | ITM_CAT_ARMOR | ITM_CAT_RUNE | ITM_CAT_MAGIC,
            };

            enum Flags
            {
                ITEM_SWD	  = 1 << 14,		//	Waffe wird wie ein Schwert behandelt
                ITEM_AXE	  = 1 << 15,		//	Waffe wird wie eine Axt behandelt
                ITEM_2HD_SWD  = 1 << 16,		//	Waffe wird wie ein Zweihänder behandelt
                ITEM_2HD_AXE  = 1 << 17,		//	Waffe wird wie eine Streitaxt behandelt
                ITEM_BOW	  = 1 << 19,		//	Waffe wird wie ein Bogen behandelt
                ITEM_CROSSBOW = 1 << 20,		//	Waffe wird wie eine Armbrust behandelt
                ITEM_AMULET	  = 1 << 22,
                ITEM_RING	  = 1 << 11,
            };

            C_Item()
                    : on_state{},
                      count{},
                      cond_atr{},
                      cond_value{},
                      change_atr{},
                      change_value{},
                      protection{},
                      damage{}
            {
                id = 0;
                hp = 0;
                hp_max = 0;
                mainflag = 0;
                flags = 0;
                weight = 0;
                value = 0;
                damageType = 0;
                damageTotal = 0;
                wear = 0;
                nutrition = 0;
                magic = 0;
                on_equip = 0;
                on_unequip = 0;
                owner = 0;
                ownerGuild = 0;
                disguiseGuild = 0;
                visual_skin = 0;
                material = 0;
                munition = 0;
                spell = 0;
                range = 0;
                mag_circle = 0;
            }

            // Für alle Items
            int32_t id;
            std::string name, nameID;
            int32_t hp, hp_max;

            int32_t mainflag, flags;        //	Hauptflag und weitere Flags
            int32_t weight, value;

            // Für Waffen
            int32_t damageType;        //	Welche Schadensarten
            int32_t damageTotal;
            int32_t damage[DAM_INDEX_MAX];

            // Für Rüstungen
            int32_t wear;
            int32_t protection[PROT_INDEX_MAX];

            // Für Nahrung
            int32_t nutrition;        //	HP-Steigerung bei Nahrung

            // Benötigte Attribute zum Benutzen des Items
            enum { COND_ATR_MAX = 3 };
            int32_t cond_atr[COND_ATR_MAX];
            int32_t cond_value[COND_ATR_MAX];

            // Attribute, die bei anlegen des Items verändert werden
            enum { CHANGE_ATR_MAX = 3 };
            int32_t change_atr[CHANGE_ATR_MAX];
            int32_t change_value[CHANGE_ATR_MAX];

            // Parserfunktionen
            uint32_t magic;        //	Parserfunktion zum "Magie Header"
            uint32_t on_equip;        //	Parserfunktion, wenn Item equipped wird.
            uint32_t on_unequip;        //	Parserfunktion, wenn Item unequipped wird.
            uint32_t on_state[4];

            // Besitzer
            uint32_t owner;        //	Besitzer : Instanz-Name
            int32_t ownerGuild;        //	Besitzer : Gilde
            int32_t disguiseGuild;        //	Zur Schau getragene Gilde durch Verkleidung

            // Die 3DS-Datei
            std::string visual;

            // Veränderung des NSC-Meshes beim Anlegen dieses Gegenstandes
            std::string visual_change;        //	ASC - File
            int32_t visual_skin;

            std::string scemeName;
            int32_t material;
            // std::string	pfx								;		//	Magic Weapon PFX
            int32_t munition;        //	Instance of Munition

            int32_t spell;
            int32_t range;

            int32_t mag_circle;

            std::string description;
            std::string text[ITM_TEXT_MAX];
            int32_t count[ITM_TEXT_MAX];
        };

        struct C_Focus : Instance
        {
            /// für NSCs
            float npc_longrange;        //	Zurufweite	( 20 m )
            float npc_range1, npc_range2;        //	Reichweite
            float npc_azi;        //	Azimuth		( Seitenwinkel )
            float npc_elevdo, npc_elevup;        //	Elevation	( Höhenwinkel  )
            int32_t npc_prio;        //	Priorität

            /// für ITEMs
            float item_range1, item_range2;        //	Reichweite
            float item_azi;        //	Azimuth		( Seitenwinkel )
            float item_elevdo, item_elevup;        //	Elevation	( Höhenwinkel  )
            int32_t item_prio;        //	Priorität

            /// für MOBs
            float mob_range1, mob_range2;        //	Reichweite
            float mob_azi;        //	Azimuth		( Seitenwinkel )
            float mob_elevdo, mob_elevup;        //	Elevation	( Höhenwinkel  )
            int32_t mob_prio;        //	Priorität
        };

        struct C_Info : Instance
        {
            int32_t npc;
            int32_t nr;
            int32_t important;        //	Wichtig Flag -> ansprechen
            uint32_t condition;
            uint32_t information;
            std::string description;
            int32_t trade;
            int32_t permanent;
        };

        struct C_ItemReact : Instance
        {
            int32_t npc;
            int32_t trade_item;
            int32_t trade_amount;
            int32_t requested_cat;
            int32_t requested_item;
            int32_t requested_amount;
            uint32_t reaction;
        };
    }

    /**
     * @brief Implements some generic script-externals
     */
    void registerDaedalusStdLib(DaedalusVM& vm, bool enableVerboseLogging = false);

    /**
     * @brief Links the classes known to the engine to the VM.
     */
    void registerGothicEngineClasses(DaedalusVM& vm);
}
