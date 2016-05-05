#include "zCModelMeshLib.h"
#include "zenParser.h"
#include "utils/logger.h"
#include "zTypes.h"
#include <string>
#include "vdfs/fileIndex.h"
#include "zCMeshSoftSkin.h"
#include "parserImpl.h"

using namespace ZenLoad;

// Types of chunks we will find in a zCModelMeshLib-Section

const uint32_t MDM_VERSION = 67699974; // TODO: Calculate this!

const uint16_t MLID_MODELMESH = 0xD000;
const uint16_t MLID_MDM_SOURCE = 0xD010;
const uint16_t MLID_MDM_NODEMESHES = 0xD020;
const uint16_t MLID_MDM_SOFSKINLIST = 0xD030;
const uint16_t MLID_MDM_END = 0xD040;

const uint32_t zMDH_VERS = 3;

const uint16_t MLID_MODELHIERARCHY = 0xD100;
const uint16_t MLID_MDH_SOURCE = 0xD110;
const uint16_t MLID_MDH_END	= 0xD120;

/**
* @brief Loads the lib from the given VDF-Archive
*/
zCModelMeshLib::zCModelMeshLib(const std::string& fileName, const VDFS::FileIndex& fileIndex)
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

		if(fileName.find(".MDM") != std::string::npos)
			loadMDM(parser);
		else if(fileName.find(".MDH") != std::string::npos)
			loadMDH(parser);
		else if(fileName.find(".MDL") != std::string::npos)
			loadMDL(parser);
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
void zCModelMeshLib::loadMDM(ZenParser& parser)
{
	// Information about the whole file we are reading here
	BinaryFileInfo fileInfo;

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
		case MLID_MODELMESH:{
			uint32_t version = parser.readBinaryDWord();

			parser.setSeek(chunkEnd); // Skip chunk
		}
		break;

		case MLID_MDM_SOURCE: {
			// TODO: Implement MDM-Conversion
			parser.setSeek(chunkEnd); // Skip chunk
		}
		 break;

		case MLID_MDM_NODEMESHES: {
			uint16_t numNodes = parser.readBinaryWord();

			std::vector<zCModelNodeInst> nodeList(numNodes);

			for(uint16_t i = 0; i < numNodes; i++)
			{
				std::string nodeName = parser.readLine(true);
				LogInfo() << "Loading node: " << nodeName;
			}

			for(uint16_t i = 0; i < numNodes; i++)
			{
				m_NodeAttachments.emplace_back();
				m_NodeAttachments.back().readObjectData(parser);
			}

			/*if (meshLib) meshLib->AllocNumNodeVisuals(num);
			for (i=0; i<num; i++) {
			zCProgMeshProto *pmesh = zNEW(zCProgMeshProto);
			if (!pmesh->Load (file)) goto convertASC;
			pmesh->PackStaticVertexBuffers();				// FIXME: ist wahrscheinlich nicht der richtige Ort dafuer!!
			if (destModel) {
			if (nodeList[i]) {
			int nodeIndex	= destModel->GetNodeList().Search (nodeList[i]);
			//						if (meshLib)	meshLib->AddNodeVisual		(nodeList[i], mesh);
			if (meshLib)	meshLib->AddNodeVisual		(nodeIndex, pmesh);
			else			nodeList[i]->SetNodeVisualS	(pmesh, TRUE);
			};
			} else {
			if (nodeProtoList[i]) {
			nodeProtoList[i]->SetNodeVisualS	(pmesh);
			};
			};
			// Dieser Kontext benoetigt das Mesh nicht mehr
			pmesh->Release();
			};
			};*/
		}
		break;

		case MLID_MDM_SOFSKINLIST:
		{
			uint32_t checksum = parser.readBinaryDWord();
			uint16_t numSoftSkins = parser.readBinaryWord();

			for(uint16_t i = 0; i < numSoftSkins; i++)
			{
				m_Meshes.emplace_back();
				m_Meshes.back().readObjectData(parser);
			}
		}
		break;

		case MLID_MDM_END:
			doneReadingChunks = true;
			break;
		default:
			parser.setSeek(chunkEnd); // Skip chunk

		}
	}
}

/**
* @brief Reads the model hierachy from a file (MDH-File)
*/
void zCModelMeshLib::loadMDH(ZenParser& parser)
{
	// Information about the whole file we are reading here
	BinaryFileInfo fileInfo;

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
		case MLID_MODELHIERARCHY:{
			uint32_t version = parser.readBinaryDWord();
			//if(version != MDM_VERSION)
			//	LogWarn() << "MDM Version mismatch!";

			uint16_t numNodes = parser.readBinaryWord();
			m_Nodes.resize(numNodes);

			for(int i = 0; i < numNodes; i++)
			{

				m_Nodes[i].name = parser.readLine(false); 
				m_Nodes[i].parentIndex = parser.readBinaryWord();

				if(m_Nodes[i].parentIndex != 0xFFFF)
				{
					if(m_Nodes[i].parentIndex >= numNodes)
						LogWarn() << "MSH-Parse fail: parentIndex >= numNodes";

					ModelNode* parent = &m_Nodes[m_Nodes[i].parentIndex];
					parent->childIndices.push_back(i);

				}
				else
				{
					m_Nodes[i].parentIndex = 0xFFFF;

					// Found root node
					m_RootNodes.push_back(i);
				}

				parser.readBinaryRaw(&m_Nodes[i].transformLocal, sizeof(Math::Matrix));
			}

			Math::float3 bbox3d[2]; parser.readBinaryRaw(bbox3d, sizeof(bbox3d));
			Math::float3 bbox3dCollision[2]; parser.readBinaryRaw(bbox3dCollision, sizeof(bbox3dCollision));

			Math::float3 rootNodeTrans; parser.readBinaryRaw(&rootNodeTrans, sizeof(rootNodeTrans));

			m_NodeChecksum = parser.readBinaryDWord();

			parser.setSeek(chunkEnd); // Skip chunk
		}
		break;

		case MLID_MDH_SOURCE: {
			// TODO: Implement MDM-Conversion
			parser.setSeek(chunkEnd); // Skip chunk
		}
		break;

		case MLID_MDH_END:
			doneReadingChunks = true;
			break;
		default:
			parser.setSeek(chunkEnd); // Skip chunk

		}
	}
}

/**
* @brief reads this lib as MDL
*/
void zCModelMeshLib::loadMDL(ZenParser& parser)
{
	loadMDH(parser);
	loadMDM(parser);
}

/**
* @brief Creates packed submesh-data
*/
void zCModelMeshLib::packMesh(PackedSkeletalMesh& mesh, float scale) const
{
	for(const auto& m : m_Meshes)
	{
		m.packMesh(mesh, scale);
	}

	// TODO: Implement node-attachments!
	/*for(const auto& m : m_NodeAttachments)
	{
		m.packMesh(mesh, scale);
	}*/

	
}
