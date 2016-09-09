#include <zenload/zCCSLib.h>
#include <vdfs/fileIndex.h>
#include <iostream>
#include <utils/logger.h>

/**
 * ----------------------------- main ------------------------------
 */
int main(int argc, char** argv)
{
    if(argc != 2)
    {
        std::cout   << "Usage: ou_load <vdf-archive>" << std::endl
        << "       <vdf-archive>: Path to the vdf-archive to load" << std::endl;
        return 0;
    }

    ZenLoad::zCCSLib lib(argv[1]);

    auto& msgs = lib.getData().blocks;
    for(auto& m : msgs)
    {
        LogInfo() << "Message: " << m.command.text;
    }

    return 0;
}
