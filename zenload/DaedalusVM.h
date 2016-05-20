#pragma once

#include <stack>
#include <functional>
#include <queue>
#include "DATFile.h"

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
         * @brief Returns the DAT-File this VM runs on
         */
        DATFile& getDATFile(){return m_DATFile;}
    private:
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

        /**
         * @brief Temporary string symbols for general purpose use
         */
        std::queue<size_t> m_FakeStringSymbols;
    };
}