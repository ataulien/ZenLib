#pragma once

namespace ZenLoad
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

        enum EInstanceClass
        {
            IC_Npc,
            IC_Mission,
            IC_Info,
            IC_Item,
            IC_ItemReact,
            IC_Focus
        };

        struct C_NPC
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

        struct C_Mission
        {
            std::string name;        //	Name des Auftrages
            std::string description;
            int duration;        //	Max. Dauer in Tageszeiten
            int important;

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

        struct C_Item
        {
            // Für alle Items
            int id;
            std::string name, nameID;
            int hp, hp_max;

            int mainflag, flags;        //	Hauptflag und weitere Flags
            int weight, value;

            // Für Waffen
            int damageType;        //	Welche Schadensarten
            int damageTotal;
            int damage[DAM_INDEX_MAX];

            // Für Rüstungen
            int wear;
            int protection[PROT_INDEX_MAX];

            // Für Nahrung
            int nutrition;        //	HP-Steigerung bei Nahrung

            // Benötigte Attribute zum Benutzen des Items
            int cond_atr[3];
            int cond_value[3];

            // Attribute, die bei anlegen des Items verändert werden
            int change_atr[3];
            int change_value[3];

            // Parserfunktionen
            uint32_t magic;        //	Parserfunktion zum "Magie Header"
            uint32_t on_equip;        //	Parserfunktion, wenn Item equipped wird.
            uint32_t on_unequip;        //	Parserfunktion, wenn Item unequipped wird.
            uint32_t on_state[4];

            // Besitzer
            uint32_t owner;        //	Besitzer : Instanz-Name
            int ownerGuild;        //	Besitzer : Gilde
            int disguiseGuild;        //	Zur Schau getragene Gilde durch Verkleidung

            // Die 3DS-Datei
            std::string visual;

            // Veränderung des NSC-Meshes beim Anlegen dieses Gegenstandes
            std::string visual_change;        //	ASC - File
            int visual_skin;

            std::string scemeName;
            int material;
            // std::string	pfx								;		//	Magic Weapon PFX
            int munition;        //	Instance of Munition

            int spell;
            int range;

            int mag_circle;

            std::string description;
            std::string text[ITM_TEXT_MAX];
            int count[ITM_TEXT_MAX];
        };

        struct C_Focus
        {
            /// für NSCs
            float npc_longrange;        //	Zurufweite	( 20 m )
            float npc_range1, npc_range2;        //	Reichweite
            float npc_azi;        //	Azimuth		( Seitenwinkel )
            float npc_elevdo, npc_elevup;        //	Elevation	( Höhenwinkel  )
            int npc_prio;        //	Priorität

            /// für ITEMs
            float item_range1, item_range2;        //	Reichweite
            float item_azi;        //	Azimuth		( Seitenwinkel )
            float item_elevdo, item_elevup;        //	Elevation	( Höhenwinkel  )
            int item_prio;        //	Priorität

            /// für MOBs
            float mob_range1, mob_range2;        //	Reichweite
            float mob_azi;        //	Azimuth		( Seitenwinkel )
            float mob_elevdo, mob_elevup;        //	Elevation	( Höhenwinkel  )
            int mob_prio;        //	Priorität
        };

        struct C_INFO
        {
            int npc;
            int nr;
            int important;        //	Wichtig Flag -> ansprechen
            uint32_t condition;
            uint32_t information;
            std::string description;
            int trade;
            int permanent;
        };

        struct C_ITEMREACT
        {
            int npc;
            int trade_item;
            int trade_amount;
            int requested_cat;
            int requested_item;
            int requested_amount;
            uint32_t reaction;
        };
    }

    void registerDaedalusStdLib(DaedalusVM& vm);

    void registerGothicEngineClasses(DaedalusVM& vm);
}
