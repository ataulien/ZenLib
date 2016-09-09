#pragma once
#include <string>
#include <vector>
#include <map>
#include "zenParser.h"
#include <utils/staticReferencedAllocator.h>

namespace ZenLoad
{

	enum EInstanceClass
	{
		IC_Npc,
		IC_Mission,
		IC_Info,
		IC_Item,
		IC_ItemReact,
		IC_Focus
	};


	enum EParFlag
    {
    	EParFlag_Const    = 1 << 0,
    	EParFlag_Return   = 1 << 1,
    	EParFlag_ClassVar = 1 << 2,
    	EParFlag_External = 1 << 3,
    	EParFlag_Merged   = 1 << 4
    };

    enum EParType
    {
    	EParType_Void       = 0,
    	EParType_Float      = 1,
    	EParType_Int        = 2,
    	EParType_String     = 3,
    	EParType_Class      = 4,
    	EParType_Func       = 5,
    	EParType_Prototype  = 6,
    	EParType_Instance   = 7
    };

    enum EParOp : uint8_t
    {
    	EParOp_Add             = 0,     // a + b
    	EParOp_Subract         = 1,     // a - b
    	EParOp_Multiply        = 2,     // a * b
    	EParOp_Divide          = 3,     // a / b
    	EParOp_Mod             = 4,     // a % b
    	EParOp_BinOr           = 5,     // a | b
    	EParOp_BinAnd          = 6,     // a & b
    	EParOp_Less            = 7,     // a < b
    	EParOp_Greater         = 8,     // a > b
    	EParOp_Assign          = 9,     // a = b
    	EParOp_LogOr           = 11,    // a || b
    	EParOp_LogAnd          = 12,    // a && b
    	EParOp_ShiftLeft       = 13,    // a << b
    	EParOp_ShiftRight      = 14,    // a >> b
    	EParOp_LessOrEqual     = 15,    // a <= b
    	EParOp_Equal           = 16,    // a == b
    	EParOp_NotEqual        = 17,    // a != b
    	EParOp_GreaterOrEqual  = 18,    // a >= b
    	EParOp_AssignAdd       = 19,    // a += b (a = a + b)
    	EParOp_AssignSubtract  = 20,    // a -= b (a = a - b)
    	EParOp_AssignMultiply  = 21,    // a *= b (a = a * b)
    	EParOp_AssignDivide    = 22,    // a /= b (a = a / b)
    	EParOp_Plus            = 30,    // +a
    	EParOp_Minus           = 31,    // -a
    	EParOp_Not             = 32,    // !a
    	EParOp_Negate          = 33,    // ~a
    //	EParOp_LeftBracket     = 40,    // '('
    //	EParOp_RightBracket    = 41,    // ')'
    //	EParOp_Semicolon       = 42,    // ';'
    //	EParOp_Comma           = 43,    // ','
    //	EParOp_CurlyBracket    = 44,    // '{', '}'
    //	EParOp_None            = 45,
    //	EParOp_Float           = 51,
    //	EParOp_Var             = 52,
    //	EParOp_Operator        = 53,
    	EParOp_Ret             = 60,
    	EParOp_Call            = 61,
    	EParOp_CallExternal    = 62,
    //	EParOp_PopInt          = 63,
    	EParOp_PushInt         = 64,
    	EParOp_PushVar         = 65,
    //	EParOp_PushString      = 66,
    	EParOp_PushInstance    = 67,
    //	EParOp_PushIndex       = 68,
    //	EParOp_PopVar          = 69,
    	EParOp_AssignString    = 70,
    	EParOp_AssignStringRef = 71,
    	EParOp_AssignFunc      = 72,
    	EParOp_AssignFloat     = 73,
    	EParOp_AssignInstance  = 74,
    	EParOp_Jump            = 75,
    	EParOp_JumpIf          = 76,
    	EParOp_SetInstance     = 80,
    //	EParOp_Skip            = 90,
    //	EParOp_Label           = 91,
    //	EParOp_Func            = 92,
    //	EParOp_FuncEnd         = 93,
    //	EParOp_Class           = 94,
    //	EParOp_ClassEnd        = 95,
    //	EParOp_Instance        = 96,
    //	EParOp_InstanceEnd     = 97,
    //	EParOp_String          = 98,
    //	EParOp_Array           = 180,  // EParOp_Var + 128
    	EParOp_PushArrayVar    = 245   // EParOp_PushVar + EParOp_Array
    };

    struct PARSymbol
    {
		PARSymbol()
		{
			dataOffset = -1;
			instanceDataHandle.invalidate();
			instanceDataClass = IC_Npc;
		}

        std::string name;

        struct Properties
        {
            int32_t  offClsRet;  // Offset (ClassVar) | Size (Class) | ReturnType (Func)
            struct {
                uint32_t count : 12; // Count:12, Type:4 (EParType_), Flags:6 (EParFlag_), Space: 1, Reserved:9
                uint32_t type  : 4; // EParType_*
                uint32_t flags : 6; // EParFlag_*
                uint32_t space: 1;
                uint32_t reserved : 9;
            }elemProps;

            struct
            {
                uint32_t value : 19;  // Value:19, Reserved:13
                uint32_t reserved : 13;
            }fileIndex;

            struct
            {
                uint32_t value : 19;  // Value:19, Reserved:13
                uint32_t reserved : 13;
            }lineStart;

            struct
            {
                uint32_t value : 19;  // Value:19, Reserved:13
                uint32_t reserved : 13;
            }lineCount;

            struct
            {
                uint32_t value : 24;  // Value:24, Reserved:8
                uint32_t reserved : 8;
            }charStart;

            struct
            {
                uint32_t value : 24;  // Value:24, Reserved:8
                uint32_t reserved : 8;
            }charCount;

        } properties;

        std::vector<float> floatData;
        std::vector<int32_t> intData;
        std::vector<std::string> strData;
        int32_t classOffset;
        uint32_t address;

		int32_t dataOffset; // Not stored in files, only valid for classes to directly write to engine memory
		void* instanceData; // Not stored in files, only valid for classes to directly write to engine memory
		ZMemory::BigHandle instanceDataHandle;
		EInstanceClass instanceDataClass;

        uint32_t parent; // 0xFFFFFFFF (-1) = none

		int32_t* getIntAddr(size_t idx = 0, void* baseAddr=nullptr)
		{
			if(baseAddr && dataOffset != -1)
				return reinterpret_cast<int32_t*>(reinterpret_cast<char*>(baseAddr) + dataOffset + (sizeof(int32_t) * idx));

			if(intData.size() <= idx) intData.resize(idx+1);
			return &intData[idx];
		}

		std::string* getStrAddr(size_t idx = 0, void* baseAddr=nullptr)
		{
			if(baseAddr && dataOffset != -1)
				return reinterpret_cast<std::string*>(reinterpret_cast<char*>(baseAddr) + dataOffset + (sizeof(std::string) * idx));

			if(strData.size() <= idx) strData.resize(idx+1);
			return &strData[idx];
		}

		int32_t getIntValue(size_t idx=0, void* baseAddr=nullptr)
		{
			if(baseAddr && dataOffset != -1)
				return *reinterpret_cast<int32_t*>(reinterpret_cast<char*>(baseAddr) + dataOffset + (sizeof(int32_t) * idx));

			if(intData.size() <= idx) intData.resize(idx+1);
			return intData[idx];
		}

		void set(int32_t v, size_t idx = 0, void* baseAddr=nullptr)
		{
			switch(properties.elemProps.type)
			{
				case EParType_Int:
					if(intData.size() <= idx) intData.resize(idx+1);
					intData[idx] = v;

					if(baseAddr && dataOffset != -1)
						*reinterpret_cast<int32_t*>(reinterpret_cast<char*>(baseAddr) + dataOffset + (sizeof(int32_t) * idx)) = v;
					break;

				case EParType_Float:
					if(floatData.size() <= idx) floatData.resize(idx+1);
					floatData[idx] = *reinterpret_cast<float*>(&v);

					if(baseAddr && dataOffset != -1)
						*reinterpret_cast<float*>(reinterpret_cast<char*>(baseAddr) + dataOffset + (sizeof(float) * idx)) = *reinterpret_cast<float*>(&v);
					break;

				case EParType_Func:
					address = v;

					if(baseAddr && dataOffset != -1)
						*reinterpret_cast<int32_t*>(reinterpret_cast<char*>(baseAddr) + dataOffset + (sizeof(int32_t) * idx)) = v;
					break;
			}
		}

		void set(const std::string& v, size_t idx=0, void* baseAddr=nullptr)
		{
			if(strData.size() <= idx) strData.resize(idx+1);
			strData[idx] = v;

			if(baseAddr && dataOffset != -1)
				*reinterpret_cast<std::string*>(reinterpret_cast<char*>(baseAddr) + dataOffset + (sizeof(float) * idx)) = v;
		}

		bool isDataSame(const PARSymbol& other) const
		{
			return intData == other.intData && floatData == other.floatData && strData == other.strData;
		}
    };

    struct PARSymTable
    {
        std::vector<uint32_t> sortTable;
        std::vector<PARSymbol> symbols;
		std::unordered_map<std::string, size_t> symbolsByName;

    };

    struct PARStack
    {
        size_t stackOffset;
		size_t stackSize;
    };

    struct PARStackOpCode
    {
		EParOp op;

        /**
         * These are valid depending on "op"
         */
        int32_t address; // EParOp_Call, EParOp_Jump, EParOp_JumpIf,
        int32_t symbol;  // EParOp_CallExternal, EParOp_PushVar, EParOp_PushInstance, EParOp_SetInstance, EParOp_PushArrayVar
        int32_t value;   // EParOp_PushInt
        uint8_t index;   // EParOp_PushArrayVar

		size_t opSize; // Size of this operation
    };

	struct PARInstance
	{
		std::map<std::string, uint32_t> symbolsByName;
	};

    class DATFile
    {
    public:
		DATFile();
        DATFile(const std::string& file, bool verbose=false);

        void parseFile();

		/**
		 * @return True, when a symbol with the given name exists
		 */
		bool hasSymbolName(const std::string& symName);

		/**
		 * @return The symbol connected to the given name
		 */
		PARSymbol& getSymbolByName(const std::string& symName);
		size_t getSymbolIndexByName(const std::string& symName);
		PARSymbol& getSymbolByIndex(size_t idx);

		/**
		 * Goes through all symbols and calls the given callback for every instance of the specified class
		 */
		void iterateSymbolsOfClass(const std::string& className, std::function<void(size_t, PARSymbol&)> callback);

		/**
		 * @return Object containing information about the opcode currently on the stack
		 */
		PARStackOpCode getStackOpCode(size_t pc);

		/**
		 * Adds a symbol and returns it's index. Note that if you change the name of this, the new symbol
		 * won't be added to the SymbolByName-Map
		 */
		size_t addSymbol();

		const PARSymTable& getSymTable(){return m_SymTable;}
		const PARStack& getStack(){return m_Stack;}
    private:
        void readSymTable();
		void readStack();

        ZenParser m_Parser;

        PARSymTable m_SymTable;
		PARStack m_Stack;

		bool m_Verbose;
    };

}