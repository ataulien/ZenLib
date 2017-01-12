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
        LogInfo() << "Arg: " << parts[i];

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

    if(!main.empty() || cmdLine)
    {
        Daedalus::DaedalusVM vm(file, main);

#include "lambdas.cpp"

        std::vector<std::pair<std::string, size_t>> debugOutput;
        vm.registerExternalFunction("printdebug", [&](Daedalus::DaedalusVM& vm){
            std::string s = vm.popString(); if(verbose) LogInfo() << "s: " << s;

            if(!debugOutput.empty() && debugOutput.back().first == s)
                debugOutput.back().second++;
            else
                debugOutput.push_back(std::make_pair(s, 0));
        });

        vm.registerExternalFunction("printdebuginst", [&](Daedalus::DaedalusVM& vm){
            std::string text = vm.popString(); if(verbose) LogInfo() << "text: " << text;

            if(!debugOutput.empty() && debugOutput.back().first == text)
                debugOutput.back().second++;
            else
                debugOutput.push_back(std::make_pair(text, 0));
        });

        vm.registerExternalFunction("printdebuginstch", [&](Daedalus::DaedalusVM& vm){
            std::string text = vm.popString(); if(verbose) LogInfo() << "text: " << text;

            if(!debugOutput.empty() && debugOutput.back().first == text)
                debugOutput.back().second++;
            else
                debugOutput.push_back(std::make_pair(text, 0));
        });

        vm.registerExternalFunction("printdebugch", [&](Daedalus::DaedalusVM& vm){
            std::string text = vm.popString(); if(verbose) LogInfo() << "text: " << text;

            if(!debugOutput.empty() && debugOutput.back().first == text)
                debugOutput.back().second++;
            else
                debugOutput.push_back(std::make_pair(text, 0));
        });

        registerDaedalusStdLib(vm, verbose);
        registerGothicEngineClasses(vm);
        vm.getGameState().registerExternals();

        // Create hero
        createPlayer(vm);

        if(!ou_file.empty())
        {
            GameState::DaedalusDialogManager* dm = new GameState::DaedalusDialogManager(vm, ou_file);


            auto onAIOutput = [&](GameState::NpcHandle self, GameState::NpcHandle target, const ZenLoad::oCMsgConversationData& msg)
            {
                LogInfo() << vm.getGameState().getNpc(self).name[0] << ": " << msg.text;
            };

            auto onAIProcessInfos = [&](GameState::NpcHandle self, std::vector<GameState::InfoHandle> infos)
            {
                GameState::NpcHandle target = ZMemory::handleCast<GameState::NpcHandle>(
                        vm.getDATFile().getSymbolByName("other").instanceDataHandle);

                LogInfo() << "Started talking with: " << vm.getGameState().getNpc(self).name[0];
                LogInfo() << "Options: ";

                std::map<size_t, size_t> optionsMap;
                std::vector<std::pair<size_t, size_t>> optionsSorted;
                for(size_t i=0;i<infos.size();i++)
                {
                    auto& info = vm.getGameState().getInfo(infos[i]);

                    //LogInfo() << " ---------- Checking: " << info.description << " ------------ ";

                    // Check condition
                    std::string vstr;
                    int32_t valid = 0;
                    if(info.condition)
                    {
                        // Most functions don't do a return
                        vm.pushState();

                        vm.pushInt(0);
                        vm.doCallOperation(info.condition);
                        runVM(vm, false);

                        valid = vm.popDataValue();
                        //LogInfo() << "#### Valid: " << valid;

                        vm.popState();
                    }

                    if(valid && !info.description.empty())
                    {
                        //LogInfo() << info.nr << ": value, '" << info.description << "'";
                        optionsMap[info.nr] = i;
                        optionsSorted.push_back(std::make_pair(info.nr, i));
                    }
                    //if(info.description.empty())
                    //    LogInfo() << " - [" << i+1 << "]: [other]";
                    //else
                    //    LogInfo() << " - [" << i+1 << "]" << vstr <<": " << info.description;
                }

                std::sort(optionsSorted.begin(), optionsSorted.end(),
                          [](const std::pair<size_t, size_t>& l, const std::pair<size_t, size_t>& r)
                          {return l.first < r.first;});

                LogInfo() << "";
                for(size_t i=0;i<optionsSorted.size();i++) {
                    auto& info = vm.getGameState().getInfo(infos[optionsSorted[i].second]);
                    LogInfo() << " - [" << i + 1 << "]" << ": " << info.description;
                }
                LogInfo() << "";

                size_t choice;
                std::cout << "Choice: " << std::flush;
                std::cin >> choice;

                LogInfo() << "";

                if(choice-1 < optionsSorted.size())
                {
                    Daedalus::GEngineClasses::C_Info& info = vm.getGameState().getInfo(infos[optionsSorted[choice-1].second]);

                    vm.pushState();
                    vm.doCallOperation(info.information);
                    runVM(vm, false);
                    vm.popState();

                    // Register information
                    dm->setNpcInfoKnown(vm.getGameState().getNpc(target).instanceSymbol, info.instanceSymbol);

                    // End dialog
                    vm.pushState();
                    vm.doCallOperation(vm.getDATFile().getSymbolByName("ZS_Talk_Loop").address);
                    runVM(vm, false);
                    vm.popState();

                    if(info.nr != 999)
                    {
                        // END wasn't selected, bring up dialog again
                        vm.pushState();
                        vm.doCallOperation(vm.getDATFile().getSymbolByName("ZS_Talk").address);
                        runVM(vm);
                        vm.popState();
                    }
                } else {
                    LogInfo() << "Invalid choice!";
                }

            };

            dm->registerExternals(onAIOutput, onAIProcessInfos);
        }



        //size_t self = vm.getDATFile().addSymbol();
        //vm.pushVar(self);

        if(!main.empty()) {
            runVM(vm);

            //listNPCs(vm);

            printDebugOutput(debugOutput);
            debugOutput.clear();
        } else
        {
            while (true)
            {
                if (cmdLine)
                {
                    // Read commandline
                    std::cout << "> " << std::flush;
                    std::string cmd;
                    std::getline(std::cin, cmd);

                    if(cmd.empty())
                        continue;

                    // Split into arguments
                    parts = Utils::split(cmd, ' ');

                    LogInfo() << cmd;

                    // First part is expected to be one of the following: "exec", [todo]
                    if (parts[0] == "exec")
                    {
                        // Arguments for exec are to be given like this:
                        // fname <vtype>:value1 <vtype2>:value2 ...
                        // Example: exec myFunction i:3 s:hello! v:SELF
                        // TODO: Support spaces in strings
                        if (parts.size() < 2)
                        {
                            std::cout << "Missing argument for exec!" << std::endl;
                            continue;
                        }

                        std::string fn = parts[1];
                        std::vector<std::string> args(parts.begin() + 2, parts.end());

                        if (!vm.getDATFile().hasSymbolName(fn))
                        {
                            std::cout << "Failed to find function: " << fn << std::endl;
                            continue;
                        }

                        vm.pushState();

                        uint32_t fnAddress = vm.getDATFile().getSymbolByName(fn).address;
                        int32_t i;
                        float flt;
                        std::string s;
                        size_t vidx;
                        for (std::string& a : args)
                        {
                            switch (a.front())
                            {
                                case 'i':
                                    i = atoi(a.c_str() + 2);
                                    vm.pushInt(i);
                                    break;

                                case 'f':
                                    flt = atof(a.c_str() + 2);
                                    vm.pushInt(*reinterpret_cast<uint32_t*>(&flt));
                                    break;

                                case 's':
                                    s = a.c_str() + 2;
                                    vm.pushString(s);
                                    LogInfo() << "s:" << s;
                                    break;

                                case 'v':
                                    s = a.c_str() + 2;
                                    vidx = vm.getDATFile().getSymbolIndexByName(s);
                                    vm.pushVar(vidx);
                                    break;

                            }
                        }


                        vm.doCallOperation(fnAddress);
                        runVM(vm);
                        vm.popState();

                        //listNPCs(vm);

                        if(verbose)
                        {
                            printDebugOutput(debugOutput);
                            debugOutput.clear();
                        }
                    } else if (parts[0] == "self")
                    {
                        if(parts.size() != 2)
                        {
                            LogInfo() << "Usage: self <symName>";
                            continue;
                        }

                        ZMemory::BigHandle h = vm.getDATFile().getSymbolByName(parts[1]).instanceDataHandle;
                        vm.setInstance("self", h, Daedalus::IC_Npc);
                    } else if (parts[0] == "other")
                    {
                        if(parts.size() != 2)
                        {
                            LogInfo() << "Usage: other <symName>";
                            continue;
                        }

                        ZMemory::BigHandle h = vm.getDATFile().getSymbolByName(parts[1]).instanceDataHandle;
                        vm.setInstance("other", h, Daedalus::IC_Npc);
                    } else if (parts[0] == "exit")
                    {
                        if(parts.size() != 2)
                        {
                            LogInfo() << "Usage: talk <npc>";
                            continue;
                        }

                        ZMemory::BigHandle h = vm.getDATFile().getSymbolByName(parts[1]).instanceDataHandle;
                        vm.setInstance("other", h, Daedalus::IC_Npc);


                    }
                    else if (parts[0] == "verbose")
                    {
                        if(parts.size() != 2)
                        {
                            LogInfo() << "Usage: verbose <0|1>";
                            continue;
                        }

                        if(parts[1] == "0")
                            verbose = false;
                        else
                            verbose = true;
                    }else if (parts[0] == "talk")
                    {
                        auto& npcs = vm.getGameState().getRegisteredObjects().NPCs;

                        parts.erase(parts.begin());

                        std::string cname = join(parts, " ");

                        size_t inst = npcInstanceFromName(vm, cname);

                        if(inst != static_cast<uint32_t>(-1))
                        {
                            vm.pushState();
                            // Set up self and other
                            ZMemory::BigHandle s = vm.getDATFile().getSymbolByIndex(inst).instanceDataHandle;
                            vm.setInstance("self", s, Daedalus::IC_Npc);

                            ZMemory::BigHandle o = vm.getDATFile().getSymbolByName("hero").instanceDataHandle;
                            vm.setInstance("other", o, Daedalus::IC_Npc);

                            vm.doCallOperation(vm.getDATFile().getSymbolByName("ZS_Talk").address);
                            runVM(vm);
                            vm.popState();
                        }
                        else
                            LogInfo() << "Failed to find npc with name: " << cname;
                    }else if (parts[0] == "log")
                    {

                        for(auto& t : vm.getGameState().getPlayerLog())
                        {
                            LogInfo() << " --- " << t.second.topic << " --- ";
                            std::string ents;
                            for(const std::string& s : t.second.entries)
                                if(s.empty())
                                        ents += s + "\n";
                                    else
                                        ents += "        \n        ---------\n        "
                                            + s + "\n";

                            LogInfo() << ents;
                        }
                    }
                    else if (parts[0] == "inv")
                    {
                        auto& npcs = vm.getGameState().getRegisteredObjects().NPCs;

                        parts.erase(parts.begin());

                        std::string cname = join(parts, " ");

                        size_t inst = npcInstanceFromName(vm, cname);

                        if(inst != static_cast<uint32_t>(-1))
                        {
                            GameState::NpcHandle h = ZMemory::handleCast<GameState::NpcHandle>(vm.getDATFile().getSymbolByIndex(inst).instanceDataHandle);

                            //vm.getGameState().getInventory(h)

                        }
                        else
                            LogInfo() << "Failed to find npc with name: " << cname;
                    }else if (parts[0] == "exit")
                    {
                        break;
                    }
                }
            }
        }
    }



    return 0;
}
