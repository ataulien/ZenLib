#include "fileIndex.h"
#include "utils/logger.h"
#include "archive_virtual.h"
#include <locale>
#include <algorithm>
#include <physfs.h>

using namespace VDFS;

FileIndex::FileIndex(const char *argv0)
{
    if (!PHYSFS_isInit()) PHYSFS_init(argv0);
}

FileIndex::~FileIndex()
{
    if (PHYSFS_isInit()) PHYSFS_deinit();
}

/**
* @brief Loads a VDF-File and initializes everything
*/
bool FileIndex::loadVDF(const std::string& vdf, uint32_t priority, const std::string& mountPoint)
{
    if (!PHYSFS_mount(vdf.c_str(), mountPoint.c_str(), 1))
    {
        LogInfo() << "Couldn't load VDF-Archive " << vdf << ": " << PHYSFS_getLastError();
        return false;
    }
	return true;
}

/**
* @brief Fills a vector with the data of the given file
*/
bool FileIndex::getFileData(const std::string& file, std::vector<uint8_t>& data) const
{
    PHYSFS_File *handle = PHYSFS_openRead(file.c_str());
    if (!handle)
    {
        LogInfo() << "Cannot read file " << file << ": " << PHYSFS_getLastError();
        return false;
    }

    auto length = PHYSFS_fileLength(handle);
    data.resize(length);
    if (PHYSFS_read(handle, data.data(), 1, length) < length)
    {
        LogInfo() << "Cannot read file " << file << ": " << PHYSFS_getLastError();
        PHYSFS_close(handle);
        return false;
    }
    PHYSFS_close(handle);
    return true;
}

bool FileIndex::hasFile(const std::string& name) const
{
    return PHYSFS_exists(name.c_str());
}

std::vector<std::string> FileIndex::getKnownFiles(const std::string& path) const
{
    char **files = PHYSFS_enumerateFiles(path.c_str());
    char **i;
    std::vector<std::string> vec;
    for (i = files; *i != NULL; i++)
        vec.push_back(*i);
    PHYSFS_freeList(files);
    return vec;
}
