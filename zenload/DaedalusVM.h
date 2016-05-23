#pragma once

#include <stack>
#include <functional>
#include <queue>
#include <set>
#include "DATFile.h"
#include "DaedalusStdlib.h"

namespace ZenLoad
{
    class DaedalusVM
    {
    public:
        DaedalusVM(DATFile& dat, const std::string& main="main");

        /**
         * @brief Performs a single instruction on the stack
         * @return True, if the program should continue, false if the end was reached
         */
        bool doStack();

        /**
         * @brief Pops an operation from the stack and modifies the program-counter
         */
        PARStackOpCode getCurrentInstruction();

        /**
         * @brief Performs a call-instruction with the given address
         */
        void doCallOperation(uint32_t target);
        void pushCallOperation(uint32_t target);

        /**
         * @brief Reverses the given stack-value
         */
        uint32_t reverseStackValue(uint32_t v);

        /**
         * @brief Registers an External-Function to the VM
         */
        void registerExternalFunction(const std::string& symName, const std::function<void(DaedalusVM&)>& fn);

        /**
         * @brief Pushes an int to the stack
         */
        void pushInt(uint32_t value);
        void pushInt(int32_t value);
        void pushVar(size_t index, uint32_t arrIdx=0);
        void pushVar(const std::string& symName);
        void pushString(const std::string& str);

        void setReturn(int32_t v);
        void setReturn(const std::string& v);
        void setReturn(float v);
        void setReturnVar(int32_t v);

        /**
         * @brief Pops an int from the stack
         */
        uint32_t popDataValue();
        float popFloatValue();
        uint32_t popVar(){uint32_t arr; popVar(arr);}
        uint32_t popVar(uint32_t& arrIdx);
        std::string popString();

        /**
         * @brief Sets the datapointer for the given instance-symbol
         */
        void setInstance(const std::string& instSymbol, void* addr);
        void setCurrentInstance(size_t symIdx);

        /**
         * @brief Sets the
         */

        /**
         * @brief Initializes the given instance by script
         * @param instance Allocated instance of the right type
         * @param symIdx Script-Symbol of the instance to initialize
         */
        void initializeInstance(GEngineClasses::C_NPC* instance, size_t symIdx)
        {
            initializeInstance(instance, symIdx, GEngineClasses::IC_Npc);
        }

        void initializeInstance(GEngineClasses::C_Focus* instance, size_t symIdx)
        {
            initializeInstance(instance, symIdx, GEngineClasses::IC_Focus);
        }

        void initializeInstance(GEngineClasses::C_Item* instance, size_t symIdx)
        {
            initializeInstance(instance, symIdx, GEngineClasses::IC_Item);
        }

        void initializeInstance(GEngineClasses::C_ITEMREACT* instance, size_t symIdx)
        {
            initializeInstance(instance, symIdx, GEngineClasses::IC_ItemReact);
        }

        void initializeInstance(GEngineClasses::C_INFO* instance, size_t symIdx)
        {
            initializeInstance(instance, symIdx, GEngineClasses::IC_Info);
        }

        void initializeInstance(GEngineClasses::C_Mission* instance, size_t symIdx)
        {
            initializeInstance(instance, symIdx, GEngineClasses::IC_Mission);
        }

        /**
         * @return all instances of the given class
         */
        const std::set<void*>& getRegisteredInstancesOf(GEngineClasses::EInstanceClass classIdx){ return m_RegisteredInstances[classIdx]; }

        /**
         * @return The currently registered instance-data pointer
         */
        void* getCurrentInstanceDataPtr(){ return m_CurrentInstanceDataPtr; }

        /**
         * @brief Returns the DAT-File this VM runs on
         */
        DATFile& getDATFile(){return m_DATFile;}
    private:

        /**
         * @brief Initializes the given instance by script
         * @param instance Allocated instance of the right type
         * @param symIdx Script-Symbol of the instance to initialize
         */
        void initializeInstance(void* instance, size_t symIdx, GEngineClasses::EInstanceClass classIdx);

        DATFile m_DATFile;

        /**
         * @brief Program counter
         */
        uint32_t m_PC;

        std::stack<int32_t> m_Stack;
        std::stack<size_t> m_RetStack;

        /**
         * @brief External functions mapped by their symbols index value
         */
        std::unordered_map<size_t, std::function<void(DaedalusVM&)>> m_ExternalsByIndex;

        /**
         * @brief instance set by SetInstance
         */
        size_t m_CurrentInstance;
        void* m_CurrentInstanceDataPtr;

        /**
         * @brief Temporary string symbols for general purpose use
         */
        std::queue<size_t> m_FakeStringSymbols;

        /**
         * @brief Map of all created instances by their class index
         */
        std::map<GEngineClasses::EInstanceClass, std::set<void*>> m_RegisteredInstances;
    };
}