#pragma once
#include "zenParser.h"
#include "parserImpl.h"

namespace ZenLoad
{
	/**
	* @brief Templated function-calls to read the right type of data 
	*		  and convert it to a string for the given type
	*/
	template<typename T> 
	static void read(ZenParser& p, std::string& outStr, T& outData, const std::string& exName){outStr = "INVALID DATATYPE";}

	template<> 
    inline void read<float>(ZenParser& p, std::string& outStr, float& outData, const std::string& exName)
	{ 
		p.getImpl()->readEntry(exName, &outData, sizeof(float), ParserImpl::ZVT_FLOAT);
		//outStr = std::to_string(outData);
    }

	template<> 
    inline void read<bool>(ZenParser& p, std::string& outStr, bool& outData, const std::string& exName)
	{ 
		p.getImpl()->readEntry(exName, &outData, sizeof(bool), ParserImpl::ZVT_BOOL);
		//outStr = std::to_string(outData ? 1 : 0);
    }

	template<> 
    inline void read<uint32_t>(ZenParser& p, std::string& outStr, uint32_t& outData, const std::string& exName)
	{ 
		p.getImpl()->readEntry(exName, &outData, sizeof(uint32_t), ParserImpl::ZVT_INT);
		//outStr = std::to_string(outData);
    }

	template<> 
    inline void read<int32_t>(ZenParser& p, std::string& outStr, int32_t& outData, const std::string& exName)
	{ 
		p.getImpl()->readEntry(exName, &outData, sizeof(int32_t), ParserImpl::ZVT_INT);
		//outStr = std::to_string(outData);
    }

	template<> 
    inline void read<uint16_t>(ZenParser& p, std::string& outStr, uint16_t& outData, const std::string& exName)
	{ 
		p.getImpl()->readEntry(exName, &outData, sizeof(uint16_t), ParserImpl::ZVT_WORD);
		//outStr = std::to_string(outData);
    }

	template<> 
    inline void read<uint8_t>(ZenParser& p, std::string& outStr, uint8_t& outData, const std::string& exName)
	{ 
		p.getImpl()->readEntry(exName, &outData, sizeof(uint16_t), ParserImpl::ZVT_BYTE);
		//outStr = std::to_string(outData);
    }

	template<> 
    inline void read<ZMath::float2>(ZenParser& p, std::string& outStr, ZMath::float2& outData, const std::string& exName)
	{ 
		p.getImpl()->readEntry(exName, &outData.x, sizeof(float), ParserImpl::ZVT_FLOAT);
		p.getImpl()->readEntry(exName, &outData.y, sizeof(float), ParserImpl::ZVT_FLOAT);
		//outStr = "[" + std::to_string(outData.x) + ", " + std::to_string(outData.y) + "]";
    }

	template<> 
    inline void read<ZMath::float3>(ZenParser& p, std::string& outStr, ZMath::float3& outData, const std::string& exName)
	{ 
		p.getImpl()->readEntry(exName, &outData, sizeof(float) * 3, ParserImpl::ZVT_VEC3);
		/*outStr = "[" + std::to_string(outData.x) 
			+ ", " + std::to_string(outData.y)
			+ ", " + std::to_string(outData.z) + "]";*/
    }

	template<> 
    inline void read<ZMath::float4>(ZenParser& p, std::string& outStr, ZMath::float4& outData, const std::string& exName)
	{ 
		p.getImpl()->readEntry(exName, &outData.x, sizeof(float), ParserImpl::ZVT_FLOAT);
		p.getImpl()->readEntry(exName, &outData.y, sizeof(float), ParserImpl::ZVT_FLOAT);
		p.getImpl()->readEntry(exName, &outData.z, sizeof(float), ParserImpl::ZVT_FLOAT);
		p.getImpl()->readEntry(exName, &outData.w, sizeof(float), ParserImpl::ZVT_FLOAT);
		/*outStr = "[" + std::to_string(outData.x) 
			+ ", " + std::to_string(outData.y)
			+ ", " + std::to_string(outData.z)
			+ ", " + std::to_string(outData.w) + "]";*/
    }

	template<> 
    inline void read<ZMath::Matrix>(ZenParser& p, std::string& outStr, ZMath::Matrix& outData, const std::string& exName)
	{ 
		float m[16];
		p.getImpl()->readEntry(exName, m, sizeof(float) * 16, ParserImpl::ZVT_RAW_FLOAT);
		outData = m;

		/*outStr = "[";
		for(size_t i = 0; i < 16; i++)
		{
			outStr += std::to_string(m[i]);

			// Only add "," when not at the last value
			if(i != 15)
				outStr += ", ";
		}
		outStr += "]";*/
    }

	template<> 
    inline void read<std::string>(ZenParser& p, std::string& outStr, std::string& outData, const std::string& exName)
	{ 
		p.getImpl()->readEntry(exName, &outData, 0, ParserImpl::ZVT_STRING);
		outStr = outData;
    }

	template<typename... T>
	static void ReadObjectProperties(ZenParser& ZenParser, std::unordered_map<std::string, std::string>& rval, std::pair<const char*, T*>... d)
	{
        auto fn = [&](auto pair)
		{
			std::string outStr;

			// Read the given datatype from the file
            read<typename std::remove_pointer<decltype(pair.second)>::type>(ZenParser, outStr, *pair.second, pair.first);

			// Save the read value as string
			//rval[pair.first] = outStr; 
		};
        
        auto x = {(fn(d), 0)...};
        
		//auto values = std::make_tuple(d...);
		//Utils::for_each_in_tuple(values, );
	}

	template<typename S>
	static std::pair<const char*, S*> Prop(const std::string& t, S& s)
	{
		return std::make_pair(t.c_str(), &s);
	}
}
