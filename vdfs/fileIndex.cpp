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

bool FileIndex::mountFolder(const std::string& path, const std::string& mountPoint)
{
    if(!PHYSFS_mount(path.c_str(), mountPoint.c_str(), 1))
    {
        LogInfo() << "Couldn't mount directory " << path << ": " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
        return false;
    }

    return true;
}

/**
* @brief Fills a vector with the data of the given file
*/
bool FileIndex::getFileData(const std::string& file, std::vector<uint8_t>& data) const
{
    std::string caseSensitivePath = findCaseSensitiveNameOf(file);
    bool exists = caseSensitivePath != "";

    if (!exists) return false;

    PHYSFS_File *handle = PHYSFS_openRead(caseSensitivePath.c_str());
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
    return findCaseSensitiveNameOf(name) != "";
}

std::vector<std::string> FileIndex::getKnownFiles(const std::string& path) const
{
    std::string filePath(path);
    std::vector<std::string> vec;
    bool exists = PHYSFSEXT_locateCorrectCase(&filePath[0]) == 0;
    if (!exists) {
        return vec;
    }

    char **files = PHYSFS_enumerateFiles(filePath.c_str());
    char **i;
    for (i = files; *i != NULL; i++)
        vec.push_back(*i);
    PHYSFS_freeList(files);
    return vec;
}

void FileIndex::updateUpperedFilenamesMap()
{
    std::vector<std::string> files = getKnownFiles();

    m_FilenamesByUpperedFileNames.clear();
    for(const std::string& file : files)
    {
        std::string uppered = file;
        std::transform(uppered.begin(), uppered.end(), uppered.begin(), ::toupper);

        m_FilenamesByUpperedFileNames[uppered] = file;
    }
}

std::string FileIndex::findCaseSensitiveNameOf(const std::string& caseInsensitiveName) const
{
    std::string uppered = caseInsensitiveName;
    std::transform(caseInsensitiveName.begin(), caseInsensitiveName.end(), uppered.begin(), ::toupper);

    auto it = m_FilenamesByUpperedFileNames.find(uppered);

    if(it == m_FilenamesByUpperedFileNames.end())
        return "";

    return it->second;
}

void FileIndex::finalizeLoad()
{
    // Must be called here so opening files will actually work
    updateUpperedFilenamesMap();
}
