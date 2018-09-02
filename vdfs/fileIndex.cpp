#include "fileIndex.h"
#include "utils/logger.h"
#include <fstream>
#include <iomanip>
#include <regex>
#include <algorithm>
#include <physfs.h>
#include <assert.h>
#include "../lib/physfs/extras/ignorecase.h"

using namespace VDFS;

FileIndex::FileIndex()
{
}

FileIndex::~FileIndex()
{
    if (PHYSFS_isInit()) PHYSFS_deinit();
}

void FileIndex::initVDFS(const char *argv0)
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
    if (!PHYSFS_mount(path.c_str(), mountPoint.c_str(), 1))
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
    assert(isFinalized());

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
    assert(isFinalized());

    return findCaseSensitiveNameOf(name) != "";
}

int64_t VDFS::FileIndex::getLastModTime(const std::string& name)
{
    int64_t result = -1;
    std::ifstream infile(name);
    if (infile.good())
    {
        std::string firstLine;
        std::getline(infile, firstLine);
        struct std::tm tm;
        std::smatch match;

        // Regex of the datetime used by G1 and G2 e.g. (Thu, 19 Dec 2002 19:24:42 GMT)
        std::regex rgxG1G2("\\(.*, ([[:digit:]]*) (.*) ([[:digit:]]*) ([[:digit:]]*):([[:digit:]]*):([[:digit:]]*) GMT\\)");

        // Regex of the datetime used by G1 e.g. 19.06.2001  18:58.06
        std::regex rgxG1("([[:digit:]]*)\\.([[:digit:]]*)\\.([[:digit:]]*)  ([[:digit:]]*):([[:digit:]]*).([[:digit:]]*)");

        if (std::regex_search(firstLine, match, rgxG1G2))
        {
            std::istringstream datetime(match[1].str() + "-" + match[2].str() + "-"
                + match[3].str() + " " + match[4].str() + ":"
                + match[5].str() + ":" + match[6].str());

            datetime >> std::get_time(&tm, "%d-%b-%Y %H:%M:%S");
            if (!datetime.fail())
                result = std::mktime(&tm);
        }
        else if (std::regex_search(firstLine, match, rgxG1))
        {
            std::istringstream datetime(match[1].str() + "-" + match[2].str() + "-"
                + match[3].str() + " " + match[4].str() + ":"
                + match[5].str() + ":" + match[6].str());

            datetime >> std::get_time(&tm, "%d-%m-%Y %H:%M:%S");
            if (!datetime.fail())
                result = std::mktime(&tm);
        }

        infile.close();
    }

    return result;
}

std::vector<std::string> FileIndex::getKnownFiles(const std::string& path) const
{
    assert(isFinalized());

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
    for (const std::string& file : files)
    {
        std::string uppered = file;
        std::transform(uppered.begin(), uppered.end(), uppered.begin(), ::toupper);

        m_FilenamesByUpperedFileNames[uppered] = file;
    }
}

std::string FileIndex::findCaseSensitiveNameOf(const std::string& caseInsensitiveName) const
{
    assert(isFinalized());

    std::string uppered = caseInsensitiveName;
    std::transform(caseInsensitiveName.begin(), caseInsensitiveName.end(), uppered.begin(), ::toupper);

    auto it = m_FilenamesByUpperedFileNames.find(uppered);

    if (it == m_FilenamesByUpperedFileNames.end())
        return "";

    return it->second;
}

void FileIndex::finalizeLoad()
{
    // Must be called here so opening files will actually work
    updateUpperedFilenamesMap();
}

bool FileIndex::isFinalized() const
{
    return !m_FilenamesByUpperedFileNames.empty();
}
