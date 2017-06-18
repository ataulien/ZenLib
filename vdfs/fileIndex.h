#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <set>


#include "archive_virtual.h"

namespace VDFS
{
	// TODO: Repace with general class and make a forward decl!
	class ArchiveVirtual;

	/**
	 * @brief Information about in which archive the file is and on what offset it starts
	 */
	struct FileInfo
	{
		std::string fileName;
		uint32_t fileSize;
		ArchiveVirtual* targetArchive; 
		uint32_t archiveOffset; // 0 for physical files
		uint32_t priority;
	};

	class FileIndex
	{
	public:
		FileIndex(const char *argv0);
		~FileIndex();

		/**
		 * @brief Loads a VDF-File and initializes everything
		 */
		bool loadVDF(const std::string& vdf, uint32_t priority = 0, const std::string& mountPoint = "/");

		/**
		 * @brief Fills a vector with the data of the given file
		 */
		bool getFileData(const std::string& file, std::vector<uint8_t>& data) const;

		/**
		 * @brief Returnst the list of all known files
		 */
        std::vector<std::string> getKnownFiles(const std::string& path = "/") const;

		/**
		 * @return Whether a file with the given name exists
		 */
         bool hasFile(const std::string& name) const;
	};
}
