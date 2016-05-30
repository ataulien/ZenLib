#pragma once 
#include <vector>
#include "utils/mathlib.h"
#include "zTypes.h"
#include "zCMeshSoftSkin.h"

namespace VDFS
{
	class FileIndex;
}

namespace ZenLoad
{
	struct ModelNode
	{
		/**
		 * @return Whether the parent is valid or not (Invalid on root-nodes)
		 */
		bool parentValid() const { return parentIndex != 0xFFFF; }

		uint16_t parentIndex;
		std::string name;
		ZMath::Matrix transformLocal;
		ZMath::Matrix transformBindPoseDebug; // "Global"

		std::vector<uint16_t> childIndices;
	};

	class ZenParser;
	class zCModelMeshLib
	{
	public:

		zCModelMeshLib(){}

		/**
		 * @brief Loads the mesh from the given VDF-Archive
		 */
		zCModelMeshLib(const std::string& fileName, const VDFS::FileIndex& fileIndex);

		/**
		 * @brief Reads the mesh-object from the given binary stream
		 * @param fromZen Whether this mesh is supposed to be read from a zenfile. In this case, information about the binary chunk is also read.
		 */
		void loadMDM(ZenParser& parser);

		/**
		* @brief Reads the model hierachy from a file (MDH-File)
		*/
		void loadMDH(ZenParser& parser);

		/**
		* @brief reads this lib as MDL
		*/
		void loadMDL(ZenParser& parser);

		/**
		 * @brief Creates packed submesh-data
		 */
		void packMesh(PackedSkeletalMesh& mesh, float scale = 1.0f) const;

		/**
		 * @return List of meshes registered in this library
		 */
		const std::vector<zCMeshSoftSkin>& getMeshes() const { return m_Meshes; }

		/**
		 * @return List of nodes in the hierachy
		 */
		const std::vector<ModelNode>& getNodes() const { return m_Nodes; }

		/**
		 * @return Checksum for this node hierachy 
		 */
		uint32_t getNodeChecksum() const { return m_NodeChecksum; }
	private:
		/**
		 * @brief List of meshes registered in this library
		 */
		std::vector<zCMeshSoftSkin> m_Meshes;

		/** 
		 * @brief Node-Attachments
		 */
		std::vector<zCProgMeshProto> m_NodeAttachments;

		/**
		 * @brief Model hierachy
		 */
		std::vector<ModelNode> m_Nodes;
		std::vector<uint16_t> m_RootNodes;

		/**
		 * @brief Checksum for this node hierachy
		 */
		uint32_t m_NodeChecksum;
	};
}
