#include <iostream>
#include <vdfs/fileIndex.h>

/**
 * Utility function to write data to disk
 */
bool writeFile(const std::string& name, const std::string& path, const std::vector<uint8_t>& data)
{
    std::string sep = (path.back() == '/' || path.back() == '\\') ? "" : "/";
    std::string target = path + sep + name;

    FILE* f = fopen(target.c_str(), "wb");
    if (!f)
        return false;

    fwrite(data.data(), data.size(), 1, f);

    fclose(f);

    return true;
}

/**
 * ----------------------------- main ------------------------------
 */
int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: vdf_unpack <vdf-archive> <target-path>" << std::endl
                  << "       <vdf-archive>: Path to the vdf-archive to load" << std::endl
                  << "       <target-path>: Path to unpack the files to" << std::endl;
        return 0;
    }


    // Initialize the system
    VDFS::FileIndex::initVDFS(argv[0]);

    const std::string archiveToLoad = argv[1];
    const std::string unpackDestination = argv[2];

    // Create file-index to hold information about the files inside the archives
    VDFS::FileIndex vdf;

    // Load the archive specified by the user.
    // Note: You can call this method again to load more archives and add more files to the index
    vdf.loadVDF(archiveToLoad);

    // Adter every VDF-Archive is loaded, the index has to be built
    vdf.finalizeLoad();

    // Print how many files we loaded
    std::cout << "Loaded " << vdf.getKnownFiles().size() << " files into fileIndex!" << std::endl;

    // List all known files
    for (auto fileName : vdf.getKnownFiles())
    {
        // Print some information about the file
        std::cout << fileName << std::endl;

        // Get the data of the file
        std::vector<uint8_t> fileBytes;

        // You can access a file by their FileInfo-Descriptor or by name.
        // This would work as well:
        //
        //      vdf.getFileData("AddonWorld.zen", data);
        //
        // Filenames are non case sensitive
        vdf.getFileData(fileName, fileBytes);

        // Write the file to disk
        if (!writeFile(fileName, unpackDestination, fileBytes))
            std::cout << " - Failed to write file!" << std::endl;
    }

    return 0;
}
