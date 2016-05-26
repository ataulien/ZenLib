#include <iostream>
#include <zenload/DATFile.h>
#include <zenload/DaedalusVM.h>
#include <zenload/DaedalusStdlib.h>
#include <zenload/DaedalusDialogManager.h>
#include <utils/logger.h>
#include <utils/split.h>
#include <inttypes.h>
#include <stdlib.h>

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

void listNPCs(ZenLoad::DaedalusVM& vm)
{
    GameState::DaedalusGameState& s = vm.getGameState();
    auto& npcs = s.getRegisteredObjects().NPCs;

    LogInfo() << "Found " << npcs.getNumObtainedElements() << " NPCs";

    for(size_t i=0;i<npcs.getNumObtainedElements(); i++)
    {
        ZenLoad::GEngineClasses::C_Npc& n = npcs.getElements()[i];
        LogInfo() << "NPC: " << n.name[0];
        LogInfo() << " - Level: " << n.level;
        LogInfo() << " - WP: " << n.wp;
    }
}

void createPlayer(ZenLoad::DaedalusVM& vm)
{
    GameState::NpcHandle hero = vm.getGameState().createNPC();
    vm.initializeInstance(Memory::toBigHandle(hero), vm.getDATFile().getSymbolIndexByName("PC_Hero"), ZenLoad::IC_Npc);
    vm.setInstance("hero", Memory::toBigHandle(hero), ZenLoad::IC_Npc);
}

void runVM(ZenLoad::DaedalusVM& vm, bool print = true)
{
    if(print) LogInfo() << " -------------------------- RUNNING -------------------------- ";

    std::vector<ZenLoad::PARSymbol> storedSymbols = vm.getDATFile().getSymTable().symbols;

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

    ZenLoad::DATFile f(file);

    if(listFunctions)
    {
        LogInfo() << "Listing functions:";
        size_t i=0;
        for(const auto& s : f.getSymTable().symbols)
        {
            if(s.properties.elemProps.type == ZenLoad::EParType_Func)
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
            if(s.properties.elemProps.type == ZenLoad::EParType_String)
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
            LogInfo() << "[" << i << "] -- Symbol: " << s.name;

            i++;
        }
    }

    if(!main.empty() || cmdLine)
    {
        ZenLoad::DaedalusVM vm(f, main);

#include "lambdas.cpp"

        std::vector<std::pair<std::string, size_t>> debugOutput;
        vm.registerExternalFunction("printdebug", [&](ZenLoad::DaedalusVM& vm){
            std::string s = vm.popString(); if(verbose) LogInfo() << "s: " << s;

            if(!debugOutput.empty() && debugOutput.back().first == s)
                debugOutput.back().second++;
            else
                debugOutput.push_back(std::make_pair(s, 0));
        });

        vm.registerExternalFunction("printdebuginst", [&](ZenLoad::DaedalusVM& vm){
            std::string text = vm.popString(); if(verbose) LogInfo() << "text: " << text;

            if(!debugOutput.empty() && debugOutput.back().first == text)
                debugOutput.back().second++;
            else
                debugOutput.push_back(std::make_pair(text, 0));
        });

        vm.registerExternalFunction("printdebuginstch", [&](ZenLoad::DaedalusVM& vm){
            std::string text = vm.popString(); if(verbose) LogInfo() << "text: " << text;

            if(!debugOutput.empty() && debugOutput.back().first == text)
                debugOutput.back().second++;
            else
                debugOutput.push_back(std::make_pair(text, 0));
        });

        vm.registerExternalFunction("printdebugch", [&](ZenLoad::DaedalusVM& vm){
            std::string text = vm.popString(); if(verbose) LogInfo() << "text: " << text;

            if(!debugOutput.empty() && debugOutput.back().first == text)
                debugOutput.back().second++;
            else
                debugOutput.push_back(std::make_pair(text, 0));
        });

        ZenLoad::registerDaedalusStdLib(vm, verbose);
        ZenLoad::registerGothicEngineClasses(vm);
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
                GameState::NpcHandle target = Memory::handleCast<GameState::NpcHandle>(
                        vm.getDATFile().getSymbolByName("other").instanceDataHandle);

                LogInfo() << "Started talking with: " << vm.getGameState().getNpc(self).name[0];
                LogInfo() << "Options: ";

                for(size_t i=0;i<infos.size();i++)
                {
                    auto& info = vm.getGameState().getInfo(infos[i]);

                    // Check condition
                    std::string vstr;
                    if(info.condition)
                    {
                        // Most functions don't do a return
                        vm.pushState();

                        vm.pushInt(0);
                        vm.doCallOperation(info.condition);
                        runVM(vm, false);

                        int32_t valid = vm.popDataValue();
                        vstr = valid == 1 ? "    " : " [x]";

                        vm.popState();
                    }

                    if(info.description.empty())
                        LogInfo() << " - [" << i+1 << "]: [other]";
                    else
                        LogInfo() << " - [" << i+1 << "]" << vstr <<": " << info.description;
                }

                size_t choice;
                std::cout << "Choice: " << std::flush;
                std::cin >> choice;

                if(choice-1 < infos.size() & choice > 0)
                {
                    ZenLoad::GEngineClasses::C_Info& info = vm.getGameState().getInfo(infos[choice-1]);

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

                    if(choice != infos.size())
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

                        Memory::BigHandle h = vm.getDATFile().getSymbolByName(parts[1]).instanceDataHandle;
                        vm.setInstance("self", h, ZenLoad::IC_Npc);
                    } else if (parts[0] == "other")
                    {
                        if(parts.size() != 2)
                        {
                            LogInfo() << "Usage: other <symName>";
                            continue;
                        }

                        Memory::BigHandle h = vm.getDATFile().getSymbolByName(parts[1]).instanceDataHandle;
                        vm.setInstance("other", h, ZenLoad::IC_Npc);
                    } else if (parts[0] == "exit")
                    {
                        if(parts.size() != 2)
                        {
                            LogInfo() << "Usage: talk <npc>";
                            continue;
                        }

                        Memory::BigHandle h = vm.getDATFile().getSymbolByName(parts[1]).instanceDataHandle;
                        vm.setInstance("other", h, ZenLoad::IC_Npc);


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
                        if(parts.size() != 2)
                        {
                            LogInfo() << "Usage: talk <npc-name>";
                            continue;
                        }

                        auto& npcs = vm.getGameState().getRegisteredObjects().NPCs;

                        bool found = false;
                        for(size_t i=0;i<npcs.getNumObtainedElements();i++)
                        {
                            if(npcs.getElements()[i].name[0] == parts[1])
                            {
                                vm.pushState();
                                // Set up self and other
                                Memory::BigHandle s = vm.getDATFile().getSymbolByIndex(npcs.getElements()[i].instanceSymbol).instanceDataHandle;
                                vm.setInstance("self", s, ZenLoad::IC_Npc);

                                Memory::BigHandle o = vm.getDATFile().getSymbolByName("hero").instanceDataHandle;
                                vm.setInstance("other", o, ZenLoad::IC_Npc);

                                vm.doCallOperation(vm.getDATFile().getSymbolByName("ZS_Talk").address);
                                runVM(vm);
                                vm.popState();

                                found = true;
                                break;
                            }
                        }

                        if(!found)
                            LogInfo() << "Failed to find npc with name: " << parts[1];
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