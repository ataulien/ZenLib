#include "archive_virtual.h"
#include "utils/logger.h"
#include "fileIndex.h"
#include <stack>
#include <functional>
#include <string.h>
#include "utils/system.h"
#include <locale>
#include <algorithm>

/**
 * Quick format rundown:
 *
 * Each file starts with a header, described by the type "VdfHeader".
 * For this to be a valid file, each header has to have the version-property set to 0x50.
 * The signature can either be the string in VDF_SIGNATURE_G1 or VDF_SIGNATURE_G2, depending
 * on the game-version this was built with.
 *
 * After the header, the root file catalog can be found, consisting out of Header.NumEntries entries of the type VdfEntryInfo.
 */

using namespace VDFS;

const char* VDF_SIGNATURE_G1 = "PSVDSC_V2.00\r\n\r\n";
const char* VDF_SIGNATURE_G2 = "PSVDSC_V2.00\n\r\n\r";

ArchiveVirtual::ArchiveVirtual() : 
	m_pStream(nullptr),
	m_ArchivePriority(0)
{
}

ArchiveVirtual::~ArchiveVirtual()
{
	if(m_pStream)
		fclose(m_pStream);
}

/**
* @brief Loads the given VDFS-File and initializes the index
*/
bool ArchiveVirtual::loadVDF(const std::string& file, uint32_t priority)
{
	if(m_pStream)
	{
		LogWarn() << "Cannot re-use a virtual archive!";
		return false;
	}

	// Open the archive
	m_pStream = fopen(file.c_str(), "rb");

	if(!m_pStream)
	{
		LogError() << "Failed to open file for reading: " << file;
		return false;
	}

	// Read and verify header
	fread(&m_VdfHeader, sizeof(m_VdfHeader), 1, m_pStream);

	// Verify header version
	if(m_VdfHeader.Version != 0x50)
	{
		LogError() << "VDFS-File '" << file << "' has an invalid header-version of: 0x" << std::hex << m_VdfHeader.Version;
		fclose(m_pStream); m_pStream = nullptr;
		return false;
	}

	// Check for game signature
	if(memcmp(m_VdfHeader.Signature, VDF_SIGNATURE_G1, strlen(VDF_SIGNATURE_G1)) == 0)
		m_ArchiveVersion = AV_Gothic1;
	else if(memcmp(m_VdfHeader.Signature, VDF_SIGNATURE_G2, strlen(VDF_SIGNATURE_G2)) == 0)
		m_ArchiveVersion = AV_Gothic2;
	else
	{
		LogError() << "Unknown VDF-Archive signature on file '" << file << "'";
		fclose(m_pStream); m_pStream = nullptr;
		return false;
	}

	// Assign priority
	// TODO: Use file data as a base, if priority was set to 0!
	m_ArchivePriority = priority;

	// Create list of all files we have here
	updateFileCatalog();

	return true;
}

/**
* @brief Updates the file catalog of this archive
*/
bool ArchiveVirtual::updateFileCatalog()
{
	if(!m_pStream)
	{
		LogError() << "VDF-File not initialized!";
		return false;
	}

	// Skip to file catalog
	fseek(m_pStream, sizeof(VdfHeader), SEEK_SET);

	// Allocate memory for the catalog
	m_EntryCatalog.resize(m_VdfHeader.NumEntries);

	// Read catalog
	if(m_VdfHeader.NumEntries != fread(m_EntryCatalog.data(), sizeof(VdfEntryInfo), m_VdfHeader.NumEntries, m_pStream))
	{
		LogError() << "Failed to read VDFS-Root catalog";
		m_EntryCatalog.clear();
		return false;
	}

	// Fix filenames. They're missing a terminating 0.
	for(size_t total = 0; total < m_EntryCatalog.size(); total++)
	{
		auto& e = m_EntryCatalog[total];

		// Fill the name-buffer with 0 until we reach a non-empty character, as
		// these are not 0 terminated
        // FIXME: This could give trouble with names which are actually 64 chars long!
        e.Name[63] = '\0';
        for(size_t i = 62; i >= 0; i--)
		{
			if(e.Name[i] != ' ')
				break;

			e.Name[i] = 0;
		}

		// Convert name to uppercase
		std::string n = std::string(e.Name);
		std::transform(n.begin(), n.end(), n.begin(), ::toupper);

        strncpy(e.Name, n.c_str(), std::min<size_t>(63u, n.size()));
	}

	// List all folders and files to console
	/*std::function<void(int,int,bool)> f = [&](int idx, int level, bool dirsOnly) {
		auto& e = m_EntryCatalog[idx];

		// List files in directory
		do
		{
			std::string tabs = std::string(level, '|');

			if(!dirsOnly || m_EntryCatalog[idx].Type & VDF_ENTRY_DIR)
				LogInfo() << "Entry: " << tabs << m_EntryCatalog[idx].Name;

			if(m_EntryCatalog[idx].Type & VDF_ENTRY_DIR)
			{
				f(m_EntryCatalog[idx].JumpTo, level + 1, dirsOnly);
				LogInfo() << "Entry: " << tabs << "END " << m_EntryCatalog[idx].Name;
			}
			idx++;
		}while(!(m_EntryCatalog[idx-1].Type & VDF_ENTRY_LAST));
	};

	// List everything
	//f(0, 0, false);

    // Print only directory structure
	f(0, 0, true);
}
	//f(0, 0, true);*/

	//extractArchiveToDisk("test");

	return true;
}

/** 
 * @brief Extracts the vdfs to disc
 */
bool ArchiveVirtual::extractArchiveToDisk(const std::string& baseDirectory)
{
	Utils::System::mkdir(baseDirectory.c_str());

	// List all folders and files to console
	std::function<void(int, const std::string&)> f = [&](int idx, const std::string& path) {
		auto& e = m_EntryCatalog[idx];

		// List files in directory
		do
		{
			LogInfo() << "Entry: " << path << "/" << m_EntryCatalog[idx].Name;

			std::string np = path + "/" + m_EntryCatalog[idx].Name;
			std::string fullPath = baseDirectory + "/" + np;
			if(m_EntryCatalog[idx].Type & VDF_ENTRY_DIR)
			{			
				Utils::System::mkdir(fullPath.c_str());

				f(m_EntryCatalog[idx].JumpTo, np);
				LogInfo() << "Entry: " << fullPath;
			}
			else
			{
				// Get data into memory
				std::vector<uint8_t> data;
				extractFile(idx, data);

				// Write back to disc
				FILE* f = fopen(fullPath.c_str(), "wb");

				if(f)
				{
					// Write data to disk
					if(1 == fwrite(data.data(), sizeof(uint8_t) * data.size(), 1, f))
					{
						fclose(f);
					}else
						LogError() << "Failed to write file: " << fullPath;
				}
				else
					LogError() << "Failed to open file for writing: " << fullPath;
			}

			idx++;
		}while(!(m_EntryCatalog[idx-1].Type & VDF_ENTRY_LAST));
	};

	f(0, "");

	return true;
}

/**
 * @brief Fills a vector with the data of a file
 */
bool ArchiveVirtual::extractFile(size_t idx, std::vector<uint8_t>& fileData)
{ 
	auto& e = m_EntryCatalog[idx];

	// Allocate data for the file
	fileData.resize(e.Size);
	
	// Jump to our file
	fseek(m_pStream, e.JumpTo, SEEK_SET);

	// Read from virtual archive
	if(1 != fread(fileData.data(), e.Size, 1, m_pStream))
	{
		LogError() << "Error while reading VDFS-file " << e.Name;
		return false;
	}

	return true;
}

/**
 * @brief Fills a vector with the data of a file
 */
bool ArchiveVirtual::extractFile(const FileInfo& inf, std::vector<uint8_t>& fileData)
{
	if(inf.targetArchive != this)
	{
		LogWarn() << "Trying to extract file from archive other than the files target archive! Aborting extraction";
		return false;
	}

	// Allocate data for the file
	fileData.resize(inf.fileSize);

	// Jump to our file
	fseek(m_pStream, inf.archiveOffset, SEEK_SET);

	// Read from virtual archive
	if(1 != fread(fileData.data(), inf.fileSize, 1, m_pStream))
	{
		LogError() << "Error while reading VDFS-file " << inf.fileName;
		return false;
	}

	return true;
}

/**
* @brief Puts all files into the index, if the priority is right
*/
size_t ArchiveVirtual::insertFilesIntoIndex(FileIndex& index)
{
	size_t r = 0;

	// Put all files of this into the index
	iterateFiles([&](int idx, const std::string& path){
		auto e = m_EntryCatalog[idx];
		FileInfo f;

		// Enter file properties
		f.archiveOffset = e.JumpTo;
		f.fileName = e.Name;
		f.fileSize = e.Size;
		f.targetArchive = this;
		f.priority = m_ArchivePriority;

		// Add to index
		if(index.addFile(f))
			r++;
	});

	return r;
}
/**
* @brief Lists every file with its path and calls a callback containing the file information
*/
bool ArchiveVirtual::iterateFiles(std::function<void(int, const std::string&)> callback)
{
	// Iterate over all files and build their paths during iteration
	std::function<void(int, const std::string&)> f = [&](int idx, const std::string& path) {
		auto& e = m_EntryCatalog[idx];

		// List files in directory
		do
		{
			std::string np = path + "/" + m_EntryCatalog[idx].Name;

			// Handle directories
			if(m_EntryCatalog[idx].Type & VDF_ENTRY_DIR)
			{			
				// Traverse deeper into the directory structure
				f(m_EntryCatalog[idx].JumpTo, np);		
			}
			else
			{
				// Trigger callback for the file we found
				callback(idx, np);
			}

			idx++;
		}while(!(m_EntryCatalog[idx-1].Type & VDF_ENTRY_LAST));
	};

	f(0, "");

	return true;
}
