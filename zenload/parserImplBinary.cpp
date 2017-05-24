#include "parserImplBinary.h"
#include "utils/logger.h"

using namespace ZenLoad;

ParserImplBinary::ParserImplBinary(ZenParser * parser) : ParserImpl(parser)
{
}

/**
 * @brief Read the start of a chunk. [...]
 */
bool ParserImplBinary::readChunkStart(ZenParser::ChunkHeader& header)
{
	// Skip chunk headers - we know these are zCMaterial
	uint32_t chunksize = m_pParser->readBinaryDWord();
	uint16_t version = m_pParser->readBinaryWord();
	uint32_t objectIndex = m_pParser->readBinaryDWord();

	m_pParser->skipSpaces();

	// Skip chunk-header
	std::string name = m_pParser->readLine();
	std::string classname = m_pParser->readLine();

	header.classname = classname;
	header.createObject = true; // TODO: References shouldn't be used in binary zens...
	header.name = name;
	header.objectID = objectIndex;
	header.size = chunksize;
	header.version = version;

	return true;
}

/**
 * @brief Read the end of a chunk. []
 */
bool ParserImplBinary::readChunkEnd()
{
	return true;
}

/**
 * @brief Read the implementation specific header and stores it in the parsers main-header struct
 */
void ParserImplBinary::readImplHeader()
{
	if(!m_pParser->skipString("objects"))
		throw std::runtime_error("Object count missing");

	m_pParser->m_Header.objectCount = m_pParser->readIntASCII();

	if(!m_pParser->skipString("END"))
		throw std::runtime_error("No END in header(2)");

	m_pParser->skipNewLines();
}

/**
 * @brief Reads a string
 */
std::string ParserImplBinary::readString()
{
	std::string ret = m_pParser->readLine();
	return ret;
}

/**
 * @brief Reads data of the expected type. Throws if the read type is not the same as specified and not 0
 */
void ParserImplBinary::readEntry(const std::string& expectedName, void* target, size_t targetSize, EZenValueType expectedType)
{
	// Special case for strings, they're read until 0-bytes
	if(expectedType == ZVT_STRING)
	{
		*reinterpret_cast<std::string*>(target) = readString();
		return;
	}

	size_t size = 0;
	switch(expectedType)
	{
		// 32-bit
		case ZVT_INT:		
		case ZVT_FLOAT:			
		case ZVT_WORD:			
		case ZVT_VEC3:		
		case ZVT_COLOR: size = sizeof(uint32_t); break;

		// Raw
		case ZVT_RAW_FLOAT:
		case ZVT_RAW:  size = targetSize;  break;

		// Byte sized
		case ZVT_BOOL:	
		case ZVT_BYTE:
		case ZVT_ENUM: size = sizeof(uint8_t); break;
	}

	m_pParser->readBinaryRaw(target, size);
}

/**
* @brief Reads the type of a single entry
*/
void ParserImplBinary::readEntryType(EZenValueType& outtype, size_t& size)
{

}
