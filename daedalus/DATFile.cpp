//
// Created by andre on 09.05.16.
//

#include "DATFile.h"
#include <utils/logger.h>
#include <map>
#include <algorithm>

using namespace ZenLoad;
using namespace Daedalus;

std::map<int, const char*> OP_MAP = {
        {0,  "Add"},    // a + b
        {1,  "Subract"},    // a - b
        {2,  "Multiply"},    // a * b
        {3,  "Divide"},    // a / b
        {4,  "Mod"},    // a % b
        {5,  "BinOr"},    // a | b
        {6,  "BinAnd"},    // a & b
        {7,  "Less"},    // a < b
        {8,  "Greater"},    // a > b
        {9,  "Assign"},    // a = b
        {11, "LogOr"},    // a || b
        {12, "LogAnd"},    // a && b
        {13, "ShiftLeft"},    // a << b
        {14, "ShiftRight"},    // a >> b
        {15, "LessOrEqual"},    // a <= b
        {16, "Equal"},    // a == b
        {17, "NotEqual"},    // a != b
        {18, "GreaterOrEqual"},    // a >= b
        {19, "AssignAdd"},    // a += b (a = a + b)
        {20, "AssignSubtract"},    // a -= b (a = a - b)
        {21, "AssignMultiply"},    // a *= b (a = a * b)
        {22, "AssignDivide"},    // a /= b (a = a / b)
        {30, "Plus"},    // +a
        {31, "Minus"},    // -a
        {32, "Not"},    // !a
        {33, "Negate"},    // ~a
        {60, "Ret"},
        {61, "Call"},
        {62, "CallExternal"},
        {64, "PushInt"},
        {65, "PushVar"},
        {67, "PushInstance"},
        {70, "AssignString"},
        {71, "AssignStringRef"},
        {72, "AssignFunc"},
        {73, "AssignFloat"},
        {74, "AssignInstance"},
        {75, "Jump"},
        {76, "JumpIf"},
        {80, "SetInstance"},
        {245, "PushArrayVar"},   // EParOp_PushVar + EParOp_Array
};


DATFile::DATFile() :
        m_Parser(),
        m_Verbose(false)
{

}

DATFile::DATFile(const std::string& file, bool verbose) :
        m_Parser(file),
        m_Verbose(verbose)
{
    /*
    std::string end = "GOTHIC.DAT";
    //end = "PARTICLEFX.DAT";
    auto pos = file.size() - end.size();
    bool endsWith = end.size() <= file.size() && file.find(end, pos) != std::string::npos;
    m_Verbose = endsWith;
     */
    parseFile();
}

void DATFile::parseFile()
{
    uint8_t version = m_Parser.readBinaryByte();
    if(m_Verbose) LogInfo() << "DAT-Version: " << (int)version;

    readSymTable();
}

void DATFile::readSymTable()
{
    if(m_Verbose) LogInfo() << "Reading Sym-Table...";

    uint32_t count = m_Parser.readBinaryDWord();
    m_SymTable.symbolsByName.reserve(count);
    m_SymTable.symbols.resize(count);
    m_SymTable.sortTable.resize(count);
    m_Parser.readBinaryRaw(m_SymTable.sortTable.data(), sizeof(uint32_t) * count);

    if(m_Verbose) LogInfo() << "Reading " << count << " symbols ";

    // Read symbols
    for(uint32_t i=0;i<count;i++)
    {
        PARSymbol s;
        uint32_t named = m_Parser.readBinaryDWord();

        if(named)
        {
            uint8_t b = m_Parser.readBinaryByte();
            while(b != 0x0A)
            {
                if(b != 0xFF) // FIXME: This happens at INSTANCE_HELP?
                    s.name += b;

                b = m_Parser.readBinaryByte();
            };

            if(m_Verbose) LogInfo() << "Symbol Name: " << s.name;
        }

        m_Parser.readBinaryRaw(&s.properties, sizeof(s.properties));

        if(m_Verbose)
        {
            auto typeName = eParTypeToString(EParType(s.properties.elemProps.type));
            LogInfo() << "Properties: " << typeName << "[" <<  s.properties.elemProps.count << "]";
        }

        if(0 == (s.properties.elemProps.flags & EParFlag_ClassVar))
        {
            switch(s.properties.elemProps.type)
            {
                case EParType_Float:
                    s.floatData.resize(s.properties.elemProps.count);
                    m_Parser.readBinaryRaw(s.floatData.data(), sizeof(float) * s.floatData.size());

                    if(m_Verbose)
                        for(uint32_t j=0;j<s.properties.elemProps.count;j++)
                            LogInfo() << " - Float: " << s.floatData[j];
                    break;

                case EParType_Int:
                    s.intData.resize(s.properties.elemProps.count);
                    m_Parser.readBinaryRaw(s.intData.data(), sizeof(uint32_t) * s.intData.size());

                    if(m_Verbose)
                        for(uint32_t j=0;j<s.properties.elemProps.count;j++)
                            LogInfo() << " - Int: " << s.intData[j];
                    break;

                case EParType_String:
                    s.strData.resize(s.properties.elemProps.count);
                    for(uint32_t j=0;j<s.properties.elemProps.count;j++)
                    {
                        uint8_t b = m_Parser.readBinaryByte();
                        while(b != 0x0A)
                        {
                            if(b != 0xFF) // FIXME: This happens at INSTANCE_HELP?
                                s.strData[j] += b;

                            b = m_Parser.readBinaryByte();
                        };

                        auto replace = [](std::string& searchSpace, const std::string& from, const std::string& to) {
                            size_t start_pos = 0;
                            while((start_pos = searchSpace.find(from, start_pos)) != std::string::npos) {
                                searchSpace.replace(start_pos, from.length(), to);
                                start_pos += to.length();
                            }
                        };
                        replace(s.strData[j], "\\n", "\n");

                        if(m_Verbose) LogInfo() << " - String[" << j << "]: " << s.strData[j];
                    }
                    break;

                case EParType_Class:
                    s.classOffset = static_cast<int32_t>(m_Parser.readBinaryDWord());
                    if(m_Verbose) LogInfo() << " - ClassOffset: " << s.classOffset;
                    break;

                case EParType_Func:
                case EParType_Prototype:
                case EParType_Instance:
                    s.address = static_cast<int32_t>(m_Parser.readBinaryDWord());
                    if(m_Verbose) LogInfo() << " - Address: " << s.address;

                    if(s.properties.elemProps.flags & EParFlag_External)
                        if(m_Verbose) LogInfo() << "External: Address: " << s.address << " Name: " << s.name;
                    break;
            }
        }

        s.parent = static_cast<int32_t>(m_Parser.readBinaryDWord());

        if(!s.name.empty())
            m_SymTable.symbolsByName.emplace(s.name, i);

        // check for callable object
        if (s.isEParType(EParType_Prototype) || // is a Prototype
                (s.isEParType(EParType_Func) // or a function, which
            && !s.hasEParFlag(EParFlag_ClassVar) // is no class-member (skip class members of type func)
            && s.hasEParFlag(EParFlag_Const))) // and is const (skip function params of type func)
        {
            m_SymTable.functionsByAddress.emplace(s.address, i);
        }

        m_SymTable.symbols[i] = std::move(s);
    }

    readStack();
}

void DATFile::readStack()
{
    if(m_Verbose) LogInfo() << "Reading Stack...";
    m_Stack.stackSize = m_Parser.readBinaryDWord();

    if(m_Verbose) LogInfo() << " - " << m_Stack.stackSize << " bytes";

    m_Stack.stackOffset = m_Parser.getSeek();

    std::string ss;
    std::stringstream sss;

    size_t seek=0;
    while(m_Parser.getSeek() < m_Parser.getFileSize())
    {
        PARStackOpCode s;
        memset(&s, 0, sizeof(s));
        s.op = static_cast<EParOp>(m_Parser.readBinaryByte());

        if(m_Verbose)
        {
            sss.str(std::string());
            sss.clear();

            sss << seek << ": " << OP_MAP[s.op];
            ss = sss.str();
        }

        seek += sizeof(uint8_t);

        switch(s.op)
        {
            case EParOp_Call:
                s.address = m_Parser.readBinaryDWord();
                seek += sizeof(int32_t);

                if(m_Verbose) LogInfo() << ss << " - address: " << s.address;
                break;

            case EParOp_CallExternal:
                s.symbol = m_Parser.readBinaryDWord();
                seek += sizeof(int32_t);

                if(m_Verbose) LogInfo() << ss << " - symbol: " << s.symbol << " (" << m_SymTable.symbols[s.symbol].name << ")";
                break;

            case EParOp_PushInt:
                s.value = m_Parser.readBinaryDWord();
                seek += sizeof(int32_t);

                if(m_Verbose) LogInfo() << ss << " - value: " << s.value;
                break;

            case EParOp_PushVar:
                s.symbol = m_Parser.readBinaryDWord();
                seek += sizeof(int32_t);

                if(m_Verbose) LogInfo() << ss << " - symbol: " << s.symbol << " (" << m_SymTable.symbols[s.symbol].name << ")";
                break;

            case EParOp_PushInstance:
                s.symbol = m_Parser.readBinaryDWord();
                seek += sizeof(int32_t);

                if(m_Verbose) LogInfo() << ss << " - symbol: " << s.symbol << " (" << m_SymTable.symbols[s.symbol].name << ")";
                break;

            case EParOp_Jump:
                s.address = m_Parser.readBinaryDWord();
                seek += sizeof(int32_t);

                if(m_Verbose) LogInfo() << ss << " - address: " << s.address;
                break;

            case EParOp_JumpIf:
                s.address = m_Parser.readBinaryDWord();
                seek += sizeof(int32_t);

                if(m_Verbose) LogInfo() << ss << " - address: " << s.address;
                break;

            case EParOp_SetInstance:
                s.symbol = m_Parser.readBinaryDWord();
                seek += sizeof(int32_t);

                if(m_Verbose) LogInfo() << ss << " - symbol: " << s.symbol << " (" << m_SymTable.symbols[s.symbol].name << ")";
                break;

            case EParOp_PushArrayVar:
                s.symbol = m_Parser.readBinaryDWord();
                seek += sizeof(int32_t);

                s.index = m_Parser.readBinaryByte();
                seek += sizeof(uint8_t);

                break;
            default:
                if(m_Verbose) LogInfo() << ss;
        }
    }
}

std::string Daedalus::eParTypeToString(EParType type)
{
    switch (type)
    {
        case EParType_Void:
            return "void";
        case EParType_Float:
            return "float";
        case EParType_Int:
            return "int";
        case EParType_String:
            return "string";
        case EParType_Class:
            return "class";
        case EParType_Func:
            return "func";
        case EParType_Prototype:
            return "prototype";
        case EParType_Instance:
            return "instance";
        default:
            return "unknown_type";
    }
}

namespace Daedalus
{
    template <> EInstanceClass enumFromClass<Daedalus::GEngineClasses::C_Npc>() { return IC_Npc; };
    template <> EInstanceClass enumFromClass<Daedalus::GEngineClasses::C_Mission>() { return IC_Mission; };
    template <> EInstanceClass enumFromClass<Daedalus::GEngineClasses::C_Info>() { return IC_Info; };
    template <> EInstanceClass enumFromClass<Daedalus::GEngineClasses::C_Item>() { return IC_Item; };
    template <> EInstanceClass enumFromClass<Daedalus::GEngineClasses::C_ItemReact>() { return IC_ItemReact; };
    template <> EInstanceClass enumFromClass<Daedalus::GEngineClasses::C_Focus>() { return IC_Focus; };
    template <> EInstanceClass enumFromClass<Daedalus::GEngineClasses::C_Menu>() { return IC_Menu; };
    template <> EInstanceClass enumFromClass<Daedalus::GEngineClasses::C_Menu_Item>() { return IC_MenuItem; };
    template <> EInstanceClass enumFromClass<Daedalus::GEngineClasses::C_SFX>() { return IC_Sfx; };
    template <> EInstanceClass enumFromClass<Daedalus::GEngineClasses::C_ParticleFX>() { return IC_Pfx; };
}

PARSymbol& DATFile::getSymbolByName(const std::string& symName)
{
    std::string n = std::string(symName);
    std::transform(n.begin(), n.end(), n.begin(), ::toupper);
    //assert(hasSymbolName(n));
    if (!hasSymbolName(n)){
        LogWarn() << "symbol " << symName << " not found";
    }
    return m_SymTable.symbols[m_SymTable.symbolsByName[n]];
}

bool DATFile::hasSymbolName(const std::string& symName)
{
    std::string n = std::string(symName);
    std::transform(n.begin(), n.end(), n.begin(), ::toupper);

    return m_SymTable.symbolsByName.find(n) != m_SymTable.symbolsByName.end();
}

size_t DATFile::getSymbolIndexByName(const std::string& symName)
{
    std::string n = std::string(symName);
    std::transform(n.begin(), n.end(), n.begin(), ::toupper);

    return m_SymTable.symbolsByName[n];
}

PARSymbol& DATFile::getSymbolByIndex(size_t idx)
{
    return m_SymTable.symbols[idx];
}

size_t DATFile::getFunctionIndexByAddress(size_t address) {
    auto it = m_SymTable.functionsByAddress.find(address);
    if (it != m_SymTable.functionsByAddress.end())
        return it->second;
    else
        return static_cast<size_t>(-1);
}

PARStackOpCode DATFile::getStackOpCode(size_t pc)
{
    m_Parser.setSeek(m_Stack.stackOffset + pc);

    PARStackOpCode s;
    s.op = static_cast<EParOp>(m_Parser.readBinaryByte());

    s.opSize = sizeof(uint8_t);

    switch(s.op)
    {
        case EParOp_Call:
            s.address = m_Parser.readBinaryDWord();
            s.opSize += sizeof(int32_t);

            if(m_Verbose) LogInfo() << "    - address: " << s.address;
            break;

        case EParOp_CallExternal:
            s.symbol = m_Parser.readBinaryDWord();
            s.opSize += sizeof(int32_t);

            if(m_Verbose) LogInfo() << "    - symbol: " << s.symbol << " (" << m_SymTable.symbols[s.symbol].name << ")";
            break;

        case EParOp_PushInt:
            s.value = m_Parser.readBinaryDWord();
            s.opSize += sizeof(int32_t);

            if(m_Verbose) LogInfo() << "    - value: " << s.value;
            break;

        case EParOp_PushVar:
            s.symbol = m_Parser.readBinaryDWord();
            s.opSize += sizeof(int32_t);

            if(m_Verbose) LogInfo() << "    - symbol: " << s.symbol << " (" << m_SymTable.symbols[s.symbol].name << ")";
            break;

        case EParOp_PushInstance:
            s.symbol = m_Parser.readBinaryDWord();
            s.opSize += sizeof(int32_t);

            if(m_Verbose) LogInfo() << "    - symbol: " << s.symbol << " (" << m_SymTable.symbols[s.symbol].name << ")";
            break;

        case EParOp_Jump:
            s.address = m_Parser.readBinaryDWord();
            s.opSize += sizeof(int32_t);

            if(m_Verbose) LogInfo() << "    - address: " << s.address;
            break;

        case EParOp_JumpIf:
            s.address = m_Parser.readBinaryDWord();
            s.opSize += sizeof(int32_t);

            if(m_Verbose) LogInfo() << "    - address: " << s.address;
            break;

        case EParOp_SetInstance:
            s.symbol = m_Parser.readBinaryDWord();
            s.opSize += sizeof(int32_t);

            if(m_Verbose) LogInfo() << "    - symbol: " << s.symbol << " (" << m_SymTable.symbols[s.symbol].name << ")";
            break;

        case EParOp_PushArrayVar:
            s.symbol = m_Parser.readBinaryDWord();
            s.opSize += sizeof(int32_t);

            s.index = m_Parser.readBinaryByte();
            s.opSize += sizeof(uint8_t);

            break;

        default:
            break;
    }

    return s;
}

size_t DATFile::addSymbol()
{
    m_SymTable.symbols.emplace_back();
    return m_SymTable.symbols.size()-1;
}

void DATFile::iterateSymbolsOfClass(const std::string& className, std::function<void(size_t, PARSymbol&)> callback)
{
    // First, find the parent-symbol
    size_t baseSym = getSymbolIndexByName(className);

    for(size_t i = 0; i< m_SymTable.symbols.size(); i++)
    {
        PARSymbol& s = getSymbolByIndex(i);
        if(s.parent == -1 || s.properties.elemProps.type != EParType_Instance)
            continue;

        if ((s.properties.elemProps.flags & Daedalus::EParFlag::EParFlag_Const) == 0)
            continue; // filters out variables of type C_NPC or C_ITEM

        PARSymbol& p = getSymbolByIndex(s.parent);
        uint32_t pBase = s.parent;

        // In case this is also just a prototype, go deeper one more level
        if(p.properties.elemProps.type == EParType_Prototype && p.parent != -1)
        {
            pBase = p.parent;
        }

        // If the parent-classes match, we found an instance of our class
        if(baseSym == pBase)
            callback(i, s);
    }
}

namespace Daedalus
{
    // getDataContainer specializations
    template<>
    std::vector<int32_t>& PARSymbol::getDataContainer()
    {
        return this->intData;
    }

    template<>
    std::vector<float>& PARSymbol::getDataContainer()
    {
        return this->floatData;
    }

    template<>
    std::vector<std::string>& PARSymbol::getDataContainer()
    {
        return this->strData;
    }
}

int32_t& PARSymbol::getInt(size_t idx, void *baseAddr)
{
    return getValue<int32_t>(idx, baseAddr);
}

std::string& PARSymbol::getString(size_t idx, void *baseAddr) {
    return getValue<std::string>(idx, baseAddr);
}

float &PARSymbol::getFloat(size_t idx, void *baseAddr) {
    return getValue<float>(idx, baseAddr);
}
