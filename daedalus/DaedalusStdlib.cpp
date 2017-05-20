//
// Created by andre on 13.05.16.
//

#include <utils/logger.h>
#include "DaedalusStdlib.h"
#include "DaedalusVM.h"
#include "DATFile.h"

#define REGISTER(cname, obj, var) datFile.registerMember(cname "." #var, obj, obj.var);
#define REGISTER_IF_EXISTS(cname, obj, var) datFile.registerMember(cname "." #var, obj, obj.var, true);

void Daedalus::registerDaedalusStdLib(Daedalus::DaedalusVM& vm, bool enableVerboseLogging)
{
    bool l = enableVerboseLogging;

    vm.registerExternalFunction("inttostring", [l](Daedalus::DaedalusVM& vm){
        int32_t x = vm.popDataValue();

        vm.setReturn(std::to_string(x));
    });

    vm.registerExternalFunction("floattoint", [l](Daedalus::DaedalusVM& vm){
        int32_t x = vm.popDataValue();
        float f = reinterpret_cast<float&>(x);
        vm.setReturn(static_cast<int32_t>(f));
    });

    vm.registerExternalFunction("inttofloat", [l](Daedalus::DaedalusVM& vm){
        int32_t x = vm.popDataValue();
        float f = static_cast<float>(x);
        vm.setReturn(reinterpret_cast<int32_t&>(f));
    });

    vm.registerExternalFunction("concatstrings", [l](Daedalus::DaedalusVM& vm){
        std::string s2 = vm.popString();
        std::string s1 = vm.popString();

        vm.setReturn(s1 + s2);
    });

    vm.registerExternalFunction("hlp_strcmp", [l](Daedalus::DaedalusVM& vm){
        std::string s1 = vm.popString();
        std::string s2 = vm.popString();

        vm.setReturn(s1 == s2 ? 1 : 0);
    });

    vm.registerExternalFunction("hlp_random", [=](Daedalus::DaedalusVM& vm){
        int32_t n0 = vm.popDataValue();

        vm.setReturn(rand() % n0);
    });

    //
}

void Daedalus::registerGothicEngineClasses(DaedalusVM& vm)
{
    GEngineClasses::C_Npc npc;
    GEngineClasses::C_Focus focus;
    GEngineClasses::C_Info info;
    GEngineClasses::C_ItemReact itemreact;
    GEngineClasses::C_Item item;
    GEngineClasses::C_Spell spell;
    GEngineClasses::C_Mission mission;
    GEngineClasses::C_Menu menu;
    GEngineClasses::C_Menu_Item menuItem;
    GEngineClasses::C_SFX sfx;
    GEngineClasses::C_ParticleFX pfx;
    auto& datFile = vm.getDATFile();

    // the vm of the GOTHIC.DAT does not contain C_Menu and MENU.DAT's vm does not contain C_NPC
    // so we need to register only class members of existing classes
    auto classExists = [&vm](const std::string& className){
        bool exists = vm.getDATFile().hasSymbolName(className);
        return exists && (vm.getDATFile().getSymbolByName(className).properties.elemProps.type == EParType_Class);
    };

    if (classExists("C_Npc")){
        REGISTER("C_Npc", npc, id);
        REGISTER("C_Npc", npc, name);
        REGISTER("C_Npc", npc, slot);
        REGISTER_IF_EXISTS("C_Npc", npc, effect);
        REGISTER("C_Npc", npc, npcType);
        REGISTER("C_Npc", npc, flags);
        REGISTER("C_Npc", npc, attribute);
        REGISTER_IF_EXISTS("C_Npc", npc, hitChance);
        REGISTER("C_Npc", npc, protection);
        REGISTER("C_Npc", npc, damage);
        REGISTER("C_Npc", npc, damagetype);
        REGISTER("C_Npc", npc, guild);
        REGISTER("C_Npc", npc, level);
        REGISTER("C_Npc", npc, mission);
        REGISTER("C_Npc", npc, fight_tactic);
        REGISTER("C_Npc", npc, weapon);
        REGISTER("C_Npc", npc, voice);
        REGISTER("C_Npc", npc, voicePitch);
        REGISTER("C_Npc", npc, bodymass);
        REGISTER("C_Npc", npc, daily_routine);
        REGISTER("C_Npc", npc, start_aistate);
        REGISTER("C_Npc", npc, spawnPoint);
        REGISTER("C_Npc", npc, spawnDelay);
        REGISTER("C_Npc", npc, senses);
        REGISTER("C_Npc", npc, senses_range);
        REGISTER("C_Npc", npc, aivar);
        REGISTER("C_Npc", npc, wp);
        REGISTER("C_Npc", npc, exp);
        REGISTER("C_Npc", npc, exp_next);
        REGISTER("C_Npc", npc, lp);
        REGISTER_IF_EXISTS("C_Npc", npc, bodyStateInterruptableOverride);
        REGISTER_IF_EXISTS("C_Npc", npc, noFocus);
    }

    if (classExists("C_Focus")){
        REGISTER("C_Focus", focus, npc_longrange);
        REGISTER("C_Focus", focus, npc_range1);
        REGISTER("C_Focus", focus, npc_range2);
        REGISTER("C_Focus", focus, npc_azi);
        REGISTER("C_Focus", focus, npc_elevdo);
        REGISTER("C_Focus", focus, npc_elevup);
        REGISTER("C_Focus", focus, npc_prio);
        REGISTER("C_Focus", focus, item_range1);
        REGISTER("C_Focus", focus, item_range2);
        REGISTER("C_Focus", focus, item_azi);
        REGISTER("C_Focus", focus, item_elevdo);
        REGISTER("C_Focus", focus, item_elevup);
        REGISTER("C_Focus", focus, item_prio);
        REGISTER("C_Focus", focus, mob_range1);
        REGISTER("C_Focus", focus, mob_range2);
        REGISTER("C_Focus", focus, mob_azi);
        REGISTER("C_Focus", focus, mob_elevdo );
        REGISTER("C_Focus", focus, mob_elevup);
        REGISTER("C_Focus", focus, mob_prio);
    }

    if (classExists("C_Info")){
        REGISTER("C_Info", info, npc);
        REGISTER("C_Info", info, nr);
        REGISTER("C_Info", info, important);
        REGISTER("C_Info", info, condition);
        REGISTER("C_Info", info, information);
        REGISTER("C_Info", info, description);
        REGISTER("C_Info", info, trade);
        REGISTER("C_Info", info, permanent);
    }

    if (classExists("C_ItemReact")){
        REGISTER("C_ItemReact", itemreact, npc);
        REGISTER("C_ItemReact", itemreact, trade_item);
        REGISTER("C_ItemReact", itemreact, trade_amount);
        REGISTER("C_ItemReact", itemreact, requested_cat);
        REGISTER("C_ItemReact", itemreact, requested_item);
        REGISTER("C_ItemReact", itemreact, requested_amount);
        REGISTER("C_ItemReact", itemreact, reaction);
    }

    if (classExists("C_Item")) {
        REGISTER("C_Item", item, id);
        REGISTER("C_Item", item, name );
        REGISTER("C_Item", item, nameID);
        REGISTER("C_Item", item, hp);
        REGISTER("C_Item", item, hp_max);
        REGISTER("C_Item", item, mainflag );
        REGISTER("C_Item", item, flags);
        REGISTER("C_Item", item, weight );
        REGISTER("C_Item", item, value);
        REGISTER("C_Item", item, damageType);
        REGISTER("C_Item", item, damageTotal);
        REGISTER("C_Item", item, damage);
        REGISTER("C_Item", item, wear);
        REGISTER("C_Item", item, protection);
        REGISTER("C_Item", item, nutrition);
        REGISTER("C_Item", item, cond_atr);
        REGISTER("C_Item", item, cond_value);
        REGISTER("C_Item", item, change_atr);
        REGISTER("C_Item", item, change_value);
        REGISTER("C_Item", item, magic   );
        REGISTER("C_Item", item, on_equip    );
        REGISTER("C_Item", item, on_unequip  );
        REGISTER("C_Item", item, on_state);
        REGISTER("C_Item", item, owner   );
        REGISTER("C_Item", item, ownerGuild    );
        REGISTER("C_Item", item, disguiseGuild);
        REGISTER("C_Item", item, visual);
        REGISTER("C_Item", item, visual_change );
        REGISTER_IF_EXISTS("C_Item", item, effect);
        REGISTER("C_Item", item, visual_skin);
        REGISTER("C_Item", item, scemeName);
        REGISTER("C_Item", item, material);
        REGISTER("C_Item", item, munition);
        REGISTER("C_Item", item, spell);
        REGISTER("C_Item", item, range);
        REGISTER("C_Item", item, mag_circle);
        REGISTER("C_Item", item, description);
        REGISTER("C_Item", item, text);
        REGISTER("C_Item", item, count);
        REGISTER_IF_EXISTS("C_Item", item, inv_zbias);
        REGISTER_IF_EXISTS("C_Item", item, inv_rotx);
        REGISTER_IF_EXISTS("C_Item", item, inv_roty);
        REGISTER_IF_EXISTS("C_Item", item, inv_rotz);
        REGISTER_IF_EXISTS("C_Item", item, inv_animate);
    }

    if (classExists("C_Spell")){
        REGISTER_IF_EXISTS("C_Spell", spell, time_per_mana);
        REGISTER_IF_EXISTS("C_Spell", spell, damage_per_level);
        REGISTER_IF_EXISTS("C_Spell", spell, damageType);
        REGISTER_IF_EXISTS("C_Spell", spell, spellType);
        REGISTER_IF_EXISTS("C_Spell", spell, canTurnDuringInvest);
        REGISTER_IF_EXISTS("C_Spell", spell, canChangeTargetDuringInvest);
        REGISTER_IF_EXISTS("C_Spell", spell, isMultiEffect);
        REGISTER_IF_EXISTS("C_Spell", spell, targetCollectAlgo);
        REGISTER_IF_EXISTS("C_Spell", spell, targetCollectType);
        REGISTER_IF_EXISTS("C_Spell", spell, targetCollectRange);
        REGISTER_IF_EXISTS("C_Spell", spell, targetCollectAzi);
        REGISTER_IF_EXISTS("C_Spell", spell, targetCollectElev);
    };

    if (classExists("C_Mission")) {
        REGISTER("C_Mission", mission, name);
        REGISTER("C_Mission", mission, description);
        REGISTER("C_Mission", mission, duration);
        REGISTER("C_Mission", mission, important);
        REGISTER("C_Mission", mission, offerConditions);
        REGISTER("C_Mission", mission, offer);
        REGISTER("C_Mission", mission, successConditions);
        REGISTER("C_Mission", mission, success);
        REGISTER("C_Mission", mission, failureConditions);
        REGISTER("C_Mission", mission, failure);
        REGISTER("C_Mission", mission, obsoleteConditions);
        REGISTER("C_Mission", mission, obsolete);
        REGISTER("C_Mission", mission, running);
    }

    if (classExists("C_Menu")) {
        REGISTER("C_Menu", menu, backPic);
        REGISTER("C_Menu", menu, backWorld);
        REGISTER("C_Menu", menu, posx);
        REGISTER("C_Menu", menu, posy);
        REGISTER("C_Menu", menu, dimx);
        REGISTER("C_Menu", menu, dimy);
        REGISTER("C_Menu", menu, alpha);
        REGISTER("C_Menu", menu, musicTheme);
        REGISTER("C_Menu", menu, eventTimerMSec);
        REGISTER("C_Menu", menu, items);
        REGISTER("C_Menu", menu, flags);
        REGISTER("C_Menu", menu, defaultOutGame);
        REGISTER("C_Menu", menu, defaultInGame);
    }

    if (classExists("C_Menu_Item")) {
        REGISTER("C_Menu_Item", menuItem, fontName);
        REGISTER("C_Menu_Item", menuItem, text);
        REGISTER("C_Menu_Item", menuItem, backPic);
        REGISTER("C_Menu_Item", menuItem, alphaMode);
        REGISTER("C_Menu_Item", menuItem, alpha);
        REGISTER("C_Menu_Item", menuItem, type);
        REGISTER("C_Menu_Item", menuItem, onSelAction);
        REGISTER("C_Menu_Item", menuItem, onSelAction_S);
        REGISTER("C_Menu_Item", menuItem, onChgSetOption);
        REGISTER("C_Menu_Item", menuItem, onChgSetOptionSection);
        REGISTER("C_Menu_Item", menuItem, onEventAction);
        REGISTER("C_Menu_Item", menuItem, posx);
        REGISTER("C_Menu_Item", menuItem, posy);
        REGISTER("C_Menu_Item", menuItem, dimx);
        REGISTER("C_Menu_Item", menuItem, dimy);
        REGISTER("C_Menu_Item", menuItem, sizeStartScale);
        REGISTER("C_Menu_Item", menuItem, flags);
        REGISTER("C_Menu_Item", menuItem, openDelayTime);
        REGISTER("C_Menu_Item", menuItem, openDuration);
        REGISTER("C_Menu_Item", menuItem, userfloat);
        REGISTER("C_Menu_Item", menuItem, userString);
        REGISTER("C_Menu_Item", menuItem, frameSizeX);
        REGISTER("C_Menu_Item", menuItem, frameSizeY);
        REGISTER_IF_EXISTS("C_Menu_Item", menuItem, hideIfOptionSectionSet);
        REGISTER_IF_EXISTS("C_Menu_Item", menuItem, hideIfOptionSet);
        REGISTER_IF_EXISTS("C_Menu_Item", menuItem, hideOnValue);
    }

    if (classExists("C_SFX")) {
        REGISTER("C_SFX", sfx, file);
        REGISTER("C_SFX", sfx, pitchOff);
        REGISTER("C_SFX", sfx, pitchVar);
        REGISTER("C_SFX", sfx, vol);
        REGISTER("C_SFX", sfx, loop);
        REGISTER("C_SFX", sfx, loopStartOffset);
        REGISTER("C_SFX", sfx, loopEndOffset);
        REGISTER("C_SFX", sfx, reverbLevel);
        REGISTER("C_SFX", sfx, pfxName);
    }

    if (classExists("C_ParticleFX")) {
        REGISTER("C_ParticleFX", pfx, ppsValue);
        REGISTER("C_ParticleFX", pfx, ppsScaleKeys_S);
        REGISTER("C_ParticleFX", pfx, ppsIsLooping);
        REGISTER("C_ParticleFX", pfx, ppsIsSmooth);
        REGISTER("C_ParticleFX", pfx, ppsFPS);
        REGISTER("C_ParticleFX", pfx, ppsCreateEm_S);
        REGISTER("C_ParticleFX", pfx, ppsCreateEmDelay);
        REGISTER("C_ParticleFX", pfx, shpType_S);
        REGISTER("C_ParticleFX", pfx, shpFOR_S);
        REGISTER("C_ParticleFX", pfx, shpOffsetVec_S);
        REGISTER("C_ParticleFX", pfx, shpDistribType_S);
        REGISTER("C_ParticleFX", pfx, shpDistribWalkSpeed);
        REGISTER("C_ParticleFX", pfx, shpIsVolume);
        REGISTER("C_ParticleFX", pfx, shpDim_S);
        REGISTER("C_ParticleFX", pfx, shpMesh_S);
        REGISTER("C_ParticleFX", pfx, shpMeshRender_B);
        REGISTER("C_ParticleFX", pfx, shpScaleKeys_S);
        REGISTER("C_ParticleFX", pfx, shpScaleIsLooping);
        REGISTER("C_ParticleFX", pfx, shpScaleIsSmooth);
        REGISTER("C_ParticleFX", pfx, shpScaleFPS);
        REGISTER("C_ParticleFX", pfx, dirMode_S);
        REGISTER("C_ParticleFX", pfx, dirFOR_S);
        REGISTER("C_ParticleFX", pfx, dirModeTargetFOR_S);
        REGISTER("C_ParticleFX", pfx, dirModeTargetPos_S);
        REGISTER("C_ParticleFX", pfx, dirAngleHead);
        REGISTER("C_ParticleFX", pfx, dirAngleHeadVar);
        REGISTER("C_ParticleFX", pfx, dirAngleElev);
        REGISTER("C_ParticleFX", pfx, dirAngleElevVar);
        REGISTER("C_ParticleFX", pfx, velAvg);
        REGISTER("C_ParticleFX", pfx, velVar);
        REGISTER("C_ParticleFX", pfx, lspPartAvg);
        REGISTER("C_ParticleFX", pfx, lspPartVar);
        REGISTER("C_ParticleFX", pfx, flyGravity_S);
        REGISTER("C_ParticleFX", pfx, flyCollDet_B);
        REGISTER("C_ParticleFX", pfx, visName_S);
        REGISTER("C_ParticleFX", pfx, visOrientation_S);
        REGISTER("C_ParticleFX", pfx, visTexIsQuadPoly);
        REGISTER("C_ParticleFX", pfx, visTexAniFPS);
        REGISTER("C_ParticleFX", pfx, visTexAniIsLooping);
        REGISTER("C_ParticleFX", pfx, visTexColorStart_S);
        REGISTER("C_ParticleFX", pfx, visTexColorEnd_S);
        REGISTER("C_ParticleFX", pfx, visSizeStart_S);
        REGISTER("C_ParticleFX", pfx, visSizeEndScale);
        REGISTER("C_ParticleFX", pfx, visAlphaFunc_S);
        REGISTER("C_ParticleFX", pfx, visAlphaStart);
        REGISTER("C_ParticleFX", pfx, visAlphaEnd);
        REGISTER("C_ParticleFX", pfx, trlFadeSpeed);
        REGISTER("C_ParticleFX", pfx, trlTexture_S);
        REGISTER("C_ParticleFX", pfx, trlWidth);
        REGISTER("C_ParticleFX", pfx, mrkFadeSpeed);
        REGISTER("C_ParticleFX", pfx, mrkTexture_S);
        REGISTER("C_ParticleFX", pfx, mrkSize);
        REGISTER_IF_EXISTS("C_ParticleFX", pfx, flockMode);
        REGISTER_IF_EXISTS("C_ParticleFX", pfx, flockStrength);
        REGISTER_IF_EXISTS("C_ParticleFX", pfx, useEmittersFOR);
        REGISTER_IF_EXISTS("C_ParticleFX", pfx, timeStartEnd_S);
        REGISTER_IF_EXISTS("C_ParticleFX", pfx, m_bIsAmbientPFX);
    }
}



