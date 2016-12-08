#pragma once
#include <vector>
#include <string>
#include <functional>
#include <set>
#include <zenload/zCCSLib.h>
#include "DaedalusGameState.h"

namespace ZenLoad
{
    struct oCMsgConversationData;
}

namespace Daedalus
{
    class DaedalusVM;


    namespace GameState
    {
        class DaedalusDialogManager
        {
        public:
            DaedalusDialogManager(Daedalus::DaedalusVM &vm, const std::string &ou_bin);

            /**
			 * Registers the externals used for dialog-processing
			 * @param onAIOutput Callback in case an NPC is saying something (self, target, message)
			 */
            void registerExternals(
                    std::function<void(NpcHandle, NpcHandle, const ZenLoad::oCMsgConversationData &)> onAIOutput,
                    std::function<void(NpcHandle, std::vector<InfoHandle>)> onStartConversation);

            /**
			 * Sets the given info-instance as "known" to the given NPC-Instance
			 */
            void setNpcInfoKnown(size_t npcInstance, size_t infoInstance);

            /**
             * Calls AI_ProcessInfos for the given NPC
             * @param npc NPC to process infos for
             */
            void processInfosFor(NpcHandle npc);

            /**
             * @return Map of NPC-Symbols -> All info they know
             */
            const std::map<size_t, std::set<size_t>>& getKnownNPCInformation(){ return m_KnownNpcInfoSymbolsByNpcSymbols; };
        private:

            /**
			 * Gathers all C_Info-Instances and puts them into m_NpcInfosByNpcSymbols
			 */
            void gatherNpcInformation();

            /**
			 * Message library
			 */
            ZenLoad::zCCSLib m_MessageLib;

            /**
			 * Callback in case an NPC is saying something (self, target, message)
			 */
            std::function<void(NpcHandle, NpcHandle, const ZenLoad::oCMsgConversationData &)> m_OnAIOutput;
			std::function<void(NpcHandle, std::vector<InfoHandle>)> m_OnStartConversation;

            /**
			 * Current daedalus VM
			 */
            Daedalus::DaedalusVM &m_VM;

            /**
			 * Map of NPC-Sym-Handles and their C_Infos
			 */
            std::map<size_t, std::vector<InfoHandle>> m_NpcInfosByNpcSymbols;
            std::map<size_t, std::set<size_t>> m_KnownNpcInfoSymbolsByNpcSymbols;
        };
    }
}