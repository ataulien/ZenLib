#pragma once
#include "zTypes.h"
#include "zenParser.h"
#include "zCVob.h"
#include "zCWayNet.h"
#include "utils/logger.h"

namespace ZenLoad
{
	class oCWorld
	{
	public:

		enum class WorldVersionInternal {
			VERSION_G1_08k = 64513,
			VERSION_G26fix = 0
		};

		static size_t readVobTree(ZenParser& parser, std::vector<zCVobData>& target, WorldVersion worldversion)
		{
			uint32_t numChildren;

			ZenParser::ChunkHeader header;
			parser.readChunkStart(header);

			if(header.classname == "\xA7")
			{
				parser.skipChunk();

				// Read how many vobs this one has as child
				parser.getImpl()->readEntry("", &numChildren, sizeof(numChildren), ZenLoad::ParserImpl::ZVT_INT);

				return 0;
			}

			// Read vob data, followed by the count of the children of this vob
			target.emplace_back();
			zCVob::readObjectData(target.back(), parser, worldversion, header);

			// Save classname of this vob
			target.back().objectClass = header.classname;

			// Read how many vobs this one has as child
			parser.getImpl()->readEntry("", &numChildren, sizeof(numChildren), ZenLoad::ParserImpl::ZVT_INT);

			// Add to parent
			

			// Read children
			target.back().childVobs.reserve(numChildren);

			size_t num = 1; // 1 + children
			for(uint32_t i = 0; i < numChildren; i++)
			{
				num += readVobTree(parser, target.back().childVobs, worldversion);
			}
			
			return num;
		}

		/**
		* Reads this object from an internal zen
		*/
		static void readObjectData(oCWorldData& info, ZenParser& parser, uint32_t versionInternal)
		{
			WorldVersion version;
			if(versionInternal == static_cast<uint32_t>(WorldVersionInternal::VERSION_G1_08k))
				version = WorldVersion::VERSION_G1_08k;
			else
				version = WorldVersion::VERSION_G26fix;

			info.objectClass = "oCWorld";

			while(!parser.readChunkEnd())
			{
				ZenParser::ChunkHeader header;
				parser.readChunkStart(header);

				LogInfo() << "oCWorld reading chunk: " << header.name;

				if(header.name == "MeshAndBsp")
				{
					parser.readWorldMesh(info);
					parser.readChunkEnd();
				}
				else if(header.name == "VobTree")
				{
					//parser.skipChunk();
					zCVobData mainVobTree;
					uint32_t numChildren;
                    
					// Read how many vobs this one has as child
					parser.getImpl()->readEntry("", &numChildren, sizeof(numChildren), ZenLoad::ParserImpl::ZVT_INT);
                    
					info.rootVobs.reserve(numChildren);

					// Read children
					info.numVobsTotal = 0;
					for(uint32_t i = 0; i < numChildren; i++)
					{
						info.numVobsTotal += readVobTree(parser, info.rootVobs, version);
					}
					parser.readChunkEnd();
				}else if(header.name == "WayNet")
				{
					zCWayNet::readObjectData(info.waynet, parser);
				}
				else
				{
					parser.skipChunk();
				}
			}
		}

	private:
	};

}
