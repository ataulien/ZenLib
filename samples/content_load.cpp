#include <zenload/zCProgMeshProto.h>
#include <vdfs/fileIndex.h>
#include <utils/export.h>
#include <iostream>

/**
 * ----------------------------- main ------------------------------
 */
int main(int argc, char** argv)
{
    if(argc != 3)
    {
        std::cout   << "Usage: content_load <vdf-archive> <mesh-name>" << std::endl
                    << "       <vdf-archive>: Path to the vdf-archive to load" << std::endl
                    << "       <mesh-name>: Name of the Mesh-File to load (.MRM-Format only!)" << std::endl;
        return 0;
    }
    
    // Create file-index to load our vdf-archive
    // See the vdf_unpack for more information
    VDFS::FileIndex vdf;
    vdf.loadVDF(argv[1]);
    
    // Content-Classes can load themselfes from a vdf-archive
    // Note: This works with most of the "zC*"-Classes defined in the ZenLoad-Namespace
    ZenLoad::zCProgMeshProto mesh(argv[2], vdf);
    
    // Bring the loaded mesh in a more accessible format
    ZenLoad::PackedMesh packedMesh;
    mesh.packMesh(packedMesh);
    
    std::cout << "Loaded Mesh with " << packedMesh.vertices.size() << " vertices and " << packedMesh.subMeshes.size() << " submeshes" << std::endl;


    std::string name = argv[2];
    name = name.substr(0, name.find_last_of('.'));
    std::string objfile = name + ".obj";

    std::cout << "Exporting to '" << objfile << ".obj'" << std::endl;

    if(!Utils::exportPackedMeshToObj(packedMesh, objfile, name))
        std::cout << "Export failed!";
    else
        std::cout << "Export succeeded!";

    return 0;
}
