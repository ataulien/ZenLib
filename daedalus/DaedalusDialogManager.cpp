#include <utils/logger.h>
#include <algorithm>
#include "DaedalusDialogManager.h"
#include "DaedalusVM.h"
#include "DaedalusStdlib.h"

using namespace Daedalus;
using namespace GameState;
using namespace ZenLoad;

DaedalusDialogManager::DaedalusDialogManager(Daedalus::DaedalusVM& vm,const std::string& ou_bin)
    : m_VM(vm),
      m_MessageLib(ou_bin)
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

        // Add to map
        m_NpcInfosByNpcSymbols[info.npc].push_back(h);
    });

    // Messages are in wrong order. Fix this.
    for(auto& v : m_NpcInfosByNpcSymbols)
    {
        std::reverse(v.second.begin(),v.second.end());
    }
}

void DaedalusDialogManager::setNpcInfoKnown(size_t npcInstance, size_t infoInstance)
{
    //LogInfo() << "He knows! (" << m_VM.getDATFile().getSymbolByIndex(npcInstance).name << " -> " << m_VM.getDATFile().getSymbolByIndex(infoInstance).name << ")!";
    m_KnownNpcInfoSymbolsByNpcSymbols[npcInstance].insert(infoInstance);
}

std::vector<InfoHandle> DaedalusDialogManager::getInfos(NpcHandle hnpc)
{
    Daedalus::GEngineClasses::C_Npc& npc = m_VM.getGameState().getNpc(hnpc);
    return m_NpcInfosByNpcSymbols[npc.instanceSymbol];
}

bool DaedalusDialogManager::doesNpcKnowInfo(size_t npcInstance, size_t infoInstance)
{
    const auto& m = m_KnownNpcInfoSymbolsByNpcSymbols[npcInstance];
    return m.find(infoInstance) != m.end();
}







