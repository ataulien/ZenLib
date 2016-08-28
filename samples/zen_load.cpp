#include <vdfs/fileIndex.h>
#include <zenload/zenParser.h>
#include <zenload/zCMesh.h>
#include <iostream>
#include <utils/export.h>

/**
 * Recursive function to list some data about the zen-file
 */
void listVobInformation(const std::vector<ZenLoad::zCVobData>& vobs)
{
    for(const ZenLoad::zCVobData& v : vobs)
    {
        if(!v.visual.empty())
        {
            // More information about what a vob stores can be found in the zTypes.h-File
            std::cout << "Vob at " << v.position.toString() << ", Visual: " << v.visual << std::endl;
        }
        
        // List the information about the children as well
        listVobInformation(v.childVobs);
    }   
}

/**
 * ----------------------------- main ------------------------------
 */
int main(int argc, char** argv)
{
    if(argc != 3)
    {
        std::cout   << "Usage: zen_load <vdf-archive> <zen-name>" << std::endl
                    << "       <vdf-archive>: Path to the vdf-archive to load" << std::endl
                    << "       <zen-name>: Name of the .ZEN-File inside the given vdf-archive (name only, no folders)" << std::endl;
        return 0;
    } 
    
    // Create file-index to load our vdf-archive
    // See the vdf_unpack for more information
    VDFS::FileIndex vdf;
    vdf.loadVDF(argv[1]);
    

	// Initialize parser with zenfile from vdf
	ZenLoad::ZenParser parser(argv[2], vdf);

	if(parser.getFileSize() == 0)
	{
		std::cout << "Error: ZEN-File either not found or empty!" << std::endl;
		return 0;
	}

	// Since this is a usual level-zen, read the file header
	// You will most likely allways need to do that
	parser.readHeader();

	// Do something with the header, if you want.
	std::cout << "Author: " << parser.getZenHeader().user << std::endl
		<< "Date: " << parser.getZenHeader().date << std::endl
		<< "Object-count (optional): " << parser.getZenHeader().objectCount << std::endl;

	// Read the rest of the ZEN-file
	ZenLoad::oCWorldData world;
	parser.readWorld(world);

	std::cout << "Done reading ZEN!" << std::endl;
	
    ZenLoad::PackedMesh packedWorldMesh;
    parser.getWorldMesh()->packMesh(packedWorldMesh, 0.01f);
	
    // Print some sample-data for vobs which got a visual
    /*std::cout << "Listing vobs..." << std::endl;
    listVobInformation(world.rootVobs);

    std::cout << "Listing waypoints..." << std::endl;
    for(const ZenLoad::zCWaypointData& v : world.waynet.waypoints)
    {
        std::cout << "Waypoint [" << v.wpName << "] at " << v.position.toString() << std::endl;
    }
    */
    
    std::cout << "Exporting obj..." << std::endl;
    
    Utils::exportPackedMeshToObj(packedWorldMesh, (argv[2] + std::string(".OBJ")));

    return 0;
}
