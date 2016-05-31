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

		static void readVobTree(ZenParser& parser, std::vector<zCVobData>& target)
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
			zCVobData v = zCVob::readObjectData(parser);

			// Read how many vobs this one has as child
			parser.getImpl()->readEntry("", &numChildren, sizeof(numChildren), ZenLoad::ParserImpl::ZVT_INT);

			// Add to parent
			target.push_back(v);

			// Read children
			target.back().childVobs.reserve(numChildren);
			for(uint32_t i = 0; i < numChildren; i++)
			{
				readVobTree(parser, target.back().childVobs);
			}
			
			
		}

		/**
		* Reads this object from an internal zen
		*/
		static oCWorldData readObjectData(ZenParser& parser)
		{
			oCWorldData info;
			info.objectClass = "oCWorld";

			while(!parser.readChunkEnd())
			{
				ZenParser::ChunkHeader header;
				parser.readChunkStart(header);

				LogInfo() << "oCWorld reading chunk: " << header.name;

				if(header.name == "MeshAndBsp")
				{
					parser.readWorldMesh();
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
						readVobTree(parser, info.rootVobs);
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
