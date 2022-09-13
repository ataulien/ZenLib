#pragma once
#include "parserImpl.h"

namespace ZenLib
{
    namespace ZenLoad
    {
        class ParserImplBinSafe : public ParserImpl
        {
            friend ZenParser;

        public:
            ParserImplBinSafe(ZenParser* parser);

            /**
		* @brief Read the implementation specific header and stores it in the parsers main-header struct
		*/
            void readImplHeader() override;

            /**
		* @brief Read the start of a chunk. [...] Returns true if there actually was a start. 
		*		  Otherwise it will leave m_Seek untouched and return false.
		*/
            bool readChunkStart(ZenParser::ChunkHeader& header) override;

            /**
		 * @brief Reads the end of a chunk. Returns true if there actually was an end. 
		 *		  Otherwise it will leave m_Seek untouched and return false.
		 */
            bool readChunkEnd() override;

            /**
		* @brief Reads a string
		*/
            std::string readString() override;

            /**
		* @brief Reads data of the expected type. Throws if the read type is not the same as specified and not 0
		*/
            void readEntry(const char* name, void* target, size_t targetSize, EZenValueType expectedType = ZVT_0) override;

            /**
		* @brief Reads the type of a single entry
		*/
            void readEntryType(EZenValueType& type, size_t& size) override;

        private:
            /**
		 * @brief reads the small header in front of datatypes
		 */
            void readTypeAndSizeBinSafe(EZenValueType& type, size_t& size);
        };
    }  // namespace ZenLoad
}  // namespace ZenLib
