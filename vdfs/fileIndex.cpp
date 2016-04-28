#include "fileIndex.h"
#include "utils/logger.h"
#include "archive_virtual.h"
#include <locale>
#include <algorithm>

using namespace VDFS;

FileIndex::FileIndex()
{
}

FileIndex::~FileIndex()
{
	for(auto& a : m_LoadedVirtualArchives)
		delete a;

	m_LoadedVirtualArchives.clear();
}

/**
* @brief Loads a VDF-File and initializes everything
*/
bool FileIndex::loadVDF(const std::string& vdf, uint32_t priority)
{
	// Check if this was already loaded
	std::string upper = vdf;
	std::transform(upper.begin(), upper.end(),upper.begin(), ::toupper);
	if(m_LoadedArchives.find(upper) != m_LoadedArchives.end())
		return true; // Already loaded, don't do it again

	ArchiveVirtual* a = new ArchiveVirtual();

	// Load the archive
	if(!a->loadVDF(vdf, priority))
	{
		delete a;
		return false;
	}

	// Grab files and handle load-priority
	a->insertFilesIntoIndex(*this);

	LogInfo() << "Successfully loaded VDF-Archive: " << vdf;

	// Put into list of archives
	m_LoadedVirtualArchives.emplace_back(a);

	// Add to loaded archive set
	m_LoadedArchives.insert(upper);

	return true;
}

/**
* @brief Places a file into the index
* @return True if the file was new, false otherwise
*/
bool FileIndex::addFile(const FileInfo& inf)
{
	// Already exists?
	auto it = m_FileIndicesByName.find(inf.fileName);
	if(it != m_FileIndicesByName.end())
	{
		// Check priority
		if(inf.priority <= m_KnownFiles[(*it).second].priority)
			return false;

		// Overwrite if new priority is greater
		m_KnownFiles[(*it).second] = inf;
		return true;
	}

	// Add to known files and register in the index-map
	m_KnownFiles.push_back(inf);
	m_FileIndicesByName[inf.fileName] = m_KnownFiles.size() - 1;

	return true;
}

/**
* @brief Replaces a file matching the same name
* @return True, if the file was actually replaced. False if it was just added because it didn't exist
*/
bool FileIndex::replaceFileByName(const FileInfo& inf)
{
	// Check if the file even exists first
	auto it = m_FileIndicesByName.find(inf.fileName);
	if(it == m_FileIndicesByName.end())
	{
		// It doesn't, just add it
		addFile(inf);
		return false;
	}

	// It does exist, replace it
	m_KnownFiles[(*it).second] = inf;
	return true;
}

/**
* @brief Fills the given pointer with the information about the provided filename.
* @return False, if the file was not found
*/
bool FileIndex::getFileByName(const std::string& name, FileInfo* outinf) const
{
	// Does the file even exist?
	std::string upper = name;
	std::transform(upper.begin(), upper.end(),upper.begin(), ::toupper);

	auto it = m_FileIndicesByName.find(upper);
	if(it == m_FileIndicesByName.end())
		return false;

	// Output the file information
	if(outinf)*outinf = m_KnownFiles[(*it).second];

	return true;
}

/**
* @brief Clears the complete index and all registered files
*/
void FileIndex::clearIndex()
{
	m_FileIndicesByName.clear();
	m_KnownFiles.clear();
}

/**
* @brief Fills a vector with the data of the given file
*/
bool FileIndex::getFileData(const FileInfo& inf, std::vector<uint8_t>& data) const
{
	return inf.targetArchive->extractFile(inf, data);
}

bool FileIndex::getFileData(const std::string& file, std::vector<uint8_t>& data) const
{
	std::string upper = file;
	std::transform(upper.begin(), upper.end(),upper.begin(), ::toupper);

	FileInfo inf;
	if(getFileByName(upper, &inf))
		return inf.targetArchive->extractFile(inf, data);

	LogError() << "File not found: " << file;

	return false;
}