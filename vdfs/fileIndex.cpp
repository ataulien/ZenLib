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
    char* filePathBuffer = new char[file.length() + 1];
    memcpy(filePathBuffer, file.c_str(), file.length() + 1);
    bool exists = PHYSFSEXT_locateCorrectCase(filePathBuffer) == 0;

    if (!exists) goto fail;

    PHYSFS_File *handle = PHYSFS_openRead(filePathBuffer);
    if (!handle)
    {
        LogInfo() << "Cannot read file " << file << ": " << PHYSFS_getLastError();
        goto fail;
    }

    auto length = PHYSFS_fileLength(handle);
    data.resize(length);
    if (PHYSFS_read(handle, data.data(), 1, length) < length)
    {
        LogInfo() << "Cannot read file " << file << ": " << PHYSFS_getLastError();
        PHYSFS_close(handle);
        goto fail;
    }
    delete filePathBuffer;
    PHYSFS_close(handle);
    return true;

fail:
    delete filePathBuffer;
    return false;
}

bool FileIndex::hasFile(const std::string& name) const
{
    char* filePathBuffer = new char[name.length() + 1];
    memcpy(filePathBuffer, name.c_str(), name.length() + 1);
    bool exists = PHYSFSEXT_locateCorrectCase(filePathBuffer) == 0;
    delete filePathBuffer;
    return exists;
}

std::vector<std::string> FileIndex::getKnownFiles(const std::string& path) const
{
    std::vector<std::string> vec;
    char* filePathBuffer = new char[path.length() + 1];
    memcpy(filePathBuffer, path.c_str(), path.length() + 1);
    bool exists = PHYSFSEXT_locateCorrectCase(filePathBuffer) == 0;
    if (!exists) {
        delete filePathBuffer;
        return vec;
    }

    char **files = PHYSFS_enumerateFiles(filePathBuffer);
    char **i;
    for (i = files; *i != NULL; i++)
        vec.push_back(*i);
    PHYSFS_freeList(files);
    delete filePathBuffer;
    return vec;
}
