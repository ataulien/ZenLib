#pragma once
#include "parserImpl.h"

namespace ZenLoad
{
	class ParserImplBinary : public ParserImpl
	{
		friend ZenParser;
	public:
		ParserImplBinary(ZenParser* parser);

		/**
		 * @brief Read the implementation specific header and stores it in the parsers main-header struct
		 */
		virtual void readImplHeader();

		/**
		* @brief Read the start of a chunk. [...] Returns true if there actually was a start. 
		*		  Otherwise it will leave m_Seek untouched and return false.
		*/
		virtual bool readChunkStart(ZenParser::ChunkHeader& header);

		/**
		* @brief Reads the end of a chunk. Returns true if there actually was an end. 
		*		  Otherwise it will leave m_Seek untouched and return false.
		*/
		virtual bool readChunkEnd();

		/**
		 * @brief Reads a string
		 */
		virtual std::string readString();

		/**
		 * @brief Reads data of the expected type. Throws if the read type is not the same as specified and not 0
		 */
		virtual void readEntry(const std::string& name, void* target, size_t targetSize, EZenValueType expectedType = ZVT_0);

		/**
		* @brief Reads the type of a single entry
		*/
		virtual void readEntryType(EZenValueType& type, size_t& size);
	};
}
