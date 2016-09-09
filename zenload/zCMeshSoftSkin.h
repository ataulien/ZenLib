#pragma once 
#include <vector>
#include "utils/mathlib.h"
#include "zTypes.h"
#include "zCProgMeshProto.h"

namespace VDFS
{
	class FileIndex;
}

namespace ZenLoad
{
	class ZenParser;
	class zCMeshSoftSkin
	{
	public:

		zCMeshSoftSkin(){}

		/**
		 * @brief Reads the mesh-object from the given binary stream
		 * @param fromZen Whether this mesh is supposed to be read from a zenfile. In this case, information about the binary chunk is also read.
		 */
		void readObjectData(ZenParser& parser);

		/**
		 * @return Internal zCProgMeshProto of this soft skin. The soft-skin only displaces the vertices found in the ProgMesh.
		 */
		const zCProgMeshProto& getMesh() const { return m_Mesh; }

		/**
		 * @brief Creates packed submesh-data
		 */
		void packMesh(PackedSkeletalMesh& mesh, float scale = 1.0f) const;
	private:
		/**
		 * @brief Internal zCProgMeshProto of this soft skin. The soft-skin only displaces the vertices found in the ProgMesh.
		 */
		zCProgMeshProto m_Mesh;

		/**
		 * @brief Stream containing the vertex-weights. Layout:
		 *		  uint32_t: numWeights
		 *		  numWeights* zTWeightEntry: weights
		 */
		std::vector<uint8_t> m_VertexWeightStream;
	};
}
