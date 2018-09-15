#include <iostream>
#include <daedalus/DATFile.h>
#include <daedalus/DaedalusVM.h>
#include <daedalus/DaedalusStdlib.h>
#include <daedalus/DaedalusDialogManager.h>
#include <utils/logger.h>
#include <utils/split.h>
#include <inttypes.h>
#include <stdlib.h>
#include <algorithm>

using namespace Daedalus;

bool listFunctions = false;
bool listStrings = false;
bool cmdLine = false;
bool listSymbols = false;
bool verbose = false;
std::string ou_file;

void printDebugOutput(const std::vector< std::pair<std::string, size_t>>& dbg)
{
    for(auto& l : dbg)
    {
        if(l.second > 1)
            LogInfo() << "DBG: " << l.first << " (" << l.second << "x)";
        else
            LogInfo() << "DBG: " << l.first;
    }
}

void listNPCs(Daedalus::DaedalusVM& vm)
{
    GameState::DaedalusGameState& s = vm.getGameState();
    auto& npcs = s.getRegisteredObjects().NPCs;

    LogInfo() << "Found " << npcs.getNumObtainedElements() << " NPCs";

    for(size_t i=0;i<npcs.getNumObtainedElements(); i++)
    {
        Daedalus::GEngineClasses::C_Npc& n = npcs.getElements()[i];
        LogInfo() << "NPC: " << n.name[0];
        LogInfo() << " - Level: " << n.level;
        LogInfo() << " - WP: " << n.wp;
    }
}

size_t npcInstanceFromName(Daedalus::DaedalusVM& vm, const std::string& n)
{
    auto& npcs = vm.getGameState().getRegisteredObjects().NPCs;
    for(size_t i=0;i<npcs.getNumObtainedElements();i++)
    {
        if(npcs.getElements()[i].name[0] == n)
        {
            return npcs.getElements()[i].instanceSymbol;
        }
    }

    return static_cast<size_t>(-1);
}

std::string join(const std::vector<std::string>& v, const std::string& delim)
{
    std::string s;

    for(size_t i=0;i<v.size();i++)
    {
        if(!s.empty())
        {
            s += delim;
        }

        s += v[i];
    }

    return s;
}

void createPlayer(Daedalus::DaedalusVM& vm)
{
    GameState::NpcHandle hero = vm.getGameState().createNPC();
    vm.initializeInstance(ZMemory::toBigHandle(hero), vm.getDATFile().getSymbolIndexByName("PC_Hero"), Daedalus::IC_Npc);
    vm.setInstance("hero", ZMemory::toBigHandle(hero), Daedalus::IC_Npc);
}

void runVM(Daedalus::DaedalusVM& vm, bool print = true)
{
    if(print) LogInfo() << " -------------------------- RUNNING -------------------------- ";

    std::vector<Daedalus::PARSymbol> storedSymbols = vm.getDATFile().getSymTable().symbols;

    while (vm.doStack(verbose));

    if(print) LogInfo() << " -------------------------- FINISHED -------------------------- ";

    if(verbose)
    {
        for (size_t i = 0; i < vm.getDATFile().getSymTable().symbols.size(); i++)
        {
            if (i >= storedSymbols.size())
                continue;

            auto& s = vm.getDATFile().getSymTable().symbols[i];
            auto& o = storedSymbols[i];

            if (s.isDataSame(o))
                continue;

            LogInfo() << "Changed Symbol: " << s.name << " (" << i << "):";

            if (!s.intData.empty())
            {
                for (size_t j = 0; j < s.intData.size(); j++)
                {
                    if (j >= o.intData.size())
                        LogInfo() << " - Int[" << j << "]: (new) " << s.intData[j];
                    else if (o.intData[j] != s.intData[j])
                        LogInfo() << " - Int[" << j << "]: " << o.intData[j] << " -> " << s.intData[j];
                }
            }

            if (!s.strData.empty())
            {
                for (size_t j = 0; j < s.strData.size(); j++)
                {
                    if (j >= o.strData.size())
                        LogInfo() << " - String[" << j << "]: (new) " << s.strData[j];
                    else if (o.strData[j] != s.strData[j])
                        LogInfo() << " - String[" << j << "]: " << o.strData[j] << " -> " << s.strData[j];
                }
            }

            if (!s.floatData.empty())
            {
                for (size_t j = 0; j < s.floatData.size(); j++)
                {
                    if (j >= o.floatData.size())
                        LogInfo() << " - Float[" << j << "]: (new) " << s.floatData[j];
                    else if (o.floatData[j] != s.floatData[j])
                        LogInfo() << " - Float[" << j << "]: " << o.floatData[j] << " -> " << s.floatData[j];
                }
            }
        }
    }
}

/**
 * ----------------------------- main ------------------------------
 */
int main(int argc, char** argv)
{
    setlocale(LC_CTYPE, "");

    /*if (argc != 2) {
        std::cout << "Usage: dat_load <dat-file>" << std::endl
                  << "(No spaces in path or filename)" << std::endl;
        return 0;
    }*/

    std::vector<std::string> parts;
    for(size_t i=1;i<argc;i++)
        parts.push_back(argv[i]);

    std::string file;
    std::string main;

    for(size_t i=0;i<parts.size();i+=2)
    {
        // Single flag args
        if(parts[i] == "-lf")
        {
            listFunctions = true;
            i--; // No arg here
            continue;
        }else if(parts[i] == "-ls")
        {
            listStrings = true;
            i--; // No arg here
            continue;
        }else if(parts[i] == "-c")
        {
            cmdLine = true;

            i--; // No arg here
            continue;
        }else if(parts[i] == "-ly")
        {
            listSymbols = true;
            i--; // No arg here
            continue;
        }
        else if(parts[i] == "-v")
        {
            verbose = true;
            i--; // No arg here
            continue;
        }


        if(i+1 == parts.size())
        {
            LogError() << "Missing argument for: " << parts[i];
            return 0;
        }

        if(parts[i] == "-m")
            main = parts[i+1];
        else if(parts[i] == "-f")
            file = parts[i+1];
        else if(parts[i] == "-ou")
        {
            ou_file = parts[i+1];
            continue;
        }
    }

    if(file.empty())
    {
        LogError() << "No file specified. Please use -f <file>.";
        return 0;
    }

    Daedalus::DATFile f(file);

    if(listFunctions)
    {
        LogInfo() << "Listing functions:";
        size_t i=0;
        for(const auto& s : f.getSymTable().symbols)
        {
            if(s.properties.elemProps.type == Daedalus::EParType_Func)
            {
                LogInfo() << "[" << i << "] - '" << s.name << "', at: " << s.address;
            }

            i++;
        }
    }

    if(listStrings)
    {
        LogInfo() << "Listing strings:";
        size_t i=0;
        for(const auto& s : f.getSymTable().symbols)
        {
            if(s.properties.elemProps.type == Daedalus::EParType_String)
            {
                if(s.strData.size() == 1 && !s.strData[0].empty())
                {
                    LogInfo() << "[" << i << "] - " << s.name << ": " << s.strData[0];
                }else
                {
                    LogInfo() << "[" << i << "]  - " << s.name;
                    for (size_t i = 0; i < s.strData.size(); i++)
                        if (!s.strData[i].empty())
                            LogInfo() << "[" << i << "]  -- String[" << i << "]: " << s.strData[i];
                }
            }

            i++;
        }
    }

    if(listSymbols)
    {
        int i=0;
        for(const auto& s : f.getSymTable().symbols)
        {
            LogInfo() << "[" << i << "] -- Symbol: " << s.name << ", Offset: " << s.classOffset;

            i++;
        }
    }


    return 0;
}
