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
		FileIndex();
		~FileIndex();

		/**
		 * @brief Loads a VDF-File and initializes everything
		 */
		bool loadVDF(const std::string& vdf, uint32_t priority = 0);

		/**
		 * @brief Places a file into the index
		 * @return True if the file was new, false otherwise
		 */
		bool addFile(const FileInfo& inf);

		/**
		 * @brief Replaces a file matching the same name
		 * @return True, if the file was actually replaced. False if it was just added because it didn't exist
		 */
		bool replaceFileByName(const FileInfo& inf);

		/**
		 * @brief Fills the given pointer with the information about the provided filename.
		 * @return False, if the file was not found
		 */
		bool getFileByName(const std::string& name, FileInfo* outinf) const;

		/**
		 * @brief Fills a vector with the data of the given file
		 */
		bool getFileData(const FileInfo& inf, std::vector<uint8_t>& data) const;
		bool getFileData(const std::string& file, std::vector<uint8_t>& data) const;

		/**
		 * @brief Clears the complete index and all registered files
		 */
		void clearIndex();

		/**
		 * @brief Returnst the list of all known files
		 */
		const std::vector<FileInfo>& getKnownFiles(){return m_KnownFiles;}

	private:
		/**
		 * @brief Vector of all known files
		 */
		std::vector<FileInfo> m_KnownFiles;

		/**
		 * @brief Map of indices of m_KnownFiles by their filenames 
		 */
		std::unordered_map<std::string, size_t> m_FileIndicesByName;

		/**
		 * @brief all currently loaded virtual archives
		 */
		std::vector<ArchiveVirtual*> m_LoadedVirtualArchives;

		/** 
		 * @brief set of all loaded archives
		 */
		std::set<std::string> m_LoadedArchives;
	};
}