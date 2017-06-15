#pragma once

#include <stack>
#include <functional>
#include <queue>
#include <set>
#include "DATFile.h"
#include "DaedalusStdlib.h"
#include "DaedalusGameState.h"

namespace Daedalus
{
    class DaedalusVM
    {
    public:
        DaedalusVM(const std::string& file, const std::string& main="main", bool registerZenLibExternals=false);

        /**
         * @brief Performs a single instruction on the stack
         * @return True, if the program should continue, false if the end was reached
         */
        bool doStack(bool verbose = false);

        /**
         * @brief Pops an operation from the stack and modifies the program-counter
         */
        PARStackOpCode getCurrentInstruction();

        /**
         * Saves the state of the VM and prepares it for a call to runFunction.
         * You can push function arguments after this call.
         */
        void prepareRunFunction();

        /**
         * Runs a complete function with the arguments given by pushing onto the stack
         * Note: Must be prepared first, using prepareRunFunction.
         * @param symIdx Symbol-index of the function to call
         * @param clearDataStack indicates whether any leftovers from previous runs should be removed (default=true)
         * @return value returned by the function
         */
        int32_t runFunctionBySymIndex(size_t symIdx, bool clearDataStack = true);

        /**
         * @brief sets the program counter
         */
        void setProgramCounter(uint32_t target);

        /**
         * @brief Clears the debug-callstack
         */
        void clearCallStack(){ m_CallStack.clear(); }

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
        void setReturn(float f);
        void setReturnVar(int32_t v);

        /**
         * Pushes/Pops the whole state (PC and RetStack)
         */
        void pushState();
        void popState();

        /**
         * @brief Pops an int from the stack
         */
        template <typename T = int32_t>
        T popDataValue();
        float popFloatValue();
        uint32_t popVar(){uint32_t arr; return popVar(arr); }
        uint32_t popVar(uint32_t& arrIdx);
        std::string popString(bool toUpper = false);

        /**
         * @brief Sets the datapointer for the given instance-symbol
         */
        void setInstance(const std::string& instSymbol, ZMemory::BigHandle h, EInstanceClass instanceClass);
        void setCurrentInstance(size_t symIdx);

        /**
          * @brief Initializes the given instance by script
          * @param instance Allocated instance of the right type
          * @param symIdx Script-Symbol of the instance to initialize
          */
        void initializeInstance(ZMemory::BigHandle instance, size_t symIdx, EInstanceClass classIdx);

        /**
         * @return all instances of the given class
         */
        std::set<void*> getRegisteredInstancesOf(EInstanceClass classIdx){ return std::set<void*>(); }

        /**
         * @return The currently registered instance-data pointer
         */
        void* getCurrentInstanceDataPtr();
        EInstanceClass getCurrentInstanceClass(){ return m_CurrentInstanceClass; }
        ZMemory::BigHandle getCurrentInstanceHandle(){ return m_CurrentInstanceHandle; }

        /**
         * @brief Returns the DAT-File this VM runs on
         */
        DATFile& getDATFile() {return m_DATFile;}

        /**
         * @brief returns the gamestate
         */
        GameState::DaedalusGameState& getGameState() { return m_GameState; }

        /**
         * @return Whether the stack is currently empty
         */
        bool isStackEmpty(){ return m_Stack.empty(); }

        /**
         * @return Callstack in text-form
         */
        std::vector<std::string> getCallStack();

        /**
         * @param callback Function called when a symbol changed its value
         */
        void setOnSymbolValueChangedCallback(const std::function<void(unsigned, EParOp)>& callback)
        {
            m_OnSymbolValueChanged = callback;
        }

        /**
         * Called right before an external call gets triggered. At this moment, parameters pushed to the stack
         * are still there
         * @param callback Function to call
         */
        void setOnExternalCalledCallback(const std::function<void(unsigned)>& callback)
        {
            m_OnExternalCalled = callback;
        }
    private:

        /**
         * pushing/popping FunctionInfo to the debug callstack using RAII concept
         * can be used to keep track of ALL function calls
         */
        class CallStackFrame
        {
        public:
            enum AddressType{
                Address,
                SymbolIndex
            };
            using FunctionInfo = std::pair<size_t, CallStackFrame::AddressType>;

            CallStackFrame(DaedalusVM& vm, size_t addressOrIndex, AddressType addrType, bool xmlLogging = false);
            ~CallStackFrame();
            std::string indent(int add = 0);
        private:
            DaedalusVM& vm;
            bool m_XmlLogging;
        };

        DATFile m_DATFile;

        /**
         * @brief Program counter
         */
        size_t m_PC;

        std::stack<uint32_t> m_Stack;
        // contains pairs of FunctionInfo, Debugging only
        std::vector<CallStackFrame::FunctionInfo> m_CallStack;

        std::string nameFromFunctionInfo(CallStackFrame::FunctionInfo functionInfo);

        /**
         * @brief External functions mapped by their symbols index value
         */
        std::unordered_map<size_t, std::function<void(DaedalusVM&)>> m_ExternalsByIndex;

        /**
         * @brief instance set by SetInstance
         */
        size_t m_CurrentInstance;
        ZMemory::BigHandle m_CurrentInstanceHandle;
        EInstanceClass m_CurrentInstanceClass;

        /**
         * @brief Temporary string symbols for general purpose use
         */
        std::queue<size_t> m_FakeStringSymbols;

        /**
         * @brief Map of all created instances by their class index
         */
        std::map<EInstanceClass, std::set<void*>> m_RegisteredInstances;

        /**
         * @brief current state of the game
         */
        GameState::DaedalusGameState m_GameState;

        struct VMState
        {
            ZMemory::BigHandle m_CurrentInstanceHandle;
            EInstanceClass m_CurrentInstanceClass;
            size_t m_PC;

            std::stack<uint32_t> m_Stack;
            std::vector<size_t> m_CallStack;

            PARSymbol m_Self;
        };
        std::stack<VMState> m_StateStack;

        /**
         * Callback for when a symbol-value changed/External got called
         */
        std::function<void(unsigned, EParOp)> m_OnSymbolValueChanged;
        std::function<void(unsigned)> m_OnExternalCalled;
    };
}
