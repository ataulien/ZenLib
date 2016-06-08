#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string>

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

        struct Instance
        {
            size_t instanceSymbol;

            /**
             * Space for the user to save something. Will not be touched at all by ZenLib.
             * Note: This is set to nullptr after creation.
             */
            void* userPtr;
        };

        struct C_Npc : Instance
        {
            int32_t id;
            std::string name[5];
            std::string slot;
            int32_t npcType;
            int32_t flags;
            int32_t attribute[ATR_INDEX_MAX];
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
            std::string spawnPoint32_t;
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
            };

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
            int32_t cond_atr[3];
            int32_t cond_value[3];

            // Attribute, die bei anlegen des Items verändert werden
            int32_t change_atr[3];
            int32_t change_value[3];

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
