#pragma once
#include <string>
#include <vector>
#include <functional>

namespace VDFS
{
	struct FileInfo;

	// These files are written on 32-bit, no packing
#pragma pack(push, 1)

	// Some of these are gracefully taken from the GothicFix-Teams VDFS implementation.

	// File attributes
	const uint32_t VDF_ENTRY_DIR = 0x80000000; // Directory
	const uint32_t VDF_ENTRY_LAST = 0x40000000; // Last file in the archive seems to have this set

	/**
	* @brief Timestamp-bitfield for vdfs-files
	*/
	struct VdfTime  
	{
		uint32_t Seconds : 5;
		uint32_t Minutes : 6;   
		uint32_t Hour	 : 5;  
		uint32_t Day	 : 5;   
		uint32_t Month	 : 4;   
		uint32_t Year	 : 7;
	};

	/**
	* @brief VDFS-File header
	*/
	struct VdfHeader
	{
		char Comment[256];
		char Signature[16];
		uint32_t NumEntries;
		uint32_t NumFiles;
		VdfTime Timestamp;
		uint32_t DataSize;
		uint32_t RootCatOffset;
		uint32_t Version; // 0x50
	};

	/**
	* @brief single file entry information
	*/
	struct VdfEntryInfo
	{
		char Name[64];
		uint32_t JumpTo; // Dirs = child entry's number, Files = data offset
		uint32_t Size;
		uint32_t Type; // = 0x00000000 for files or VDFS_ENTRY_DIR, may be bitmasked by VDFS_ENTRY_LAST
		uint32_t Attributes; // Always 20 = A;
	};
#pragma pack(pop)

	class FileIndex;
	class ArchiveVirtual
	{
	public:
		ArchiveVirtual();
		~ArchiveVirtual();

		/**
		 * @brief Loads the given VDFS-File and initializes the index
		 */
		bool loadVDF(const std::string& file, uint32_t priority = 0);

		/**
		 * @brief Updates the file catalog of this archive.
		 *		  Note: Called internally by loadVDF().	
		 */
		bool updateFileCatalog();

		/**
		 * @brief Puts all files into the index, if the priority is right
		 * @return number of files actually added to the index
		 */
		size_t insertFilesIntoIndex(FileIndex& index);

		/** 
		 * @brief Extracts the vdfs to disc
		 */
		bool extractArchiveToDisk(const std::string& baseDirectory);

		/**
		 * @brief Fills a vector with the data of a file
		 */
		bool extractFile(size_t idx, std::vector<uint8_t>& fileData);
		bool extractFile(const FileInfo& inf, std::vector<uint8_t>& fileData);

	protected:

		/**
		 * @brief Lists every file with its path and calls a callback containing the file information
		 */
		bool iterateFiles(std::function<void(int,const std::string&)> callback);

		/**
		 * @brief File-Stream for this archive
		 */
		FILE* m_pStream;

		/**
		 * @brief Game-Version this is from
		 */
		enum
		{
			AV_Gothic1,
			AV_Gothic2
		} m_ArchiveVersion;

		/**
		 * @brief Header of the loaded VDF-Archive
		 */
		VdfHeader m_VdfHeader;

		/**
		 * @brief File catalog of files of this archive
		 */
		std::vector<VdfEntryInfo> m_EntryCatalog;

		/**
		 * @brief Priority for files of this archive
		 */
		uint32_t m_ArchivePriority;
	};

	// TODO: Make an archive-type for physical as well. Then use virtual functions to simplyfy access
	typedef ArchiveVirtual Archive;
}