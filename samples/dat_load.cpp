#include <iostream>
#include <zenload/DATFile.h>
#include <zenload/DaedalusVM.h>
#include <zenload/DaedalusStdlib.h>
#include <utils/logger.h>
#include <utils/split.h>
#include <inttypes.h>
#include <stdlib.h>


void runVM(ZenLoad::DaedalusVM& vm)
{
    LogInfo() << " -------------------------- RUNNING -------------------------- ";

    std::vector<ZenLoad::PARSymbol> storedSymbols = vm.getDATFile().getSymTable().symbols;

    while (vm.doStack());

    LogInfo() << " -------------------------- FINISHED -------------------------- ";

    for (size_t i = 0; i < vm.getDATFile().getSymTable().symbols.size(); i++) {
        if (i >= storedSymbols.size())
            continue;

        auto& s = vm.getDATFile().getSymTable().symbols[i];
        auto& o = storedSymbols[i];

        if (s.isDataSame(o))
            continue;

        /*LogInfo() << "Changed Symbol: " << s.name << " (" << i << "):";

        if (!s.intData.empty()) {
            for (size_t j = 0; j < s.intData.size(); j++) {
                if (j >= o.intData.size())
                    LogInfo() << " - Int[" << j << "]: (new) " << s.intData[j];
                else if (o.intData[j] != s.intData[j])
                    LogInfo() << " - Int[" << j << "]: " << o.intData[j] << " -> " << s.intData[j];
            }
        }

        if (!s.strData.empty()) {
            for (size_t j = 0; j < s.strData.size(); j++) {
                if (j >= o.strData.size())
                    LogInfo() << " - String[" << j << "]: (new) " << s.strData[j];
                else if (o.strData[j] != s.strData[j])
                    LogInfo() << " - String[" << j << "]: " << o.strData[j] << " -> " << s.strData[j];
            }
        }

        if (!s.floatData.empty()) {
            for (size_t j = 0; j < s.floatData.size(); j++) {
                if (j >= o.floatData.size())
                    LogInfo() << " - Float[" << j << "]: (new) " << s.floatData[j];
                else if (o.floatData[j] != s.floatData[j])
                    LogInfo() << " - Float[" << j << "]: " << o.floatData[j] << " -> " << s.floatData[j];
            }
        }*/
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
    bool listFunctions = false;
    bool listStrings = false;
    bool cmdLine = false;
    bool listSymbols = false;
    for(size_t i=0;i<parts.size();i+=2)
    {
        LogInfo() << "Arg: " << parts[i];

        // Single flag args
        if(parts[i] == "-lf")
        {
            listFunctions = true;
            continue;
        }else if(parts[i] == "-ls")
        {
            listStrings = true;
            continue;
        }else if(parts[i] == "-c")
        {
            cmdLine = true;
            continue;
        }else if(parts[i] == "-ly")
        {
            listSymbols = true;
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

        std::vector<std::string> debugOutput;
        vm.registerExternalFunction("printdebug", [&](ZenLoad::DaedalusVM& vm){
            std::string s = vm.popString(); LogInfo() << "s: " << s;

            debugOutput.push_back(s);
        });

        vm.registerExternalFunction("printdebuginst", [&](ZenLoad::DaedalusVM& vm){
            std::string text = vm.popString(); LogInfo() << "text: " << text;

            debugOutput.push_back(text);
        });

        vm.registerExternalFunction("printdebuginstch", [&](ZenLoad::DaedalusVM& vm){
            std::string text = vm.popString(); LogInfo() << "text: " << text;

            debugOutput.push_back(text);
        });

        vm.registerExternalFunction("printdebugch", [&](ZenLoad::DaedalusVM& vm){
            std::string text = vm.popString(); LogInfo() << "text: " << text;

            debugOutput.push_back(text);
        });

        ZenLoad::registerDaedalusStdLib(vm);
        ZenLoad::registerGothicEngineClasses(vm);

        //size_t self = vm.getDATFile().addSymbol();
        //vm.pushVar(self);

        if(!main.empty()) {
            runVM(vm);

            for(std::string& s : debugOutput)
                LogInfo() << "DBG: " << s;
            debugOutput.clear();
        } else
        {
            while (true)
            {
                if (cmdLine)
                {
                    // Read commandline
                    std::cout << "> ";
                    std::string cmd;
                    std::getline(std::cin, cmd);

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

                        const std::set<void*>& npcs = vm.getRegisteredInstancesOf(ZenLoad::GEngineClasses::IC_Npc);
                        for(auto& np : npcs)
                        {
                            ZenLoad::GEngineClasses::C_NPC* n = (ZenLoad::GEngineClasses::C_NPC*)np;
                            LogInfo() << "NPC: " << n->name[0];
                            LogInfo() << " - Level: " << n->level;
                            LogInfo() << " - WP: " << n->wp;
                        }

                        //for (std::string& s : debugOutput)
                        //    LogInfo() << "DBG: " << s;
                        debugOutput.clear();
                    } else if (parts[0] == "exit")
                    {
                        break;
                    }
                }
            }
        }
    }



    return 0;
}