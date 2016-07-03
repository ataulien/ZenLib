#include "zCMesh.h"
#include "zenParser.h"
#include "utils/logger.h"
#include "zTypes.h"
#include <string>
#include "vdfs/fileIndex.h"
#include "zCMaterial.h"
#include <map>

using namespace ZenLoad;

// Types of chunks we will find in a zCMesh-Section
static const unsigned short	MSID_MESH = 0xB000;
static const unsigned short	MSID_BBOX3D = 0xB010;
static const unsigned short	MSID_MATLIST = 0xB020;
static const unsigned short	MSID_LIGHTMAPLIST = 0xB025;
static const unsigned short	MSID_LIGHTMAPLIST_SHARED = 0xB026;
static const unsigned short	MSID_VERTLIST = 0xB030;
static const unsigned short	MSID_FEATLIST = 0xB040;
static const unsigned short	MSID_POLYLIST = 0xB050;
static const unsigned short	MSID_MESH_END = 0xB060;

enum EVersion
{
	G1_1_08k = 9,
	G2_2_6fix = 265
};

/**
* @brief Loads the mesh from the given VDF-Archive
*/
zCMesh::zCMesh(const std::string& fileName, VDFS::FileIndex& fileIndex)
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
		
		// .MSH-Files are just saved zCMeshes
		readObjectData(parser);
	}
	catch(std::exception &e)
	{
		LogError() << e.what();
		return;
	}
}

/**
 * Helper structs for version independend loading of polygon data
 */
#pragma pack(push, 1)
// FIXME: This is going to give some unaligned access issues on ARM
template<typename FT>
struct polyData1
{
    int16_t	materialIndex; // -1 if none
    int16_t	lightmapIndex; // -1 if none
    zTPlane		polyPlane;
    FT          flags;
    uint8_t		polyNumVertices;
};

template<typename IT, typename FT>
struct polyData2 : public polyData1<FT>
{
    /**
     * @brief ugly helper-constructor to get from the specific G1/G2-format to a generic one
     */
    template<typename _IT, typename _FT>
    void from(const polyData2<_IT,_FT>& src)
    {
        for(size_t i=0;i<src.polyNumVertices;i++)
        {
            indices[i].VertexIndex = static_cast<IT>(src.indices[i].VertexIndex);
            indices[i].FeatIndex = src.indices[i].FeatIndex;
        }

        polyData1<FT>::materialIndex = src.materialIndex;
        polyData1<FT>::lightmapIndex = src.lightmapIndex;
        polyData1<FT>::polyPlane = src.polyPlane;
        polyData1<FT>::polyNumVertices = src.polyNumVertices;
        polyData1<FT>::flags = src.flags.generify();
    }

    polyData2(){}

    struct Index
    {
        IT VertexIndex;
        uint32_t FeatIndex;
    };


    Index indices[255];
};
#pragma pack(pop)
/**
* @brief Reads the mesh-object from the given binary stream
*/
void zCMesh::readObjectData(ZenParser& parser, const std::vector<size_t>& skipPolys, bool forceG132bitIndices)
{
    uint16_t version;

    // Information about a single chunk
    BinaryChunkInfo chunkInfo;

	// Read chunks until we left the virtual binary file or got to the end-chunk
	// Each chunk starts with a header (BinaryChunkInfo) which gives information
	// about what to do and how long the chunk is
	bool doneReadingChunks = false;
	while(!doneReadingChunks)
	{
		// Read chunk header and calculate position of next chunk
		parser.readStructure(chunkInfo);

		size_t chunkEnd = parser.getSeek() + chunkInfo.length;

		switch(chunkInfo.id)
		{
		case MSID_MESH:
		{
			// uint32 - version
			// zDate - Structure
			// \n terminated string for the name
			version = parser.readBinaryWord();
			zDate date; parser.readStructure(date);
			std::string name = parser.readLine(false);
			(void)date;

			LogInfo() << "Reading mesh '" << name << "' (Version: " << version << ")";

			parser.setSeek(chunkEnd); // Skip chunk
		}
		break;

		case MSID_BBOX3D:
		{
			ZMath::float4 min, max;
			parser.readStructure(min);
			parser.readStructure(max);

			m_BBMin = ZMath::float3(min.x, min.y,min.z);
			m_BBMax = ZMath::float3(max.x, max.y,max.z);

			parser.setSeek(chunkEnd); // Skip chunk
		}
			break;

		case MSID_MATLIST:
			{
				// ZenArchive - Header
				// uint32_t - Num materials
				// For each material:
				//  - String - Name
				//  - zCMaterial-Chunk

				ZenParser p2(&parser.getData()[parser.getSeek()], parser.getData().size() - parser.getSeek());
				p2.readHeader();

				// Read number of materials
				uint32_t numMaterials = p2.readBinaryDWord();

				// Read every stored material
				for(uint32_t i = 0; i < numMaterials; i++)
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

				// Note: There is a bool stored here in the G2-Formats, which says whether to use alphatesting or not
				// 		 We just skip this for now

				parser.setSeek(chunkEnd); // Skip chunk
			}
			break;

		case MSID_LIGHTMAPLIST:
			parser.setSeek(chunkEnd); // Skip chunk
			break;

		case MSID_LIGHTMAPLIST_SHARED:
			parser.setSeek(chunkEnd); // Skip chunk
			break;

		case MSID_VERTLIST:
			{
				// uint32 - number of vertices
				// numVx float3s of vertexpositions

				// Read how many vertices we have in this chunk
				uint32_t numVertices = parser.readBinaryDWord();
				m_Vertices.clear();

				// Read vertex data and emplace into m_Vertices
				m_Vertices.resize(numVertices);
				parser.readBinaryRaw(m_Vertices.data(), numVertices * sizeof(float) * 3);

                LogInfo() << "Found " << numVertices << " vertices";
				// Flip x-coord to make up for right handedness
				//for(auto& v : m_Vertices)
				//	v.x = -v.x;
				
			}
			break;

		case MSID_FEATLIST:
			{
				// uint32 - number of features
				// zTMSH_FeatureChunk*num - features

				// Read how many feats we have
				uint32_t numFeats = parser.readBinaryDWord();

				// Read features
				m_Features.resize(numFeats);
				parser.readBinaryRaw(m_Features.data(), numFeats * sizeof(zTMSH_FeatureChunk));
			}
			break;

		case MSID_POLYLIST:
			{
                // Current entry of the skip list. This is increased as we go.
                size_t skipListEntry = 0;

				// Read number of polys
				int numPolys = parser.readBinaryDWord();

				// Read block of data
				std::vector<uint8_t> dataBlock;
				dataBlock.resize(chunkInfo.length);
				parser.readBinaryRaw(dataBlock.data(), chunkInfo.length);

				uint8_t* blockPtr = dataBlock.data();
                size_t blockSize = version == EVersion::G2_2_6fix
                                   ? sizeof(polyData1<PolyFlags2_6fix>)
                                   : sizeof(polyData1<PolyFlags1_08k>);

                size_t indicesSize = version == EVersion::G2_2_6fix
                                     ? sizeof(polyData2<uint32_t, PolyFlags2_6fix>::Index)
                                     : sizeof(polyData2<uint16_t, PolyFlags1_08k>::Index);

                if(forceG132bitIndices)
                {
                    indicesSize = sizeof(polyData2<uint32_t, PolyFlags1_08k>::Index);
                }

				// Iterate throuh every poly
				for(int i = 0; i < numPolys; i++)
				{
					polyData2<uint32_t, PolyFlags2_6fix>* p26 = (polyData2<uint32_t, PolyFlags2_6fix> *)blockPtr;
                    polyData2<uint16_t, PolyFlags1_08k>* p18k = (polyData2<uint16_t, PolyFlags1_08k> *)blockPtr;
                    polyData2<uint32_t, PolyFlags1_08k>* p18k_32 = (polyData2<uint32_t, PolyFlags1_08k> *)blockPtr;

                    // Convert to a generic version
                    polyData2<uint32_t, PolyFlags> p;
                    if(version == EVersion::G2_2_6fix) {
                        p.from(*p26);
                    }
                    else {
                        if(forceG132bitIndices)
                            p.from(*p18k_32);
                        else
                            p.from(*p18k);
                    }

                    if(skipPolys.empty() || skipPolys[skipListEntry] == i)
                    {
                        // TODO: Store these somewhere else
                        // TODO: lodFlag isn't set to something useful in Gothic 1. Also the portal-flags aren't set? Investigate!
                        if (!p.flags.ghostOccluder && !p.flags.portalPoly && !p.flags.ghostOccluder &&
                            !p.flags.portalIndoorOutdoor)
                        {
                            if (p.polyNumVertices != 0)
                            {

                                if (p.polyNumVertices == 3)
                                {

                                    // Write indices directly to a vector
                                    WorldVertex vx[3];
                                    for (int v = 0; v < 3; v++)
                                    {
                                        m_Indices.emplace_back(p.indices[v].VertexIndex);
                                        m_FeatureIndices.emplace_back(p.indices[v].FeatIndex);

                                        // Gather vertex information
                                        vx[v].Position = m_Vertices[p.indices[v].VertexIndex];
                                        vx[v].Color = m_Features[p.indices[v].FeatIndex].lightStat;

                                        vx[v].TexCoord = ZMath::float2(m_Features[p.indices[v].FeatIndex].uv[0],
                                                                       m_Features[p.indices[v].FeatIndex].uv[1]);
                                        vx[v].Normal = m_Features[p.indices[v].FeatIndex].vertNormal;
                                    }

                                    // Save material index for the written triangle
                                    m_TriangleMaterialIndices.push_back(p.materialIndex);

									// Save lightmap-index
									m_TriangleLightmapIndices.push_back(p.lightmapIndex);

                                    WorldTriangle triangle;
                                    triangle.flags = p.flags;
                                    memcpy(triangle.vertices, vx, sizeof(vx));

                                    // Save triangle
                                    m_Triangles.push_back(triangle);
                                }
                                else
                                {
                                    // Triangulate a triangle-fan
                                    //for(unsigned int i = p.polyNumVertices - 2; i >= 1; i--)
                                    for (unsigned int i = 1; i < p.polyNumVertices - 1; i++)
                                    {
                                        m_Indices.emplace_back(p.indices[0].VertexIndex);
                                        m_Indices.emplace_back(p.indices[i].VertexIndex);
                                        m_Indices.emplace_back(p.indices[i + 1].VertexIndex);

                                        m_FeatureIndices.emplace_back(p.indices[0].FeatIndex);
                                        m_FeatureIndices.emplace_back(p.indices[i].FeatIndex);
                                        m_FeatureIndices.emplace_back(p.indices[i + 1].FeatIndex);

                                        // Save material index for the written triangle
                                        m_TriangleMaterialIndices.push_back(p.materialIndex);

										// Save lightmap-index
										m_TriangleLightmapIndices.push_back(p.lightmapIndex);

                                        WorldTriangle triangle;
                                        triangle.flags = p.flags;

                                        uint32_t idx[] = {p.indices[0].VertexIndex, p.indices[i].VertexIndex,
                                                          p.indices[i + 1].VertexIndex};

                                        // Gather vertex information
                                        for (int v = 0; v < 3; v++)
                                        {
                                            triangle.vertices[v].Position = m_Vertices[idx[v]];
                                            triangle.vertices[v].Color = m_Features[idx[v]].lightStat;
                                            triangle.vertices[v].TexCoord = ZMath::float2(m_Features[idx[v]].uv[0],
                                                                                          m_Features[idx[v]].uv[1]);
                                            triangle.vertices[v].Normal = m_Features[idx[v]].vertNormal;
                                        }

                                        // Start filling in the flags
                                        m_Triangles.push_back(triangle);
                                    }
                                }
                            }
                        }

                        skipListEntry++;
                    } else
                    {

                    }

					// Goto next polygon using this weird shit
					blockPtr += blockSize + indicesSize * p.polyNumVertices;
				}

				parser.setSeek(chunkEnd); // Skip chunk, there could be more data here which is never read
			}
			break;

		case MSID_MESH_END:
			doneReadingChunks = true;
			break;

		default:
			parser.setSeek(chunkEnd); // Skip chunk
		}
	}
}


/**
* @brief Creates packed submesh-data
*/
void zCMesh::packMesh(PackedMesh& mesh, float scale)
{
	std::vector<WorldVertex>& newVertices = mesh.vertices;
	std::vector<uint32_t> newIndices;

	// Map of vertex-indices and their used feature-indices to a vertex in "newVertices"
	std::map<std::pair<uint32_t, uint32_t>, uint32_t> vfToNewVx;

	// Map of the new indices and the old indices to the index-vector
	std::unordered_map<uint32_t, uint32_t> newToOldIdxIdx;

	// Get vertices
	for(size_t i = 0, end = m_Indices.size(); i < end; i++)
	{
		uint32_t featidx = m_FeatureIndices[i];
		uint32_t vertidx = m_Indices[i];

		// Check if we already got this pair of vertex/feature
		auto it = vfToNewVx.find(std::make_pair(vertidx, featidx));
		if(it == vfToNewVx.end())
		{
			// Add new entry
			vfToNewVx[std::make_pair(vertidx, featidx)] = newVertices.size();
			WorldVertex vx;

			// Extract vertex information
			vx.Position = m_Vertices[vertidx] * scale;
			vx.Color = m_Features[featidx].lightStat;
			vx.TexCoord = ZMath::float2(m_Features[featidx].uv[0], m_Features[featidx].uv[1]);
			vx.Normal = m_Features[featidx].vertNormal;

			// Add index to this very vertex
			newIndices.push_back(newVertices.size());

			newVertices.push_back(vx);
		}
		else
		{
			// Simply put an index to the existing new vertex
			newIndices.push_back((*it).second);
		}

		// Store what this new index was before
		newToOldIdxIdx[newIndices.back()] = i;
	}

	
	
	// Filter textures
	std::unordered_map<std::string, uint32_t> materialsByTexture;
	std::unordered_map<uint32_t, uint32_t> newMaterialSlotsByMatIndex;
	for(size_t i = 0, end = m_Materials.size(); i < end; i++)
	{
		materialsByTexture[m_Materials[i].texture] = i;
		
	}

	// Assign materials to packed mesh
	uint32_t mc = 0;
	for(auto& m : materialsByTexture)
	{
		newMaterialSlotsByMatIndex[m.second] = mc++;
		mesh.subMeshes.emplace_back();
		mesh.subMeshes.back().material = m_Materials[m.second];
	}

	mesh.subMeshes.resize(materialsByTexture.size());

	// Add triangles
	for(size_t i = 0, end = newIndices.size(); i < end; i += 3)
	{
		// Get material info of this triangle
		uint32_t matIdx = m_TriangleMaterialIndices[i / 3];

		// Find texture of this material
		matIdx = newMaterialSlotsByMatIndex[materialsByTexture[m_Materials[matIdx].texture]];

		// Add this triangle to its submesh
		for(size_t j = 0; j < 3; j++)
			mesh.subMeshes[matIdx].indices.push_back(newIndices[i+j]);
	}

	// Store triangles with more information attached as well
	mesh.triangles.reserve(m_Triangles.size());
	for(auto& t : m_Triangles)
	{
		mesh.triangles.push_back(t);

		for(int v = 0; v < 3; v++)
			mesh.triangles.back().vertices[v].Position = mesh.triangles.back().vertices[v].Position * scale;
	}
}

void zCMesh::skip(ZenParser &parser)
{
    // Information about a single chunk
    BinaryChunkInfo chunkInfo;

    // Read chunks until we left the virtual binary file or got to the end-chunk
    // Each chunk starts with a header (BinaryChunkInfo) which gives information
    // about what to do and how long the chunk is
    while(true)
    {
        // Read chunk header and calculate position of next chunk
        parser.readStructure(chunkInfo);

        size_t chunkEnd = parser.getSeek() + chunkInfo.length;

        // Just skip all of it
        switch(chunkInfo.id)
        {
            case MSID_MESH_END:
                return;

            default:
                parser.setSeek(chunkEnd); // Skip chunk
        }
    }
}
