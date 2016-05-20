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
    PARSymbol s;
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

            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr);
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
            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr);
            *addr += popDataValue();
            break;
        case EParOp_AssignSubtract:
            a = popVar(arr);
            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr);
            *addr -= popDataValue();
            break;
        case EParOp_AssignMultiply:
            a = popVar(arr);
            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr);
            *addr *= popDataValue();
            break;
        case EParOp_AssignDivide:
            a = popVar(arr);
            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr);
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
        case EParOp_PushInstance: pushVar(op.symbol); break; //TODO: Not sure about this
        case EParOp_AssignString:
            a = popVar(arr);
            b = popVar(arr2);
            straddr = m_DATFile.getSymbolByIndex(a).getStrAddr(arr);
            *straddr = *m_DATFile.getSymbolByIndex(b).getStrAddr(arr2);

            LogInfo() << " - " << a << ", " << b << ": " << *m_DATFile.getSymbolByIndex(b).getStrAddr(arr2);
            break;
        case EParOp_AssignStringRef:
            LogError() << "EParOp_AssignStringRef not implemented!";
            break;
        case EParOp_AssignFunc:
            a = popDataValue();
            b = popDataValue(); // TODO: Maybe should be using popVar here

            sym = m_DATFile.getSymbolByIndex(a);
            sym2 = m_DATFile.getSymbolByIndex(b);

            sym.address = sym2.address;
            break;

        case EParOp_AssignFloat:
            a = popVar(arr);
            b = popDataValue();

            sym = m_DATFile.getSymbolByIndex(a);
            assert(!sym.floatData.empty());

            sym.floatData[arr] = *reinterpret_cast<float*>(b);
            break;

        case EParOp_AssignInstance:
            a = popVar();
            b = popVar();

            LogInfo() << "AssignInstance: a=" << a << ", b=" << b;

            sym = m_DATFile.getSymbolByIndex(a);
            sym2 = m_DATFile.getSymbolByIndex(b);

            sym.classOffset = sym2.classOffset;
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
            break;
        case EParOp_PushArrayVar:
            pushVar(op.symbol, op.index);
            LogInfo() << " - [" << op.symbol << "] " << m_DATFile.getSymbolByIndex(op.symbol).name << ", Index:" << (int)op.index;
            break;
    }

    /*switch(op.op)
    {
        case EParOp_CallExternal:
        case EParOp_Call: {
            LogInfo() << oldPC <<" - CALL: " << op.address;

            // FIXME: Support actual externals
            auto it = m_ExternalsByIndex.find(op.address);
            if(it != m_ExternalsByIndex.end())
            {
                (*it).second(*this);
            }else {
                m_RetStack.push(m_PC);
                m_PC = op.address;
            }

        }
            break;

        case EParOp_Ret:
            if(m_RetStack.empty())
                return false; // Program ended

            LogInfo() << oldPC <<" - RET: -> " << static_cast<uint32_t>(m_RetStack.top());

            m_PC = static_cast<uint32_t>(m_RetStack.top());
            m_RetStack.pop();

            break;

        case EParOp_PushInt:
            m_Stack.push(op.value);
            LogInfo() << oldPC <<" - PUSHINT: " << op.value;
            break;


        case EParOp_SetInstance:
            m_CurrentInstance = popDataValue();
            break;

        case EParOp_PushArrayVar:
            m_Stack.push(op.symbol);
            LogInfo() << oldPC <<" - PushArrayVar: " << op.symbol;
            break;

        case EParOp_PushVar:
            m_Stack.push(op.symbol);
            LogInfo() << oldPC <<" - PushVar: " << op.symbol;
            break;

        case EParOp_PushInstance:
            m_Stack.push(op.symbol);
            LogInfo() << oldPC <<" - PUSHINSTANCE: " << op.symbol;
            break;

        case EParOp_Jump:
            m_PC = m_DATFile.getStack().stackSize - op.address - 1;
            LogInfo() << oldPC <<" - JUMP: " << (op.address);
            break;

        case EParOp_JumpIf: {
            int32_t b = m_Stack.top();
            m_Stack.pop();

            LogInfo() << oldPC <<" - JUMPIF: (" << b << ") -> " << (op.address);

            if(b)
                m_PC = (op.address);


        }
            break;


        case EParOp_Equal: {
            int32_t a = m_Stack.top();
            m_Stack.pop();
            int32_t b = m_Stack.top();
            m_Stack.pop();

            m_Stack.push(a == b ? 1 : 0);

            LogInfo() << m_PC <<" - EQUAL: (" << a << " == " << b << ")";
        }
            break;

        case EParOp_AssignString:
            a = popDataValue();
            b = popDataValue();

            sym = m_DATFile.getSymbolByIndex(a);
            sym2 = m_DATFile.getSymbolByIndex(b);
            assert(!sym.strData.empty());
            assert(!sym2.strData.empty());

            sym.strData[0] = sym2.strData[0];
            break;

        case EParOp_Add:
            a = popDataValue();
            b = popDataValue();
            pushInt(a + b);
            break;
        case EParOp_Subract:
            a = popDataValue();
            b = popDataValue();
            pushInt(a - b);
            break;
        case EParOp_Multiply:
            a = popDataValue();
            b = popDataValue();
            pushInt(a * b);
            break;
        case EParOp_Divide:
            a = popDataValue();
            b = popDataValue();
            pushInt(a / b);
            break;
        case EParOp_Mod:
            a = popDataValue();
            b = popDataValue();
            pushInt(a % b);
            break;
        case EParOp_BinOr:
            a = popDataValue();
            b = popDataValue();
            pushInt(a | b);
            break;
        case EParOp_BinAnd:
            a = popDataValue();
            b = popDataValue();
            pushInt(a & b);
            break;
        case EParOp_Less:
            a = popDataValue();
            b = popDataValue();
            pushInt(a < b ? 1 : 0);
            break;
        case EParOp_Greater:
            a = popDataValue();
            b = popDataValue();
            pushInt(a > b ? 1 : 0);
            break;
        case EParOp_Assign:
            a = popDataValue();
            b = popDataValue();

            sym = m_DATFile.getSymbolByIndex(a);
            if(sym.intData.empty())
                sym.intData.resize(1);

            assert(!sym.intData.empty());

            sym.intData[0] = b;
            break;
        case EParOp_LogOr:
            a = popDataValue();
            b = popDataValue();
            pushInt(a || b ? 1 : 0);
            break;
        case EParOp_LogAnd:
            a = popDataValue();
            b = popDataValue();
            pushInt(a && b ? 1 : 0);
            break;
        case EParOp_ShiftLeft:
            a = popDataValue();
            b = popDataValue();
            pushInt(a << b);
            break;
        case EParOp_ShiftRight:
            a = popDataValue();
            b = popDataValue();
            pushInt(a >> b);
            break;
        case EParOp_LessOrEqual:
            a = popDataValue();
            b = popDataValue();
            pushInt(a <= b ? 1 : 0);
            break;
        case EParOp_NotEqual:
            a = popDataValue();
            b = popDataValue();
            pushInt(a != b ? 1 : 0);
            break;
        case EParOp_GreaterOrEqual:
            a = popDataValue();
            b = popDataValue();
            pushInt(a >= b ? 1 : 0);
            break;
        case EParOp_AssignAdd:
            a = popDataValue();
            b = popDataValue();

            sym = m_DATFile.getSymbolByIndex(a);
            assert(!sym.intData.empty());

            sym.intData[0] += b;
            break;
        case EParOp_AssignSubtract:
            a = popDataValue();
            b = popDataValue();

            sym = m_DATFile.getSymbolByIndex(a);
            assert(!sym.intData.empty());

            sym.intData[0] -= b;
            break;
        case EParOp_AssignMultiply:
            a = popDataValue();
            b = popDataValue();

            sym = m_DATFile.getSymbolByIndex(a);
            assert(!sym.intData.empty());

            sym.intData[0] *= b;
            break;
        case EParOp_AssignDivide:
            a = popDataValue();
            b = popDataValue();

            sym = m_DATFile.getSymbolByIndex(a);
            assert(!sym.intData.empty());

            sym.intData[0] /= b;
            break;
        case EParOp_Plus:
            a = popDataValue();

            sym = m_DATFile.getSymbolByIndex(a);
            assert(!sym.intData.empty());

            // TODO: Assumed + means abs. Find more information about this
            sym.intData[0] = static_cast<int32_t>(std::abs(sym.intData[0]));
            break;
        case EParOp_Minus:
            sym = m_DATFile.getSymbolByIndex(a);
            assert(!sym.intData.empty());

            sym.intData[0] = -sym.intData[0];
            break;
        case EParOp_Not:
            sym = m_DATFile.getSymbolByIndex(a);
            assert(!sym.intData.empty());

            sym.intData[0] = !sym.intData[0];
            break;
        case EParOp_Negate:
            sym = m_DATFile.getSymbolByIndex(a);
            assert(!sym.intData.empty());

            sym.intData[0] = ~sym.intData[0];
            break;
        case EParOp_AssignStringRef:
            LogError() << "Unimplemented opcode: EParOp_AssignStringRef";
            assert(false);
            break;
        case EParOp_AssignFunc:
            a = popDataValue();
            b = popDataValue();

            sym = m_DATFile.getSymbolByIndex(a);
            sym2 = m_DATFile.getSymbolByIndex(b);

            sym.address = sym2.address;
            break;
        case EParOp_AssignFloat:
            a = popDataValue();
            b = popDataValue();

            sym = m_DATFile.getSymbolByIndex(a);
            sym2 = m_DATFile.getSymbolByIndex(b);
            assert(!sym.strData.empty());
            assert(!sym2.strData.empty());

            sym.floatData[0] = sym2.floatData[0];
            break;
        case EParOp_AssignInstance:
            a = popDataValue();
            b = popDataValue();

            sym = m_DATFile.getSymbolByIndex(a);
            sym2 = m_DATFile.getSymbolByIndex(b);
            assert(!sym.strData.empty());
            assert(!sym2.strData.empty());

            sym.address = sym2.address;
            break;

        default:
            if(log) LogInfo() << OP_MAP[op.op];
    }*/

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




































