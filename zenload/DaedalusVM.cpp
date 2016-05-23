//
// Created by andre on 09.05.16.
//

#include <utils/logger.h>
#include <map>
#include <assert.h>
#include "DaedalusVM.h"
#include "DATFile.h"

extern std::map<int, const char*> OP_MAP;

const int NUM_FAKE_STRING_SYMBOLS = 5;

using namespace ZenLoad;

DaedalusVM::DaedalusVM(ZenLoad::DATFile& dat, const std::string& main)
{
    m_PC = 0;
    m_DATFile = dat;

    // See if we find a "main"-function
    if(m_DATFile.hasSymbolName(main))
    {
        const PARSymbol& s = m_DATFile.getSymbolByName(main);

        LogInfo() << "Found main-function at Address: " << s.address;
        m_PC = s.address;
    }else
    {
        LogInfo() << "Did not find main-function!";
    }

    // Make fake-strings
    for(size_t i=0;i<NUM_FAKE_STRING_SYMBOLS;i++)
        m_FakeStringSymbols.push(m_DATFile.addSymbol());

    m_CurrentInstanceDataPtr = nullptr;
}

bool DaedalusVM::doStack()
{
    bool log = true;
    size_t oldPC = m_PC;
    PARStackOpCode op = getCurrentInstruction();

    int32_t a;
    int32_t b;
    uint32_t arr, arr2;
    int32_t* addr;
    std::string* straddr, *straddr2;
    static PARSymbol s;
    PARSymbol& sym = s;
    PARSymbol& sym2 = s;

    if(log) LogInfo() << oldPC << ": " << OP_MAP[op.op];

    switch(op.op)
    {

        case EParOp_Add: pushInt(popDataValue() + popDataValue()); break;
        case EParOp_Subract: pushInt(popDataValue() - popDataValue()); break;
        case EParOp_Multiply: pushInt(popDataValue() * popDataValue()); break;
        case EParOp_Divide: pushInt(popDataValue() / popDataValue()); break;
        case EParOp_Mod: pushInt(popDataValue() % popDataValue()); break;
        case EParOp_BinOr: pushInt(popDataValue() | popDataValue()); break;
        case EParOp_BinAnd: pushInt(popDataValue() & popDataValue()); break;
        case EParOp_Less: pushInt(popDataValue() < popDataValue() ? 1 : 0); break;
        case EParOp_Greater: pushInt(popDataValue() > popDataValue() ? 1 : 0); break;
        case EParOp_Assign:
            a = popVar(arr);

            LogInfo() << " - a: " << a << ", arr: " << arr;

            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr, m_CurrentInstanceDataPtr);
            b = *addr;
            *addr = popDataValue();
            LogInfo() << " - " << b << " -> " << *addr;
            break;
        case EParOp_LogOr:pushInt(popDataValue() || popDataValue() ? 1 : 0);break;
        case EParOp_LogAnd:pushInt(popDataValue() && popDataValue() ? 1 : 0);break;
        case EParOp_ShiftLeft:pushInt(popDataValue() << popDataValue());break;
        case EParOp_ShiftRight:pushInt(popDataValue() << popDataValue());break;
        case EParOp_LessOrEqual:pushInt(popDataValue() <= popDataValue() ? 1 : 0);break;
        case EParOp_Equal:pushInt(popDataValue() == popDataValue() ? 1 : 0);break;
        case EParOp_NotEqual:pushInt(popDataValue() != popDataValue() ? 1 : 0);break;
        case EParOp_GreaterOrEqual:pushInt(popDataValue() >= popDataValue() ? 1 : 0);break;
        case EParOp_AssignAdd:
            a = popVar(arr);
            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr, m_CurrentInstanceDataPtr);
            *addr += popDataValue();
            break;
        case EParOp_AssignSubtract:
            a = popVar(arr);
            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr, m_CurrentInstanceDataPtr);
            *addr -= popDataValue();
            break;
        case EParOp_AssignMultiply:
            a = popVar(arr);
            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr, m_CurrentInstanceDataPtr);
            *addr *= popDataValue();
            break;
        case EParOp_AssignDivide:
            a = popVar(arr);
            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr, m_CurrentInstanceDataPtr);
            *addr /= popDataValue();
            break;
        case EParOp_Plus:
            pushInt(+popDataValue());
            break;
        case EParOp_Minus:
            pushInt(-popDataValue());
            break;
        case EParOp_Not:
            pushInt(~popDataValue());
            break;
        case EParOp_Negate:
            pushInt(!popDataValue());
            break;
        case EParOp_Ret:
            if(m_RetStack.empty())
                return false; // Program ended

            m_PC = static_cast<uint32_t>(m_RetStack.top());
            m_RetStack.pop();
            break;
        case EParOp_Call:
        {
            LogInfo() << oldPC <<" - CALL: " << op.address;
            // FIXME: Support actual externals
            m_RetStack.push(m_PC);
            m_PC = op.address;
        }
            break;

        case EParOp_CallExternal: {
            auto it = m_ExternalsByIndex.find(op.symbol);

            LogInfo() << " - Function: " << m_DATFile.getSymbolByIndex(op.symbol).name;
            if (it != m_ExternalsByIndex.end()) {
                (*it).second(*this);
            }
        }
            break;

        case EParOp_PushInt: pushInt(op.value); LogInfo() << " - " << op.value; break;
        case EParOp_PushVar:
            pushVar(op.symbol);

            // Push second time just for debugging
            pushVar(op.symbol);
            LogInfo() << " - [" << op.symbol << "] " << m_DATFile.getSymbolByIndex(op.symbol).name << ": " << popDataValue();
            break;
        case EParOp_PushInstance:
            pushVar(op.symbol);
            LogInfo() << " - [" << op.symbol << "] " << m_DATFile.getSymbolByIndex(op.symbol).name << ", addr: " << m_DATFile.getSymbolByIndex(op.symbol).instanceData;
            break; //TODO: Not sure about this
        case EParOp_AssignString:
            a = popVar(arr);
            b = popVar(arr2);
            straddr = m_DATFile.getSymbolByIndex(a).getStrAddr(arr, m_CurrentInstanceDataPtr);
            *straddr = *m_DATFile.getSymbolByIndex(b).getStrAddr(arr2, m_CurrentInstanceDataPtr);

            LogInfo() << " - " << a << ", " << b << ": " << *m_DATFile.getSymbolByIndex(b).getStrAddr(arr2, m_CurrentInstanceDataPtr);
            break;
        case EParOp_AssignStringRef:
            LogError() << "EParOp_AssignStringRef not implemented!";
            break;
        case EParOp_AssignFunc:
            a = popDataValue();
            b = popDataValue(); // TODO: Maybe should be using popVar here

            sym2 = m_DATFile.getSymbolByIndex(b);
            m_DATFile.getSymbolByIndex(a).set(sym2.address, 0, m_CurrentInstanceDataPtr);
            break;

        case EParOp_AssignFloat:
            a = popVar(arr);
            b = popDataValue();

            m_DATFile.getSymbolByIndex(a).set(b, arr, m_CurrentInstanceDataPtr);
            break;

        case EParOp_AssignInstance:
            a = popVar();
            b = popVar();



            m_DATFile.getSymbolByIndex(a).instanceData = m_DATFile.getSymbolByIndex(b).instanceData;

            //sym.classOffset = sym2.classOffset;
            //sym.instanceData = sym2.instanceData;

            LogInfo() << "AssignInstance: a=" << a << ", b=" << b;
            LogInfo() << " - [" << a << "] " << m_DATFile.getSymbolByIndex(a).name << ", addr: " << m_DATFile.getSymbolByIndex(a).instanceData;

            break;

        case EParOp_Jump:
            m_PC = op.address;
            break;

        case EParOp_JumpIf:
            a = popDataValue();
            if(!a) // TODO: They seem to jump at 0, rather than 1
                m_PC = op.address;

            LogInfo() << " - " << a << ", " << oldPC << " -> " << m_PC;
            break;

        case EParOp_SetInstance:
            m_CurrentInstance = op.symbol;
            LogInfo() << " - [" << op.symbol << "] " << m_DATFile.getSymbolByIndex(op.symbol).name;

            m_CurrentInstanceDataPtr = m_DATFile.getSymbolByIndex(op.symbol).instanceData;
            break;
        case EParOp_PushArrayVar:
            pushVar(op.symbol, op.index);
            LogInfo() << " - [" << op.symbol << "] " << m_DATFile.getSymbolByIndex(op.symbol).name << ", Index:" << (int)op.index;
            break;
    }


    return true;
}

PARStackOpCode DaedalusVM::getCurrentInstruction()
{
    PARStackOpCode op = m_DATFile.getStackOpCode(m_PC);
    m_PC += op.opSize;

    return op;
}

void DaedalusVM::doCallOperation(uint32_t target)
{
    m_RetStack.push(m_PC);
    m_PC = target;
}

uint32_t DaedalusVM::reverseStackValue(uint32_t v)
{
    return m_DATFile.getStack().stackSize - v - 1;
}

void DaedalusVM::registerExternalFunction(const std::string& symName, const std::function<void(DaedalusVM&)>& fn)
{
    if(m_DATFile.hasSymbolName(symName))
    {
        size_t s = m_DATFile.getSymbolIndexByName(symName);
        m_ExternalsByIndex[s] = fn;
    }
}

void DaedalusVM::pushInt(uint32_t value)
{
    m_Stack.push(value);
    m_Stack.push(EParOp_PushInt);
}

void DaedalusVM::pushInt(int32_t value)
{
    pushInt(static_cast<uint32_t>(value));
}

uint32_t DaedalusVM::popDataValue()
{
    assert(!m_Stack.empty());

    uint32_t tok = m_Stack.top();
    m_Stack.pop(); // Pop token

    uint32_t v = m_Stack.top();
    m_Stack.pop(); // Pop value

    uint32_t arrIdx = 0;
    switch(tok)
    {

        case EParOp_PushInt:
            return v;

        case EParOp_PushVar:
            arrIdx = m_Stack.top();
            m_Stack.pop(); // Pop array index
            return m_DATFile.getSymbolByIndex(v).getIntValue(arrIdx);

        default:
            return 0;
    }

}

void DaedalusVM::pushVar(size_t index, uint32_t arrIdx)
{
    m_Stack.push(arrIdx);
    m_Stack.push(static_cast<int32_t>(index));
    m_Stack.push(EParOp_PushVar);
}

uint32_t DaedalusVM::popVar(uint32_t& arrIdx)
{
    assert(m_Stack.size() >= 3);
    // Stack:
    //  - Token
    //  - Var-Index
    //  - Array-Index

    uint32_t tok = m_Stack.top();
    m_Stack.pop();

    uint32_t v = m_Stack.top();
    switch(tok)
    {
        case EParOp_PushVar:
            m_Stack.pop();

            arrIdx = m_Stack.top();
            m_Stack.pop();
            break;

        default:
            return 0xFFFFFFFF;
    }

    return v;
}


std::string DaedalusVM::popString()
{
    uint32_t arr;
    uint32_t idx = popVar(arr);

    return *m_DATFile.getSymbolByIndex(idx).getStrAddr(arr);
}

void DaedalusVM::setReturn(int32_t v)
{
    pushInt(v);
}

void DaedalusVM::setReturn(const std::string& v)
{
    pushString(v);
}

void DaedalusVM::setReturn(float v)
{
    pushInt(*reinterpret_cast<int32_t*>(&v));
}

void DaedalusVM::setReturnVar(int32_t v)
{
    pushVar(v);
}

float DaedalusVM::popFloatValue()
{
    int32_t v = popDataValue();
    return *reinterpret_cast<float*>(&v);
}

void DaedalusVM::pushVar(const std::string& symName)
{
    size_t idx = m_DATFile.getSymbolIndexByName(symName);
    pushVar(idx);
}

void DaedalusVM::pushString(const std::string& str)
{
    size_t symIdx = m_FakeStringSymbols.front();

    ZenLoad::PARSymbol& s = m_DATFile.getSymbolByIndex(symIdx);
    m_FakeStringSymbols.push(m_FakeStringSymbols.front());
    m_FakeStringSymbols.pop();

    s.set(str, 0);

    pushVar(symIdx, 0);
}

void DaedalusVM::setInstance(const std::string& instSymbol, void* addr)
{
    PARSymbol& s = m_DATFile.getSymbolByName(instSymbol);
    s.instanceData = addr;
}

void DaedalusVM::initializeInstance(void* instance, size_t symIdx, GEngineClasses::EInstanceClass classIdx)
{
    PARSymbol& s = m_DATFile.getSymbolByIndex(symIdx);

    // Enter address into instance-symbol
    s.instanceData = instance;

    setInstance("SELF", instance);
    setCurrentInstance(m_DATFile.getSymbolIndexByName("SELF"));

    // Point the PC to the instance-constructor
    doCallOperation(s.address);

    // Empty retstack, so we can stop execution after this call
    std::stack<size_t> retStack = m_RetStack;
    m_RetStack = std::stack<size_t>();
    size_t pc = m_PC;

    // Run script code to initialize the object
    while(doStack());

    // Return to old location and continue like nothing ever happened
    m_PC = pc;
    m_RetStack = retStack;

    // Add to registered instances
    m_RegisteredInstances[classIdx].insert(instance);
}

void DaedalusVM::setCurrentInstance(size_t symIdx)
{
    m_CurrentInstance = symIdx;
    m_CurrentInstanceDataPtr = m_DATFile.getSymbolByIndex(symIdx).instanceData;
}










































