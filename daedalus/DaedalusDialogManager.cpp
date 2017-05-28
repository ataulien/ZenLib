#include <utils/logger.h>
#include <algorithm>
#include "DaedalusDialogManager.h"
#include "DaedalusVM.h"
#include "DaedalusStdlib.h"

using namespace Daedalus;
using namespace GameState;
using namespace ZenLoad;

DaedalusDialogManager::DaedalusDialogManager(Daedalus::DaedalusVM& vm,
                                             const std::string& ou_bin,
                                             std::map<size_t, std::set<size_t>>& knownInfos)
    : m_VM(vm),
      m_MessageLib(ou_bin),
      m_KnownNpcInfoSymbolsByNpcSymbols(knownInfos)
{
    gatherNpcInformation();
}

void DaedalusDialogManager::gatherNpcInformation()
{
    m_VM.getDATFile().iterateSymbolsOfClass("C_Info", [&](size_t i, Daedalus::PARSymbol& s){

        // Create new info-object
        InfoHandle h = m_VM.getGameState().createInfo();
        Daedalus::GEngineClasses::C_Info& info = m_VM.getGameState().getInfo(h);
        m_VM.initializeInstance(ZMemory::toBigHandle(h), i, Daedalus::IC_Info);

        // Add to vector
        m_NpcInfos.push_back(h);
    });
}

void DaedalusDialogManager::setNpcInfoKnown(size_t npcInstance, size_t infoInstance)
{
    //LogInfo() << "He knows! (" << m_VM.getDATFile().getSymbolByIndex(npcInstance).name << " -> " << m_VM.getDATFile().getSymbolByIndex(infoInstance).name << ")!";
    m_KnownNpcInfoSymbolsByNpcSymbols[npcInstance].insert(infoInstance);
}

std::vector<InfoHandle> DaedalusDialogManager::getInfos(NpcHandle hnpc)
{
    Daedalus::GEngineClasses::C_Npc& npc = m_VM.getGameState().getNpc(hnpc);
    std::vector<InfoHandle> result;
    for (auto& infoHandle : m_NpcInfos){
        Daedalus::GEngineClasses::C_Info& info = m_VM.getGameState().getInfo(infoHandle);
        if (info.npc == npc.instanceSymbol) {
            result.push_back(infoHandle);
        }
    }
    return result;
}

bool DaedalusDialogManager::doesNpcKnowInfo(size_t npcInstance, size_t infoInstance)
{
    const auto& m = m_KnownNpcInfoSymbolsByNpcSymbols[npcInstance];
    return m.find(infoInstance) != m.end();
}







