#include "fileIndex.h"
#include "utils/logger.h"
#include <locale>
#include <algorithm>
#include <physfs.h>
#include "../lib/physfs/extras/ignorecase.h"

using namespace VDFS;

FileIndex::FileIndex()
{
}

FileIndex::~FileIndex()
{
    if (PHYSFS_isInit()) PHYSFS_deinit();
}

void FileIndex::Init(const char *argv0)
{
    if (!PHYSFS_isInit()) PHYSFS_init(argv0);
}

/**
* @brief Loads a VDF-File and initializes everything
*/
bool FileIndex::loadVDF(const std::string& vdf, uint32_t priority, const std::string& mountPoint)
{
    if (!PHYSFS_mount(vdf.c_str(), mountPoint.c_str(), 1))
    {
        LogInfo() << "Couldn't load VDF-Archive " << vdf << ": " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
        return false;
    }
	return true;
}

/**
* @brief Fills a vector with the data of the given file
*/
bool FileIndex::getFileData(const std::string& file, std::vector<uint8_t>& data) const
{
    std::vector<char> filePath(file.begin(), file.end());
    filePath.push_back('\0');
    bool exists = PHYSFSEXT_locateCorrectCase(filePath.data()) == 0;

    if (!exists) return false;

    PHYSFS_File *handle = PHYSFS_openRead(filePath.data());
    if (!handle)
    {
        LogInfo() << "Cannot read file " << file << ": " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
        return false;
    }

    auto length = PHYSFS_fileLength(handle);
    data.resize(length);
    if (PHYSFS_readBytes(handle, data.data(), length) < length)
    {
        LogInfo() << "Cannot read file " << file << ": " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
        PHYSFS_close(handle);
        return false;
    }
    PHYSFS_close(handle);
    return true;
}

bool FileIndex::hasFile(const std::string& name) const
{
    std::vector<char> filePath(name.begin(), name.end());
    filePath.push_back('\0');
    bool exists = PHYSFSEXT_locateCorrectCase(filePath.data()) == 0;
    return exists;
}

std::vector<std::string> FileIndex::getKnownFiles(const std::string& path) const
{
    std::vector<std::string> vec;
    std::vector<char> filePath(path.begin(), path.end());
    filePath.push_back('\0');
    bool exists = PHYSFSEXT_locateCorrectCase(filePath.data()) == 0;
    if (!exists) return vec;

    char **files = PHYSFS_enumerateFiles(filePath.data());
    char **i;
    for (i = files; *i != NULL; i++)
        vec.push_back(*i);
    PHYSFS_freeList(files);
    return vec;
}
