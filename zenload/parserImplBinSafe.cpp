#include <utils/logger.h>
#include "parserImplBinSafe.h"

using namespace ZenLoad;

ZenLoad::ParserImplBinSafe::ParserImplBinSafe(ZenParser * parser) : ParserImpl(parser)
{
}

/**
* @brief Read the start of a chunk. [...]
*/
bool ParserImplBinSafe::readChunkStart(ZenParser::ChunkHeader& header)
{
	size_t seek = m_pParser->getSeek();
	try{

		EZenValueType type;
		size_t size;

		// Check if this actually will be a string
		readTypeAndSizeBinSafe(type, size);
		m_pParser->setSeek(seek);

		if(type != ZVT_STRING)	
			return false; // Next property isn't a string or the end

		// Read chunk-header
		std::string vobDescriptor = readString();

		// Early exit if this is a chunk-end or not a chunk header
		if(vobDescriptor.empty() || (vobDescriptor.front() != '[' && vobDescriptor.back() != ']') || vobDescriptor.size() <= 2)
		{
			m_pParser->setSeek(seek);
			return false;
		}

		vobDescriptor = vobDescriptor.substr(1, vobDescriptor.size() - 2);

		// Special case for camera keyframes
		if(vobDescriptor.find('%') != std::string::npos && vobDescriptor.find('\xA7') != std::string::npos)
		{
			// Make a header with createObject = true and ref as classname
			auto parts = Utils::split(vobDescriptor, ' ');
			header.createObject = true;
			header.classname = "\xA7";
			header.name = "%";
			header.size = 0;
			header.version = 0;
			header.objectID = std::stoi(parts.back());
			return true;
		}

		// Save chunks starting-position (right after chunk-header)
		header.startPosition = m_pParser->m_Seek;

		// Parse chunk-header
		std::vector<std::string> vec = Utils::split(vobDescriptor, ' ');

		std::string name;
		std::string className;
		int classVersion = 0;
		int objectID = 0;
		bool createObject = false;
		enum State
		{
			S_OBJECT_NAME,
			S_REFERENCE,
			S_CLASS_NAME,
			S_CLASS_VERSION,
			S_OBJECT_ID,
			S_FINISHED
		} state = S_OBJECT_NAME;

		for(auto &arg : vec)
		{
			switch(state)
			{
			case S_OBJECT_NAME:
				if(arg != "%")
				{
					name = arg;
					state = S_REFERENCE;
					break;
				}
			case S_REFERENCE:
				if(arg == "%")
				{
					createObject = true;
					state = S_CLASS_NAME;
					break;
				}
				else if(arg == "\xA7")
				{
					createObject = false;
					state = S_CLASS_NAME;
					break;
				}
				else
					createObject = true;
			case S_CLASS_NAME:
				if(!m_pParser->isNumber(arg))
				{
					className = arg;
					state = S_CLASS_VERSION;
					break;
				}
			case S_CLASS_VERSION:
				classVersion = std::atoi(arg.c_str());
				state = S_OBJECT_ID;
				break;
			case S_OBJECT_ID:
				objectID = std::atoi(arg.c_str());
				state = S_FINISHED;
				break;
			default:
				throw std::runtime_error("Strange parser state");
			}
		}

		if(state != S_FINISHED)
			throw std::runtime_error("Parser did not finish");

		header.classname = className;
		header.createObject = createObject;
		header.name = name;
		header.objectID = objectID;
		header.size = 0; // Doesn't matter in ASCII
		header.version = classVersion;

	}
	catch(std::runtime_error e)
	{
		m_pParser->setSeek(seek);
		return false;
	}

	return true;
}

/**
* @brief Read the end of a chunk. []
*/
bool ParserImplBinSafe::readChunkEnd()
{
	size_t seek = m_pParser->getSeek();
	try{
		EZenValueType type;
		size_t size;

		// Check if this actually will be a string
		readTypeAndSizeBinSafe(type, size);
		m_pParser->setSeek(seek);

		if(type != ZVT_STRING)	
			return false; // Next property isn't a string or the end
		

		std::string l = readString();

		if(l != "[]")
		{
			m_pParser->setSeek(seek);
			return false;
		}
	}
	catch(std::runtime_error e)
	{
		m_pParser->setSeek(seek);
		return false; // Next property isn't a string or the end
	}

	return true;
}

/**
* @brief Read the implementation specific header and stores it in the parsers main-header struct
*/
void ParserImplBinSafe::readImplHeader()
{
	// Bin-Safe archiver has its own version stored again for some reason
	m_pParser->m_Header.binSafeHeader.bsVersion = m_pParser->readBinaryDWord();

	// Read object count
	m_pParser->m_Header.objectCount = m_pParser->readBinaryDWord();

	// Read offset of where to find the global hash-table of all objects
	m_pParser->m_Header.binSafeHeader.bsHashTableOffset = m_pParser->readBinaryDWord();

	// Read hashtable
	size_t s = m_pParser->m_Seek;
	m_pParser->m_Seek = m_pParser->m_Header.binSafeHeader.bsHashTableOffset;

	struct BinSafeEntry
	{
		std::string key;
		uint16_t insertionIndex;
	};

	std::unordered_map<uint32_t, std::vector<BinSafeEntry>> map;
	uint32_t htSize = m_pParser->readBinaryDWord();
	for(uint32_t i = 0; i < htSize; i++)
	{
		uint16_t keyLen = m_pParser->readBinaryWord();
		uint16_t insIdx = m_pParser->readBinaryWord();
		uint32_t hashValue = m_pParser->readBinaryDWord();

		// Read the keys data from the archive
		std::vector<uint8_t> keyData(keyLen);
		m_pParser->readBinaryRaw(keyData.data(), keyLen);

		// Put the source key together with the insertion index into the map
		//std::string key(keyData.begin(), keyData.end());
		//map[hashValue].push_back( {key, insIdx} );
	}

	// Restore old position
	m_pParser->m_Seek = s;
}

/**
* @brief Reads a string
*/
std::string ParserImplBinSafe::readString()
{
	EZenValueType type;
	size_t size;

	// These values start with a small header 
	readTypeAndSizeBinSafe(type, size);

	if(type != ZVT_STRING)
		throw std::runtime_error("Expected string-type");

	std::string str; str.resize(size);
	m_pParser->readBinaryRaw(&str[0], size);

	// Skip potential hash-value at the end of the string
	EZenValueType t = static_cast<EZenValueType>(m_pParser->readBinaryByte());
	if(t != ZVT_HASH)
		m_pParser->m_Seek -= sizeof(uint8_t);
	else
		m_pParser->m_Seek += sizeof(uint32_t);

	return str;
}

/**
* @brief reads the small header in front of datatypes
*/
void ParserImplBinSafe::readTypeAndSizeBinSafe(EZenValueType & type, size_t & size)
{
	type = static_cast<EZenValueType>(m_pParser->readBinaryByte());
	size = 0;

	switch(type)
	{
	case ZVT_VEC3:
		size = sizeof(float) * 3;
		break;

	case ZVT_BYTE:
		size = sizeof(uint8_t);
		break;

	case ZVT_WORD:
		size = sizeof(uint16_t);
		break;

	case ZVT_RAW:
	case ZVT_RAW_FLOAT:
	case ZVT_STRING:
		size = m_pParser->readBinaryWord();
		break;

	case ZVT_INT:
	case ZVT_ENUM:
	case ZVT_BOOL:
	case ZVT_HASH:
	case ZVT_FLOAT:
	case ZVT_COLOR:
		size = sizeof(uint32_t); // FIXME: Bool might be only 8 bytes
		break;

	default:
		throw std::runtime_error(std::string("BinSafe: Datatype not implemented"));
	}
}

/**
* @brief Reads data of the expected type. Throws if the read type is not the same as specified and not 0
*/
void ParserImplBinSafe::readEntry(const std::string& expectedName, void* target, size_t targetSize, EZenValueType expectedType)
{
	EZenValueType type;
	size_t size;

	// Read type and size of the entry
	readTypeAndSizeBinSafe(type, size);

    // These are the same in size
    if(expectedType == ZVT_INT)
    {
        if(type == ZVT_COLOR)
            type = ZVT_INT;

        if(type == ZVT_ENUM && size == sizeof(uint32_t))
            type = ZVT_INT; // Enum seems the be there in both 1 and 4 byte?
    }

	if(expectedType != ZVT_BYTE || type != ZVT_ENUM) // International CSLibs have this
		if(expectedType != ZVT_0 && type != expectedType)
			throw std::runtime_error("Valuetype name does not match expected type. Value:" + expectedName);

	switch(type)
	{
	case ZVT_0: break;
	case ZVT_STRING: 
		{ 
			std::string str; str.resize(size);
			m_pParser->readBinaryRaw(&str[0], size); 
			*reinterpret_cast<std::string*>(target) = str;
		} 
		break;

	case ZVT_HASH:
	case ZVT_INT: *reinterpret_cast<uint32_t*>(target) = m_pParser->readBinaryDWord(); break;
	case ZVT_FLOAT: *reinterpret_cast<float*>(target) = m_pParser->readBinaryFloat(); break;
	case ZVT_BYTE: *reinterpret_cast<uint8_t*>(target) = m_pParser->readBinaryByte(); break;
	case ZVT_WORD: *reinterpret_cast<int16_t*>(target) = m_pParser->readBinaryWord(); break;
	case ZVT_BOOL: *reinterpret_cast<bool*>(target) = m_pParser->readBinaryDWord() != 0; break;
	case ZVT_VEC3:
			reinterpret_cast<ZMath::float3*>(target)->x = m_pParser->readBinaryFloat();
			reinterpret_cast<ZMath::float3*>(target)->y = m_pParser->readBinaryFloat();
			reinterpret_cast<ZMath::float3*>(target)->z = m_pParser->readBinaryFloat();
			break;

	case ZVT_COLOR: 
		reinterpret_cast<uint8_t*>(target)[0] = m_pParser->readBinaryByte(); // FIXME: These are may ordered wrong
		reinterpret_cast<uint8_t*>(target)[1] = m_pParser->readBinaryByte();
		reinterpret_cast<uint8_t*>(target)[2] = m_pParser->readBinaryByte();
		reinterpret_cast<uint8_t*>(target)[3] = m_pParser->readBinaryByte();
		break;

	case ZVT_RAW_FLOAT:
	case ZVT_RAW: 
		m_pParser->readBinaryRaw(target, size);
		break;
	case ZVT_10: break;
	case ZVT_11: break;
	case ZVT_12: break;
	case ZVT_13: break;
	case ZVT_14: break;
	case ZVT_15: break;
	case ZVT_ENUM: *reinterpret_cast<uint8_t*>(target) = m_pParser->readBinaryDWord(); break;
	}

	// Skip potential hash-value at the end of the entry
	EZenValueType t = static_cast<EZenValueType>(m_pParser->readBinaryByte());
	if(t != ZVT_HASH)
		m_pParser->m_Seek -= sizeof(uint8_t);
	else
		m_pParser->m_Seek += sizeof(uint32_t);
}

/**
* @brief Reads the type of a single entry
*/
void ParserImplBinSafe::readEntryType(EZenValueType& outtype, size_t& size)
{
	readTypeAndSizeBinSafe(outtype, size);
}
