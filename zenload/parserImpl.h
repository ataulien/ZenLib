#pragma once
#include "zenParser.h"

namespace ZenLoad
{
	class ParserImpl
	{
	public:

		/**
		* @brief Type of data in front of the chunks of the binSave-format
		*/
		enum EZenValueType
		{
			ZVT_0 = 0,
			ZVT_STRING = 0x1,
			ZVT_INT = 0x2,
			ZVT_FLOAT = 0x3,
			ZVT_BYTE = 0x4,
			ZVT_WORD = 0x5,
			ZVT_BOOL = 0x6,
			ZVT_VEC3 = 0x7,
			ZVT_COLOR = 0x8,
			ZVT_RAW = 0x9,
			ZVT_HASH = 0x12,
			ZVT_10,
			ZVT_11,
			ZVT_12,
			ZVT_13,
			ZVT_14,
			ZVT_15,
			ZVT_RAW_FLOAT = 0x10,
			ZVT_ENUM = 0x11,
		};

		ParserImpl(ZenParser* parser);
		virtual ~ParserImpl(){}

		/**
		 * @brief Read the implementation specific header and stores it in the parsers main-header struct
		 */
		virtual void readImplHeader() = 0;

		/**
		 * @brief Read the start of a chunk. [...] Returns true if there actually was a start. 
		 *		  Otherwise it will leave m_Seek untouched and return false.
		 */
		virtual bool readChunkStart(ZenParser::ChunkHeader& header)=0;

		/**
		 * @brief Reads the end of a chunk. Returns true if there actually was an end. 
		 *		  Otherwise it will leave m_Seek untouched and return false.
		 */
		virtual bool readChunkEnd()=0;

		/**
		 * @brief Reads a string
		 */
		virtual std::string readString()=0;

		/**
		 * @brief Reads data of the expected type. Throws if the read type is not the same as specified and not 0
		 */
		virtual void readEntry(const std::string& name, void* target, size_t targetSize, EZenValueType expectedType = ZVT_0) = 0;

		void readEntry(const std::string& name, std::string* target){ readEntry(name, target, 0, ZVT_STRING); }
		void readEntry(const std::string& name, int32_t* target){ readEntry(name, target, sizeof(*target), ZVT_INT); }
		void readEntry(const std::string& name, uint8_t* target){ readEntry(name, target, sizeof(*target), ZVT_BYTE); }
		void readEntry(const std::string& name, bool* target){ readEntry(name, target, sizeof(*target), ZVT_BOOL); }
		void readEntry(const std::string& name, uint32_t* target){ readEntry(name, target, sizeof(*target), ZVT_INT); }
		void readEntry(const std::string& name, float* target){ readEntry(name, target, sizeof(*target), ZVT_FLOAT); }
		void readEntry(const std::string& name, ZMath::float3* target){ readEntry(name, target, sizeof(*target), ZVT_VEC3); }
		void readEntry(const std::string& name, ZMath::Matrix* target){ readEntry(name, target, sizeof(*target), ZVT_RAW_FLOAT); }


		/**
		 * @brief Reads the type of a single entry
		 */
		virtual void readEntryType(EZenValueType& type, size_t& size) = 0;

	protected:

		/**
		 * @brief Parser-Object this operates on
		 */
		ZenParser* m_pParser;
	};
}
