//
// Created by andre on 09.05.16.
//

#include <utils/logger.h>
#include <map>
#include <assert.h>
#include "DaedalusVM.h"

#define STACKIFY(x) static_cast<int32_t>(x)

extern std::map<int, const char*> OP_MAP;

const int NUM_FAKE_STRING_SYMBOLS = 5;

using namespace ZenLoad;
using namespace Daedalus;

DaedalusVM::DaedalusVM(const DATFile &dat, const std::string &main)
    : m_GameState(*this)
{
    init(dat, main);
}

DaedalusVM::DaedalusVM(const std::string& file, const std::string& main)
    : m_GameState(*this)
{
    init(Daedalus::DATFile(file), main);
}

void DaedalusVM::init(const DATFile &dat, const std::string& main)
{
    m_PC = 0;
    m_DATFile = dat;

    // See if we find a "main"-function
    if(m_DATFile.hasSymbolName(main))
    {
        const PARSymbol& s = m_DATFile.getSymbolByName(main);

        LogInfo() << "Found main-function at Address: " << s.address;
        m_PC = s.address;
    }

    // Make fake-strings
    for(size_t i=0;i<NUM_FAKE_STRING_SYMBOLS;i++)
        m_FakeStringSymbols.push(m_DATFile.addSymbol());

    m_GameState.registerExternals();

    m_CurrentInstanceHandle.invalidate();

}

bool DaedalusVM::doStack(bool verbose)
{
    static bool log = verbose;
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

        case EParOp_AssignFunc:
            /*a = popVar(arr);
            b = popDataValue();

            sym2 = m_DATFile.getSymbolByIndex(b);
            m_DATFile.getSymbolByIndex(a).set(sym2.address, 0, getCurrentInstanceDataPtr());
            break;*/

        case EParOp_Assign:
            a = popVar(arr);

            if(log) LogInfo() << " - a: " << a << ", arr: " << arr;

            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr, getCurrentInstanceDataPtr());
            b = *addr;
            *addr = popDataValue();
            if(log) LogInfo() << " - " << b << " -> " << *addr;

            if(m_OnSymbolValueChanged)
                m_OnSymbolValueChanged((unsigned)a, EParOp_Assign);

            break;
        case EParOp_LogOr:
            // Note: This can't be done in one line because || wouldn't pop the second piece of the stack
            // if the first was 1
            a = popDataValue();
            b = popDataValue();
            pushInt(a || b ? 1 : 0);
            break;
        case EParOp_LogAnd:
            // Note: This can't be done in one line because || wouldn't pop the second piece of the stack
            // if the first was 1
            a = popDataValue();
            b = popDataValue();
            pushInt(a && b ? 1 : 0);
            break;
        case EParOp_ShiftLeft:pushInt(popDataValue() << popDataValue());break;
        case EParOp_ShiftRight:pushInt(popDataValue() << popDataValue());break;
        case EParOp_LessOrEqual:pushInt(popDataValue() <= popDataValue() ? 1 : 0);break;
        case EParOp_Equal:pushInt(popDataValue() == popDataValue() ? 1 : 0);break;
        case EParOp_NotEqual:pushInt(popDataValue() != popDataValue() ? 1 : 0);break;
        case EParOp_GreaterOrEqual:pushInt(popDataValue() >= popDataValue() ? 1 : 0);break;
        case EParOp_AssignAdd:
            a = popVar(arr);
            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr, getCurrentInstanceDataPtr());
            *addr += popDataValue();

            if(m_OnSymbolValueChanged)
                m_OnSymbolValueChanged((unsigned)a, EParOp_AssignAdd);
            break;
        case EParOp_AssignSubtract:
            a = popVar(arr);
            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr, getCurrentInstanceDataPtr());
            *addr -= popDataValue();

            if(m_OnSymbolValueChanged)
                m_OnSymbolValueChanged((unsigned)a, EParOp_AssignSubtract);
            break;
        case EParOp_AssignMultiply:
            a = popVar(arr);
            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr, getCurrentInstanceDataPtr());
            *addr *= popDataValue();

            if(m_OnSymbolValueChanged)
                m_OnSymbolValueChanged((unsigned)a, EParOp_AssignMultiply);
            break;
        case EParOp_AssignDivide:
            a = popVar(arr);
            addr = m_DATFile.getSymbolByIndex(a).getIntAddr(arr, getCurrentInstanceDataPtr());
            *addr /= popDataValue();

            if(m_OnSymbolValueChanged)
                m_OnSymbolValueChanged((unsigned)a, EParOp_AssignDivide);
            break;
        case EParOp_Plus:
            pushInt(+popDataValue());
            break;
        case EParOp_Minus:
            pushInt(-popDataValue());
            break;
        case EParOp_Not:
            pushInt(!popDataValue());
            break;
        case EParOp_Negate:
            pushInt(~popDataValue());
            break;
        case EParOp_Ret:
            /*if(m_RetStack.empty())
                return false; // Program ended

            m_PC = static_cast<uint32_t>(m_RetStack.top());
            m_RetStack.pop();
            m_CallStack.pop_back();*/
			return false;
            break;
        case EParOp_Call:
        {
            if(log) LogInfo() << oldPC <<" - CALL: " << op.address;

			/*m_RetStack.push(m_PC);
			m_PC = (size_t)op.address;

			m_CallStack.push_back((size_t)op.address);*/

			ZMemory::BigHandle currentInstanceHandle = m_CurrentInstanceHandle;
			EInstanceClass currentInstanceClass = m_CurrentInstanceClass;
			size_t PC = m_PC;
           
			m_PC = (size_t)op.address;
			while(doStack());

			m_PC = PC;
			m_CurrentInstanceHandle = currentInstanceHandle;
			m_CurrentInstanceClass = currentInstanceClass; 
        }
            break;

        case EParOp_CallExternal: {
            auto it = m_ExternalsByIndex.find(op.symbol);

			ZMemory::BigHandle currentInstanceHandle = m_CurrentInstanceHandle;
			EInstanceClass currentInstanceClass = m_CurrentInstanceClass;
			size_t PC = m_PC;

            if(log) LogInfo() << " - Function: " << m_DATFile.getSymbolByIndex(op.symbol).name;
            if (it != m_ExternalsByIndex.end()) {

                if(m_OnExternalCalled)
                    m_OnExternalCalled((unsigned)op.symbol);

                (*it).second(*this);
            }

			m_PC = PC;
			m_CurrentInstanceHandle = currentInstanceHandle;
			m_CurrentInstanceClass = currentInstanceClass; 

        }
            break;

        case EParOp_PushInt: pushInt(op.value); if(log) LogInfo() << " - " << op.value; break;
        case EParOp_PushVar:
            pushVar(op.symbol);

            if(log)
            {
                // Push second time just for debugging
                pushVar(op.symbol);
                LogInfo() << " - [" << op.symbol << "] " << m_DATFile.getSymbolByIndex(op.symbol).name << ": " <<
                popDataValue();
            }
            break;
        case EParOp_PushInstance:
            pushVar(op.symbol);
            if(log) LogInfo() << " - [" << op.symbol << "] " << m_DATFile.getSymbolByIndex(op.symbol).name << ", Handle: " << m_DATFile.getSymbolByIndex(op.symbol).instanceDataHandle.index;
            break; //TODO: Not sure about this
        case EParOp_AssignString:
            a = popVar(arr);
            b = popVar(arr2);

            if(b == -1)
            {
                LogWarn() << "EParOp_AssignString failed! Invalid value for 'b'!";
                break;
            }

            sym = m_DATFile.getSymbolByIndex(a);
            straddr = m_DATFile.getSymbolByIndex(a).getStrAddr(arr, getCurrentInstanceDataPtr());

            {
                //std::string s1 = *straddr;
                std::string s2 = *m_DATFile.getSymbolByIndex(b).getStrAddr(arr2, getCurrentInstanceDataPtr());

                //LogInfo() << "s1 (" << sym.name << "): " << s1 << " s2: " << s2;

                *straddr = s2;
            }

            if(m_OnSymbolValueChanged)
                m_OnSymbolValueChanged((unsigned)a, EParOp_AssignString);

            //LogInfo() << "We're fine!";
            //if(log) LogInfo() << " - " << a << ", " << b << ": " << *m_DATFile.getSymbolByIndex(b).getStrAddr(arr2, getCurrentInstanceDataPtr());
            break;
        case EParOp_AssignStringRef:
            LogError() << "EParOp_AssignStringRef not implemented!";
            break;


        case EParOp_AssignFloat:
            a = popVar(arr);
            b = popDataValue();

            m_DATFile.getSymbolByIndex(a).set(b, arr, getCurrentInstanceDataPtr());

            if(m_OnSymbolValueChanged)
                m_OnSymbolValueChanged((unsigned)a, EParOp_AssignFloat);
            break;

        case EParOp_AssignInstance:
            a = popVar();
            b = popVar();



            m_DATFile.getSymbolByIndex(a).instanceDataHandle = m_DATFile.getSymbolByIndex(b).instanceDataHandle;
            m_DATFile.getSymbolByIndex(a).instanceDataClass = m_DATFile.getSymbolByIndex(b).instanceDataClass;

            //sym.classOffset = sym2.classOffset;
            //sym.instanceData = sym2.instanceData;

            if(log) LogInfo() << "AssignInstance: a=" << a << ", b=" << b;
            if(log) LogInfo() << " - [" << a << "] " << m_DATFile.getSymbolByIndex(a).name << ", addr: " << m_DATFile.getSymbolByIndex(a).instanceDataHandle.index;

            if(m_OnSymbolValueChanged)
                m_OnSymbolValueChanged((unsigned)a, EParOp_AssignInstance);
            break;

        case EParOp_Jump:
            m_PC = op.address;
            break;

        case EParOp_JumpIf:
            a = popDataValue();
            if(!a) // TODO: They seem to jump at 0, rather than 1
                m_PC = op.address;

            if(log) LogInfo() << " - " << a << ", " << oldPC << " -> " << m_PC;
            break;

        case EParOp_SetInstance:
            m_CurrentInstance = op.symbol;
            if(log) LogInfo() << " - [" << op.symbol << "] " << m_DATFile.getSymbolByIndex(op.symbol).name;

            setCurrentInstance(op.symbol);
            break;
        case EParOp_PushArrayVar:
            pushVar(STACKIFY(op.symbol), op.index);
            if(log) LogInfo() << " - [" << op.symbol << "] " << m_DATFile.getSymbolByIndex(op.symbol).name << ", Index:" << (int)op.index;
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
    m_CallStack.push_back(target);
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

int32_t DaedalusVM::popDataValue()
{
    // Default behavior of the ZenGin is to pop a 0, if nothing is on the stack.
    if(m_Stack.empty())
        return 0;

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
            return m_DATFile.getSymbolByIndex(v).getIntValue(arrIdx, getCurrentInstanceDataPtr());

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
	if(m_Stack.empty())
		return 0;

    // Stack:
    //  - Token
    //  - Var-Index
    //  - Array-Index

    uint32_t tok = static_cast<uint32_t>(m_Stack.top());
    m_Stack.pop();

    uint32_t v = static_cast<uint32_t>(m_Stack.top());
    switch(tok)
    {
        case EParOp_PushVar:
            m_Stack.pop();

            arrIdx = static_cast<uint32_t>(m_Stack.top());
            m_Stack.pop();
            break;

		case EParOp_PushInt:
			// popVar and popInt don't seem to be right in some cases
			m_Stack.pop();
			arrIdx = 0;
			break;

        default:
            return 0xFFFFFFFF;
    }

    return v;
}


std::string DaedalusVM::popString(bool toUpper)
{
    uint32_t arr;
    uint32_t idx = popVar(arr);

    std::string s = *m_DATFile.getSymbolByIndex(idx).getStrAddr(arr, getCurrentInstanceDataPtr());

    if(toUpper)
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);

    return s;
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
    pushVar(STACKIFY(v));
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

        Daedalus::PARSymbol& s = m_DATFile.getSymbolByIndex(symIdx);
    m_FakeStringSymbols.push(m_FakeStringSymbols.front());
    m_FakeStringSymbols.pop();

    s.set(str, 0);

    pushVar(symIdx, 0);
}

void DaedalusVM::setInstance(const std::string& instSymbol, ZMemory::BigHandle h, EInstanceClass instanceClass)
{
    PARSymbol& s = m_DATFile.getSymbolByName(instSymbol);
    s.instanceDataHandle = ZMemory::toBigHandle(h);
    s.instanceDataClass = instanceClass;
}

void DaedalusVM::initializeInstance(ZMemory::BigHandle instance, size_t symIdx, EInstanceClass classIdx)
{
    pushState();

    if(symIdx == m_DATFile.getSymbolIndexByName("GRD_282_NEK"))
        LogInfo() << "Nek!";

    PARSymbol& s = m_DATFile.getSymbolByIndex(symIdx);

    // Enter address into instance-symbol
    s.instanceDataHandle = instance;
    s.instanceDataClass = classIdx;

    ZMemory::BigHandle currentInstanceHandle = m_CurrentInstanceHandle;
    EInstanceClass currentInstanceClass = m_CurrentInstanceClass;

    setCurrentInstance(symIdx);

    // Set self
	PARSymbol selfCpy = m_DATFile.getSymbolByName("self"); // Copy of "self"-symbol
    setInstance("self", instance, classIdx);

    // Place the assigning symbol into the instance
    GEngineClasses::Instance* instData = m_GameState.getByClass(instance, classIdx);
    instData->instanceSymbol = symIdx;

    // Point the PC to the instance-constructor
    doCallOperation(s.address);

    // Empty retstack, so we can stop execution after this call
    std::stack<size_t> retStack = m_RetStack;
    m_RetStack = std::stack<size_t>();
    size_t pc = m_PC;

    m_CallStack.clear();

    // Run script code to initialize the object
    while(doStack());

	// Reset to old "self"
	m_DATFile.getSymbolByName("self") = selfCpy;

    // Return to old location and continue like nothing ever happened
    m_PC = pc;
    m_RetStack = retStack;


    // Reset state
    //m_DATFile.getSymbolByName("SELF").instanceDataHandle = oldSelfInstance;
    //m_DATFile.getSymbolByName("SELF").instanceDataClass = oldSelfClass;
    m_CurrentInstanceHandle = currentInstanceHandle;
    m_CurrentInstanceClass = currentInstanceClass;

    popState();
}

void DaedalusVM::setCurrentInstance(size_t symIdx)
{
    m_CurrentInstance = symIdx;
    m_CurrentInstanceHandle = m_DATFile.getSymbolByIndex(symIdx).instanceDataHandle;
    m_CurrentInstanceClass = m_DATFile.getSymbolByIndex(symIdx).instanceDataClass;
}

void* DaedalusVM::getCurrentInstanceDataPtr()
{
    return m_GameState.getByClass(m_CurrentInstanceHandle, m_CurrentInstanceClass);
}

void DaedalusVM::pushState()
{
	return;
    VMState s;
    s.m_CurrentInstanceClass = m_CurrentInstanceClass;
    s.m_CurrentInstanceHandle = m_CurrentInstanceHandle;
    s.m_PC = m_PC;
    s.m_RetStack = m_RetStack;
    s.m_Stack = m_Stack;
    s.m_Self = m_DATFile.getSymbolByName("self");
    s.m_CallStack = m_CallStack;

    m_PC = 0;
    m_RetStack = std::stack<size_t>();
    m_Stack = std::stack<uint32_t>();
    m_CurrentInstanceHandle.invalidate();
    m_CallStack.clear();

    m_StateStack.push(s);
}

void DaedalusVM::popState()
{
	return; 
    m_PC = m_StateStack.top().m_PC;
    m_RetStack = m_StateStack.top().m_RetStack;
    m_Stack = m_StateStack.top().m_Stack;
    m_CallStack = m_StateStack.top().m_CallStack;

    m_CurrentInstanceHandle = m_StateStack.top().m_CurrentInstanceHandle;
    m_CurrentInstanceClass= m_StateStack.top().m_CurrentInstanceClass;

    m_DATFile.getSymbolByName("self") = m_StateStack.top().m_Self;

    m_StateStack.pop();
}

std::vector<std::string> DaedalusVM::getCallStack()
{
    std::vector<std::string> cs;

    for(size_t adr : m_CallStack)
    {
        // Find symbol with that address
        for(const PARSymbol& s : m_DATFile.getSymTable().symbols)
        {
            if(s.address == adr)
                cs.push_back(s.name);
        }
    }

    std::reverse(cs.begin(), cs.end());

    return cs;
}


void DaedalusVM::prepareRunFunction()
{
    // Clean the VM for this run
    pushState();
}


int32_t DaedalusVM::runFunction(const std::string& fname)
{
    assert(getDATFile().hasSymbolName(fname));

    return runFunction(getDATFile().getSymbolByName(fname).address);
}

int32_t DaedalusVM::runFunction(size_t addr)
{
	if(addr == 0)
		return -1;

    // Place the call-operation
    doCallOperation(addr);

    clearCallStack();

    // Execute the instructions
    while(doStack());

    int32_t ret = 0;

    // Only pop if the VM didn't mess up
    if(!isStackEmpty())
        ret = popDataValue();
    else
        LogWarn() << "DaedalusVM: Safety int was popped by scriptcode!";

    // Restore to previous VM-State
    popState();
    return ret;
}

int32_t DaedalusVM::runFunctionBySymIndex(size_t symIdx)
{
    int32_t r = runFunction(getDATFile().getSymbolByIndex(symIdx).address);

    return r;
}










































