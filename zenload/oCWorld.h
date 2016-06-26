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

		static void readVobTree(ZenParser& parser, std::vector<zCVobData>& target, WorldVersion worldversion)
		{
			uint32_t numChildren;

			ZenParser::ChunkHeader header;
			parser.readChunkStart(header);

			if(header.classname == "\xA7")
			{
				parser.skipChunk();

				// Read how many vobs this one has as child
				parser.getImpl()->readEntry("", &numChildren, sizeof(numChildren), ZenLoad::ParserImpl::ZVT_INT);

				return;
			}

			static int i=0;
			i++;

			// Read vob data, followed by the count of the children of this vob
			zCVobData v = zCVob::readObjectData(parser, worldversion);

			// Read how many vobs this one has as child
			parser.getImpl()->readEntry("", &numChildren, sizeof(numChildren), ZenLoad::ParserImpl::ZVT_INT);

			// Add to parent
			target.push_back(v);

			// Read children
			target.back().childVobs.reserve(numChildren);
			for(uint32_t i = 0; i < numChildren; i++)
			{
				readVobTree(parser, target.back().childVobs, worldversion);
			}
			
			
		}

		/**
		* Reads this object from an internal zen
		*/
		static oCWorldData readObjectData(ZenParser& parser, uint32_t versionInternal)
		{
			WorldVersion version;
			if(versionInternal == static_cast<uint32_t>(WorldVersionInternal::VERSION_G1_08k))
				version = WorldVersion::VERSION_G1_08k;
			else
				version = WorldVersion::VERSION_G26fix;

			oCWorldData info;
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
					for(uint32_t i = 0; i < numChildren; i++)
					{
						readVobTree(parser, info.rootVobs, version);
					}
					parser.readChunkEnd();
				}else if(header.name == "WayNet")
				{
					info.waynet = zCWayNet::readObjectData(parser);
				}
				else
				{
					parser.skipChunk();
				}
			}

			return info;
		}

	private:
	};

}
