#include "zCMeshSoftSkin.h"
#include <algorithm>
#include <cfloat>
#include <string>
#include "zCProgMeshProto.h"
#include "zTypes.h"
#include "zenParser.h"
#include "utils/alignment.h"
#include "utils/logger.h"
#include "vdfs/fileIndex.h"

namespace ZenLib
{
    using namespace ZenLoad;

    static const uint16_t MSID_MESHSOFTSKIN = 0xE100;
    static const uint16_t MSID_MESHSOFTSKIN_END = 0xE110;

    void oBBox3d::load(ZenParser& parser)
    {
        parser.readBinaryRaw(&center, sizeof(center));

        parser.readBinaryRaw(&axis, sizeof(axis));
        parser.readBinaryRaw(&extends, sizeof(extends));

        uint16_t numChildren = parser.readBinaryWord();

        for (uint16_t i = 0; i < numChildren; i++)
        {
            children.emplace_back();
            children.back().load(parser);
        }
    }

    void oBBox3d::getAABB(ZMath::float3& min, ZMath::float3& max) const
    {
        const float sign[8][3] = {
            -1, -1, -1,
            -1, -1, +1,
            -1, +1, -1,
            -1, +1, +1,
            +1, -1, -1,
            +1, -1, +1,
            +1, +1, -1,
            +1, +1, +1};

        min = {FLT_MAX, FLT_MAX, FLT_MAX};
        max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

        for (int i = 0; i < 8; i++)
        {
            ZMath::float3 point = center;
            const ZMath::float3 axis0 = axis[0] * extends.x * sign[i][0];
            const ZMath::float3 axis1 = axis[1] * extends.y * sign[i][1];
            const ZMath::float3 axis2 = axis[2] * extends.z * sign[i][2];

            point.x += axis0.x + axis1.x + axis2.x;
            point.y += axis0.y + axis1.y + axis2.y;
            point.z += axis0.z + axis1.z + axis2.z;

            min.x = std::min(min.x, point.x);
            min.y = std::min(min.y, point.y);
            min.z = std::min(min.z, point.z);

            max.x = std::max(max.x, point.x);
            max.y = std::max(max.y, point.y);
            max.z = std::max(max.z, point.z);
        }
    }

    /**
* @brief Reads the mesh-object from the given binary stream
*/
    void zCMeshSoftSkin::readObjectData(ZenParser& parser)
    {
        // Information about the whole file we are reading here
        BinaryFileInfo fileInfo;

        // Information about a single chunk
        BinaryChunkInfo chunkInfo;

        // Read chunks until we left the virtual binary file or got to the end-chunk
        // Each chunk starts with a header (BinaryChunkInfo) which gives information
        // about what to do and how long the chunk is
        bool doneReadingChunks = false;
        while (!doneReadingChunks)
        {
            // Read chunk header and calculate position of next chunk
            parser.readStructure(chunkInfo);

            size_t chunkEnd = parser.getSeek() + chunkInfo.length;

            switch (chunkInfo.id)
            {
                case MSID_MESHSOFTSKIN:
                {
                    uint32_t version = parser.readBinaryDWord();

                    m_Mesh.readObjectData(parser);

                    uint32_t vertexWeightStreamSize = parser.readBinaryDWord();

                    m_VertexWeightStream.resize(vertexWeightStreamSize);
                    parser.readBinaryRaw(m_VertexWeightStream.data(), vertexWeightStreamSize);

                    uint32_t numNodeWedgeNormals = parser.readBinaryDWord();
                    std::vector<zTNodeWedgeNormal> nodeWedgeNormals(numNodeWedgeNormals);

                    if (numNodeWedgeNormals > 0)
                        parser.readBinaryRaw(nodeWedgeNormals.data(), numNodeWedgeNormals * sizeof(zTNodeWedgeNormal));

                    uint16_t numNodes = parser.readBinaryWord();
                    std::vector<int32_t> nodeList(numNodes);
                    parser.readBinaryRaw(nodeList.data(), numNodes * sizeof(int32_t));

                    for (uint16_t i = 0; i < numNodes; i++)
                    {
                        m_BBoxesByNodes.emplace_back();
                        m_BBoxesByNodes.back().load(parser);
                    }

                    updateBboxTotal();
                    // Chunksize seems wrong, can't skip here!
                    //parser.setSeek(chunkEnd); // Skip chunk
                }
                break;

                case MSID_MESHSOFTSKIN_END:
                    doneReadingChunks = true;
                    break;

                default:
                    parser.setSeek(chunkEnd);  // Skip chunk
            }
        }
    }

    /**
* @brief Creates packed submesh-data
*/
    void zCMeshSoftSkin::packMesh(PackedSkeletalMesh& mesh, float scale) const
    {
        std::vector<uint32_t> submeshIndexStarts;
        std::vector<uint32_t> indices;
        std::vector<SkeletalVertex> vertices(m_Mesh.getVertices().size());

        // Extract weights and local positions
        const uint8_t* stream = m_VertexWeightStream.data();
        //LogInfo() << "Stream size: " << m_VertexWeightStream.size();

        for (size_t i = 0, end = m_Mesh.getVertices().size(); i < end; i++)
        {
            // Layout:
            //	uint32_t: numWeights
            //	numWeights* zTWeightEntry: weights

            uint32_t numWeights;
            Utils::getUnaligned(&numWeights, stream);
            stream += sizeof(uint32_t);

            for (size_t j = 0; j < numWeights; j++)
            {
                // Note: Using zTWeightEntry here causes a SIGBUS on ARM because of packing!
                float weight;
                Utils::getUnaligned(&weight, stream);
                stream += sizeof(float);

                ZMath::float3 localVertexPosition;
                Utils::getUnaligned(&localVertexPosition.x, stream);
                stream += sizeof(float);
                Utils::getUnaligned(&localVertexPosition.y, stream);
                stream += sizeof(float);
                Utils::getUnaligned(&localVertexPosition.z, stream);
                stream += sizeof(float);

                unsigned char nodeIndex;
                Utils::getUnaligned(&nodeIndex, stream);
                stream += sizeof(uint8_t);

                // Move data to our structure
                vertices[i].BoneIndices[j] = nodeIndex;
                vertices[i].LocalPositions[j] = localVertexPosition * scale;
                vertices[i].Weights[j] = weight;
            }
        }

        for (size_t s = 0; s < m_Mesh.getNumSubmeshes(); s++)
        {
            const zCProgMeshProto::SubMesh& sm = m_Mesh.getSubmesh(s);

            unsigned int meshVxStart = mesh.vertices.size();

            // Get data
            for (size_t i = 0; i < sm.m_WedgeList.size(); i++)
            {
                const zWedge& wedge = sm.m_WedgeList[i];

                SkeletalVertex v = vertices[wedge.m_VertexIndex];

                v.Normal = wedge.m_Normal;
                v.TexCoord = wedge.m_Texcoord;
                v.Color = 0xFFFFFFFF;  // TODO: Apply color from material!
                mesh.vertices.push_back(v);
            }

            // Mark when the submesh starts
            submeshIndexStarts.push_back(indices.size());

            // And get the indices
            for (size_t i = 0; i < sm.m_TriangleList.size(); i++)
            {
                //for(int j=2;j>=0;j--)
                for (int j = 0; j < 3; j++)
                {
                    indices.push_back((sm.m_TriangleList[i].m_Wedges[j])  // Take wedge-index of submesh
                                      + meshVxStart);                     // And add the starting location of the vertices for this submesh
                }
            }
        }

        // Create objects for all submeshes
        for (size_t i = 0; i < m_Mesh.getNumSubmeshes(); i++)
        {
            mesh.subMeshes.emplace_back();
            mesh.subMeshes.back().material = m_Mesh.getSubmesh(i).m_Material;

            // Get indices
            for (size_t j = submeshIndexStarts[i]; j < submeshIndexStarts[i] + m_Mesh.getSubmesh(i).m_TriangleList.size() * 3; j++)
            {
                mesh.subMeshes.back().indices.push_back(indices[j]);
            }
        }

        mesh.bbox[0] = m_BBoxTotal[0] * scale;
        mesh.bbox[1] = m_BBoxTotal[1] * scale;
    }

    void zCMeshSoftSkin::updateBboxTotal()
    {
        m_BBoxTotal[0] = {FLT_MAX, FLT_MAX, FLT_MAX};
        m_BBoxTotal[1] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

        for (const oBBox3d& bb : m_BBoxesByNodes)
        {
            ZMath::float3 min, max;
            bb.getAABB(min, max);

            m_BBoxTotal[0].x = std::min(m_BBoxTotal[0].x, min.x);
            m_BBoxTotal[0].y = std::min(m_BBoxTotal[0].y, min.y);
            m_BBoxTotal[0].z = std::min(m_BBoxTotal[0].z, min.z);

            m_BBoxTotal[1].x = std::max(m_BBoxTotal[1].x, max.x);
            m_BBoxTotal[1].y = std::max(m_BBoxTotal[1].y, max.y);
            m_BBoxTotal[1].z = std::max(m_BBoxTotal[1].z, max.z);
        }
    }

    void zCMeshSoftSkin::getAABBTotal(ZMath::float3& min, ZMath::float3& max) const
    {
        min = m_BBoxTotal[0];
        max = m_BBoxTotal[1];
    }
}  // namespace ZenLib
