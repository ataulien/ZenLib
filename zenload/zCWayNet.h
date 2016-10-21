#pragma once
#include "zTypes.h"
#include "zenParser.h"
#include "utils/logger.h"
#include <map>

namespace ZenLoad
{
    class zCWayNet
    {
    public:

        /**
         * Reads a single waypoint
         */
        static zCWaypointData readWaypoint(ZenParser& parser)
        {
            zCWaypointData info;

            parser.getImpl()->readEntry("wpName", &info.wpName);
            parser.getImpl()->readEntry("waterDepth", &info.waterDepth);
            parser.getImpl()->readEntry("underWater", &info.underWater);
            parser.getImpl()->readEntry("position", &info.position);
            parser.getImpl()->readEntry("direction", &info.direction);

            return info;
        }

        /**
         * Reads this object from an internal zen
         */
        static void readObjectData(zCWayNetData& info, ZenParser& parser)
        {
            ZenParser::ChunkHeader waynetHeader;
            parser.readChunkStart(waynetHeader);

            ReadObjectProperties(parser, info.properties,
                                 Prop("waynetVersion", info.waynetVersion));

            if(info.waynetVersion == 0)
            {
                // TODO: Implement old waynet format
                LogWarn() << "Old waynet-format not yet supported!";
                return;
            }

            // First, read the waypoints array
            uint32_t numWaypoints;
            parser.getImpl()->readEntry("numWaypoints", &numWaypoints, sizeof(numWaypoints), ParserImpl::ZVT_INT);

            LogInfo() << "Loading " << numWaypoints << " freepoints";

            std::map<uint32_t, size_t> wpRefMap;
            for(uint32_t i=0;i<numWaypoints;i++)
            {
                ZenParser::ChunkHeader wph;

                // These are always new ones
                parser.readChunkStart(wph);
                zCWaypointData w = readWaypoint(parser);
                w.objectClass = wph.classname;
                info.waypoints.push_back(w);

                parser.readChunkEnd();

                // Save for later access
                wpRefMap[wph.objectID] = info.waypoints.size() - 1;
            }

            // Then, the edges (ways)
            uint32_t numWays;
            parser.getImpl()->readEntry("numWays", &numWays, sizeof(numWays), ParserImpl::ZVT_INT);

            LogInfo() << "Loading " << numWays << " edges";

            for(uint32_t i=0;i<numWays;i++)
            {
                size_t wp1, wp2;

                size_t* tgt = &wp1;
                for(int i=0;i<2;i++)
                {
                    ZenParser::ChunkHeader wph;

                    // References might occur here
                    parser.readChunkStart(wph);

                    // Loading a reference?
                    if (wph.classname.empty())
                    {
                        *tgt = wpRefMap[wph.objectID];
                    } else
                    {
                        // Create new waypoint
                        zCWaypointData w = readWaypoint(parser);
                        w.objectClass = wph.classname;
                        info.waypoints.push_back(w);

                        // Save for later access
                        wpRefMap[wph.objectID] = info.waypoints.size() - 1;
                        *tgt = info.waypoints.size() - 1;
                    }

                    parser.readChunkEnd();

                    tgt = &wp2;
                }
                info.edges.push_back(std::make_pair(wp1, wp2));
            }

			LogInfo() << "Done loading edges!";

            parser.readChunkEnd();
        }

    private:
    };

}
