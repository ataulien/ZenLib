#pragma once

#include <functional>
#include "zTypes.h"
#include "zenParser.h"
#include "zCMesh.h"
#include <assert.h>

namespace ZenLoad
{
    class ZenParser;
    class zCBspTree
    {
    public:

        enum EVersion
        {
            Gothic_26f = 0, // TODO
            Gothic_18k = 34144256
        };

        enum EBspChunk
        {
            CHUNK_BSP = 0xC000,
            CHUNK_BSP_POLYLIST = 0xC010,
            CHUNK_BSP_TREE = 0xC040,
            CHUNK_BSP_LEAF_LIGHT = 0xC045,
            CHUNK_BSP_OUTDOOR_SECTORS = 0xC050,
            CHUNK_BSP_END = 0xC0FF,
        };

        /**
        * Reads this object from an internal zen
        */
        static zCBspTreeData readObjectData(ZenParser& parser, zCMesh* mesh)
        {
            zCBspTreeData info;

            // Information about the whole file we are reading here
            BinaryFileInfo fileInfo;
            uint32_t version = 0;
            size_t binFileEnd; // Ending location of the binary file

            // Read information about the current file. Mainly size is important here.
            parser.readStructure(fileInfo);

            // Calculate ending location and thus, the filesize
            binFileEnd = parser.getSeek() + fileInfo.size;

            // Read the BSP-Tree first
            size_t meshPosition = parser.getSeek();
            zCMesh::skip(parser);
            //mesh->readObjectData(parser);

            // Information about a single chunk
            BinaryChunkInfo chunkInfo;

            bool done = false;
            while(!done && parser.getSeek() < binFileEnd)
            {
                // Read chunk header and calculate position of next chunk
                parser.readStructure(chunkInfo);

                size_t chunkEnd = parser.getSeek() + chunkInfo.length;

                switch(chunkInfo.id)
                {
                    case EBspChunk::CHUNK_BSP:
                        version = parser.readBinaryWord();
                        info.mode = static_cast<zCBspTreeData::TreeMode >(parser.readBinaryDWord());
                        break;

                    case CHUNK_BSP_POLYLIST:
                    {
                        uint32_t numPolys = parser.readBinaryDWord();
                        info.treePolyIndices.resize(numPolys);

                        LogInfo() << "numPolys: " << numPolys;

                        // Convert these to size_t
                        for(volatile uint32_t i=0;i<numPolys;i++)
                            info.treePolyIndices[i] = static_cast<size_t>(parser.readBinaryDWord());

                    }
                        break;

                    case CHUNK_BSP_TREE:
                    {
                        uint32_t numNodes = parser.readBinaryDWord();
                        uint32_t numLeafs = parser.readBinaryDWord();
                        //info.nodes.resize(numNodes + numLeafs);
                        //info.leafIndices.reserve(numLeafs);

                        LogInfo() << "Num nodes: " << numNodes;

                        if(!numNodes)
                        {
                            parser.setSeek(chunkEnd); // Skip chunk
                            break;
                        }

                        info.nodes.reserve(numNodes + numLeafs);
                        info.nodes.emplace_back();
                        info.nodes[0].parent = zCBspNode::INVALID_NODE;

                        std::function<void(bool)> loadRec = [&](bool isNode)
                        {
                            size_t idx = info.nodes.size() - 1;
                            //LogInfo() << " - Reading node " << idx;

                            zCBspNode& n = info.nodes[idx];

                            parser.readStructure(n.bbox3dMin);
                            parser.readStructure(n.bbox3dMax);

                            n.bbox3dMin *= 0.01f; // Convert to meters
                            n.bbox3dMax *= 0.01f;

                            // Read indices to the polys this contains
                            n.treePolyIndex = static_cast<size_t>(parser.readBinaryDWord());
                            n.numPolys = static_cast<size_t>(parser.readBinaryDWord());

                            n.front = zCBspNode::INVALID_NODE;
                            n.back = zCBspNode::INVALID_NODE;
                            n.parent = zCBspNode::INVALID_NODE;

                            // Only need to load data if this isn't a leaf
                            if(isNode)
                            {
                                /**
                                 * Flags:
                                 * 1: front
                                 * 2: back
                                 * 4: front is leaf
                                 * 8: back is leaf
                                 */
                                const int FLAG_FRONT = 1;
                                const int FLAG_BACK = 2;
                                const int FLAG_FRONT_IS_LEAF = 4;
                                const int FLAG_BACK_IS_LEAF = 8;

                                // flags tell if this node got children and whether they are leafs
                                uint8_t flags = parser.readBinaryByte();

                                parser.readStructure(n.plane.w);
                                parser.readStructure(n.plane.x);
                                parser.readStructure(n.plane.y);
                                parser.readStructure(n.plane.z);

                                n.plane.w *= 0.01f; // Convert to meters

                                // G1 has an extra byte here
                                if(fileInfo.version == Gothic_18k)
                                    parser.readBinaryByte(); // Lod-flag

                                // Read front node
                                if((flags & FLAG_FRONT) != 0)
                                {
                                    // Assign index and add actual node
                                    n.front = info.nodes.size();
                                    info.nodes.emplace_back();

                                    zCBspNode& front = info.nodes[n.front];

                                    // Set new nodes parent
                                    front.parent = idx;

                                    // If this is a leaf, add it to the leaf-list
                                    if((flags & FLAG_FRONT_IS_LEAF) != 0)
                                        info.leafIndices.push_back(n.front);

                                    // Continue to load the tree
                                    loadRec((flags & FLAG_FRONT_IS_LEAF) == 0);
                                }

                                // Read back node
                                if((flags & FLAG_BACK) != 0)
                                {
                                    // Assign index and add actual node
                                    n.back = info.nodes.size();
                                    info.nodes.emplace_back();

                                    zCBspNode& back = info.nodes[n.back];

                                    // Set new nodes parent
                                    back.parent = idx;

                                    // If this is a leaf, add it to the leaf-list
                                    if((flags & FLAG_BACK_IS_LEAF) != 0)
                                        info.leafIndices.push_back(n.back);

                                    // Continue to load the tree
                                    loadRec((flags & FLAG_BACK_IS_LEAF) == 0);
                                }
                            } else{
                                //LogInfo() << idx << " Leaf!";
                            }

                        };

                        loadRec(true);
                    }
                        break;

                    case CHUNK_BSP_LEAF_LIGHT:
                        parser.setSeek(chunkEnd); // Skip chunk
                        break;

                    case CHUNK_BSP_OUTDOOR_SECTORS:
                        parser.setSeek(chunkEnd); // Skip chunk
                        break;

                    case CHUNK_BSP_END:
                        done = true;
                        break;

                    default:
                        parser.setSeek(chunkEnd); // Skip chunk
                }
            }

            // Now get the list of non-lod polygons to load the worldmesh without them
            std::vector<size_t> nonLodPolys = getNonLodPolygons(info);

            std::sort(nonLodPolys.begin(), nonLodPolys.end());
            nonLodPolys.erase( std::unique( nonLodPolys.begin(), nonLodPolys.end() ), nonLodPolys.end() );

            LogInfo() << "Found " << nonLodPolys.size() << " non lod polys";

            // Reset to mesh position
            size_t seek = parser.getSeek();
            parser.setSeek(meshPosition);
            mesh->readObjectData(parser, nonLodPolys);


            parser.setSeek(binFileEnd);

            return info;
        }

        /**
         * Returns a list of polygon-indices which are not LOD-polyons
         * @param d Loaded BSP-Tree data
         * @return List of indices to polygons, which are not LOD-Polygons. These are the
         *         actual indices of the polygons inside the mesh.
         */
        static std::vector<size_t> getNonLodPolygons(const zCBspTreeData& d)
        {
            std::vector<size_t> r;

            for(size_t nidx : d.leafIndices)
            {
                const zCBspNode& n = d.nodes[nidx];

                for(size_t i=n.treePolyIndex; i<n.treePolyIndex + n.numPolys; i++)
                    r.push_back(d.treePolyIndices[i]);
            }



            return r;
        }

    private:
    };
}
