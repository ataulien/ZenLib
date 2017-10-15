#include "zCProgMeshProto.h"
#include "zenParser.h"
#include "utils/logger.h"
#include "zTypes.h"
#include <string>
#include "vdfs/fileIndex.h"
#include "zCMaterial.h"
#include <algorithm>

using namespace ZenLoad;

static const uint16_t zCPROGMESH_FILE_VERS_G2 = 0x0905;
static const uint16_t MSID_PROGMESH = 0xB100;
static const uint16_t MSID_PROGMESH_END = 0xB1FF;

struct MeshDataEntry
{
	uint32_t offset;
	uint32_t size;
};

struct MeshOffsetsMain
{
	MeshDataEntry position;
	MeshDataEntry normal;
};

struct MeshOffsetsSubMesh
{
	MeshDataEntry triangleList;
	MeshDataEntry wedgeList;
	MeshDataEntry colorList;
	MeshDataEntry trianglePlaneIndexList;
	MeshDataEntry trianglePlaneList;
	MeshDataEntry wedgeMap;
	MeshDataEntry vertexUpdates;
	MeshDataEntry triangleEdgeList;
	MeshDataEntry edgeList;
	MeshDataEntry edgeScoreList;
};


/**
* @brief Loads the mesh from the given VDF-Archive
*/
zCProgMeshProto::zCProgMeshProto(const std::string& fileName, const VDFS::FileIndex& fileIndex)
{
	std::vector<uint8_t> data;
	fileIndex.getFileData(fileName, data);

    if(data.empty())
        return; // TODO: Throw an exception or something
    
	try
	{
		// Create parser from memory
		// FIXME: There is an internal copy of the data here. Optimize!
		ZenLoad::ZenParser parser(data.data(), data.size());
		
		readObjectData(parser);
	}
	catch(std::exception &e)
	{
		LogError() << e.what();
		return;
	}
}

/**
* @brief Reads the mesh-object from the given binary stream
*/
void zCProgMeshProto::readObjectData(ZenParser& parser)
{
	// Information about a single chunk 
	BinaryChunkInfo chunkInfo;

	bool doneReadingChunks = false;
	while(!doneReadingChunks && parser.getSeek() <= parser.getFileSize())
	{
		// Read chunk header and calculate position of next chunk
		parser.readStructure(chunkInfo);

		size_t chunkEnd = parser.getSeek() + chunkInfo.length;
		switch(chunkInfo.id)
		{
		case MSID_PROGMESH:
			{
				uint16_t version = parser.readBinaryWord();
				/*if(version != zCPROGMESH_FILE_VERS_G2)
				{
					LogWarn() << "Unsupported zCProgMeshProto-Version: " << version;
				}*/

				// Read data-pool
				uint32_t dataSize = parser.readBinaryDWord();
				std::vector<uint8_t> dataPool;
				dataPool.resize(dataSize);
				parser.readBinaryRaw(dataPool.data(), dataSize);

				// Read how many submeshes we got
				uint8_t numSubmeshes = parser.readBinaryByte();

				// Read data offsets for the main position/normal-list
				MeshOffsetsMain mainOffsets; parser.readStructure(mainOffsets);

				// Read offsets to indices data
				std::vector<MeshOffsetsSubMesh> subMeshOffsets;
				subMeshOffsets.resize(numSubmeshes);
				parser.readBinaryRaw(subMeshOffsets.data(), numSubmeshes * sizeof(MeshOffsetsSubMesh));

				// Read materials
				
					// ZenArchive - Header
					// uint32_t - Num materials
					// For each material:
					//  - String - Name
					//  - zCMaterial-Chunk

					ZenParser p2(&parser.getData()[parser.getSeek()], parser.getData().size() - parser.getSeek());
					p2.readHeader();

					// Read every stored material
					for(uint32_t i = 0; i < numSubmeshes; i++)
					{
						p2.readLine(false); // Read unused material name (Stored a second time later)

						// Skip chunk headers - we know these are zCMaterial
						uint32_t chunksize = p2.readBinaryDWord();
						uint16_t version = p2.readBinaryWord();
						uint32_t objectIndex = p2.readBinaryDWord();

						p2.skipSpaces();

						// Skip chunk-header
						std::string name = p2.readLine();
						std::string classname = p2.readLine();

						// Save into vector
						m_Materials.emplace_back(zCMaterial::readObjectData(p2, version));
					}
				

				parser.setSeek(p2.getSeek() + parser.getSeek());

				if(version == zCPROGMESH_FILE_VERS_G2)
				{
					// Read whether we want to have alphatesting
					m_IsUsingAlphaTest = parser.readBinaryByte() != 0;
				}else
				{
					m_IsUsingAlphaTest = false;
				}

				// Read boundingbox
				ZMath::float3 min, max;
				parser.readStructure(min);
				parser.readStructure(max);

				m_BBMin = ZMath::float3(min.x, min.y,min.z);
				m_BBMax = ZMath::float3(max.x, max.y,max.z);

				// Extract data
				m_Vertices.resize(mainOffsets.position.size);
				m_Normals.resize(mainOffsets.normal.size);

				// Copy vertex-data
				memcpy(m_Vertices.data(), &dataPool[mainOffsets.position.offset], sizeof(float) * 3 * mainOffsets.position.size);
				memcpy(m_Normals.data(), &dataPool[mainOffsets.normal.offset], sizeof(float) * 3 * mainOffsets.normal.size);
			
				// Copy submesh-data
				m_SubMeshes.resize(numSubmeshes);
				for(uint32_t i = 0; i < numSubmeshes; i++)
				{
					auto& d = subMeshOffsets[i];

					m_SubMeshes[i].m_Material = m_Materials[i];
					m_SubMeshes[i].m_TriangleList.resize(d.triangleList.size);
					m_SubMeshes[i].m_WedgeList.resize(d.wedgeList.size);
					m_SubMeshes[i].m_ColorList.resize(d.colorList.size);
					m_SubMeshes[i].m_TrianglePlaneIndexList.resize(d.trianglePlaneIndexList.size);
					m_SubMeshes[i].m_TrianglePlaneList.resize(d.trianglePlaneList.size);
					m_SubMeshes[i].m_TriEdgeList.resize(d.triangleEdgeList.size);
					m_SubMeshes[i].m_EdgeList.resize(d.edgeList.size);
					m_SubMeshes[i].m_EdgeScoreList.resize(d.edgeScoreList.size);
					m_SubMeshes[i].m_WedgeMap.resize(d.wedgeMap.size);

					if(!m_SubMeshes[i].m_TriangleList.empty())
						memcpy(m_SubMeshes[i].m_TriangleList.data(), &dataPool[d.triangleList.offset], sizeof(zTriangle) * d.triangleList.size);
					
					if(!m_SubMeshes[i].m_WedgeList.empty())
						memcpy(m_SubMeshes[i].m_WedgeList.data(), &dataPool[d.wedgeList.offset], sizeof(zWedge) * d.wedgeList.size);
					
					if(!m_SubMeshes[i].m_ColorList.empty())
						memcpy(m_SubMeshes[i].m_ColorList.data(), &dataPool[d.colorList.offset], sizeof(float) * d.colorList.size);
					
					if(!m_SubMeshes[i].m_TrianglePlaneIndexList.empty())
						memcpy(m_SubMeshes[i].m_TrianglePlaneIndexList.data(), &dataPool[d.trianglePlaneIndexList.offset], sizeof(uint16_t) * d.trianglePlaneIndexList.size);
					
					if(!m_SubMeshes[i].m_TrianglePlaneList.empty())
						memcpy(m_SubMeshes[i].m_TrianglePlaneList.data(), &dataPool[d.trianglePlaneList.offset], sizeof(zTPlane) * d.trianglePlaneList.size);
					
					if(!m_SubMeshes[i].m_TriEdgeList.empty())
						memcpy(m_SubMeshes[i].m_TriEdgeList.data(), &dataPool[d.triangleEdgeList.offset], sizeof(zTriangleEdges) * d.triangleEdgeList.size);
					
					if(!m_SubMeshes[i].m_EdgeList.empty())
						memcpy(m_SubMeshes[i].m_EdgeList.data(), &dataPool[d.edgeList.offset], sizeof(zEdge) * d.edgeList.size);
					
					if(!m_SubMeshes[i].m_EdgeScoreList.empty())
						memcpy(m_SubMeshes[i].m_EdgeScoreList.data(), &dataPool[d.edgeScoreList.offset], sizeof(float) * d.edgeScoreList.size);
					
					if(!m_SubMeshes[i].m_WedgeMap.empty())
						memcpy(m_SubMeshes[i].m_WedgeMap.data(), &dataPool[d.wedgeMap.offset], sizeof(uint16_t) * d.wedgeMap.size);

				}
			}

			parser.setSeek(chunkEnd);
			break;

		case MSID_PROGMESH_END:
			doneReadingChunks = true;
			break;
		default:
			parser.setSeek(chunkEnd);
		}
	}
}

/**
 * @brief Packs vertices only
 */
void ZenLoad::zCProgMeshProto::packVertices(std::vector<WorldVertex>& vxs, std::vector<uint32_t>& ixs, uint32_t indexStart, std::vector<uint32_t>& submeshIndexStarts, float scale) const
{
	for(size_t s=0;s<m_SubMeshes.size();s++)
	{

		const SubMesh& sm = m_SubMeshes[s];

		unsigned int meshVxStart = vxs.size();

		// Get data
		for(size_t i=0;i<sm.m_WedgeList.size();i++)
		{
			const zWedge& wedge = sm.m_WedgeList[i];

			WorldVertex v;
			v.Position = m_Vertices[wedge.m_VertexIndex] * scale;
			v.Normal = wedge.m_Normal;
			v.TexCoord = wedge.m_Texcoord;
			//v.TexCoord2 = float2(0,0);
			v.Color = 0xFFFFFFFF; // TODO: Apply color from material!
			vxs.push_back(v);
		}

		// Mark when the submesh starts
		submeshIndexStarts.push_back(ixs.size());

		// And get the indices
		for(size_t i=0;i<sm.m_TriangleList.size();i++)
		{
			//for(int j=2;j>=0;j--)
			for(int j=0;j<3;j++)
			{
				ixs.push_back((sm.m_TriangleList[i].m_Wedges[j]) // Take wedge-index of submesh
					+ indexStart // Add our custom offset
					+ meshVxStart); // And add the starting location of the vertices for this submesh
			}
		}		
	}
}

/**
* @brief Creates packed submesh-data
*/
void zCProgMeshProto::packMesh(PackedMesh& mesh, float scale) const 
{
	std::vector<uint32_t> submeshIndexStarts;
	std::vector<uint32_t> indices;
	packVertices(mesh.vertices, indices, 0, submeshIndexStarts, scale);

	// Put in all materials. There could be more than there are submeshes for animated textures or headmeshes
	mesh.subMeshes.resize(std::max(m_Materials.size(), m_SubMeshes.size()));
	for(size_t i=0;i<m_SubMeshes.size();i++)
	{
		auto &sm = mesh.subMeshes[i];
		sm.material = m_SubMeshes[i].m_Material;
	}

	// Create objects for all submeshes
	for(size_t i=0;i<m_SubMeshes.size();i++)
	{
		auto& sm = mesh.subMeshes[i];

		// Get indices
		for(size_t j = submeshIndexStarts[i]; j < submeshIndexStarts[i] + m_SubMeshes[i].m_TriangleList.size() * 3; j++)
		{
			sm.indices.push_back(indices[j]);
		}
	}

	mesh.bbox[0] = m_BBMin * scale;
	mesh.bbox[1] = m_BBMax * scale;
}
