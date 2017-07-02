#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

namespace VDFS
{
	class FileIndex
	{
	public:
		FileIndex();
		~FileIndex();

        /*
         * @brief Initializes the VDFS
         */
        void Init(const char *argv0);

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
