#pragma once 
#include <vector>
#include "utils/mathlib.h"
#include "zTypes.h"

namespace VDFS
{
	class FileIndex;
}

namespace ZenLoad
{
	class ZenParser;
	class zCMesh
	{
	public:

		zCMesh(){}

		/**
		 * @brief Loads the mesh from the given VDF-Archive
		 */
		zCMesh(const std::string& fileName, VDFS::FileIndex& fileIndex);

		/**
		 * @brief Reads the mesh-object from the given binary stream
		 * @param fromZen Whether this mesh is supposed to be read from a zenfile. In this case, information about the binary chunk is also read.
		 */
		void readObjectData(ZenParser& parser, bool fromZen);

		/**
		@ brief returns the vector of vertex-positions
		*/
		const std::vector<Math::float3>& getVertices() const { return m_Vertices; }

		/**
		@ brief returns the vector of features
		*/
		const std::vector<zTMSH_FeatureChunk>& getFeatures() const { return m_Features; }


		/**
		 * @brief returns the vector of triangle-indices
		 */
		const std::vector<uint32_t>& getIndices() const { return m_Indices; }

		/**
		 * @brief returns the vector of triangle-indices
		 */
		const std::vector<uint32_t>& getFeatureIndices() const { return m_FeatureIndices; }
		
		/**
		* @brief returns the vector of triangle-material-indices
		*/
		const std::vector<uint32_t>& getTriangleMaterialIndices() const { return m_TriangleMaterialIndices; }

		/**
		 * @brief returns the vector of the materials used by this mesh
		 */
		const std::vector<zCMaterialData>& getMaterials() const { return m_Materials; }

		/**
		 * @brief getter for the boudingboxes
		 */
		void getBoundingBox(Math::float3& min, Math::float3& max){min = m_BBMin; max = m_BBMax;}

		/**
		 * @brief Creates packed submesh-data
		 */
		void packMesh(PackedMesh& mesh, float scale = 1.0f);
	private:

		/**
		 * @brief vector of vertex-positions for this mesh
		 */
		std::vector<Math::float3> m_Vertices;

		/** 
		 * @brief Featues for the vertices with the corresponding index
		 */
		std::vector<zTMSH_FeatureChunk> m_Features;

		/**
		 * @brief indices for the triangles of the mesh
		 */
		std::vector<uint32_t> m_Indices;

		/**
		* @brief indices for the triangles of the mesh
		*/
		std::vector<uint32_t> m_FeatureIndices;

		/**
		 * @brief Textures for the triangles. Divide index-position by 3 to get the 
		 * corresponding material-info 
		 */
		std::vector<uint32_t> m_TriangleMaterialIndices;

		/**
		 * @brief All materials used by this mesh
		 */
		std::vector<zCMaterialData> m_Materials;

		/**
		 * @brief Triangles of the current mesh, containing flags
		 */
		std::vector<WorldTriangle> m_Triangles;

		/**
		 * @brief Bounding-box of this mesh
		 */
		Math::float3 m_BBMin;
		Math::float3 m_BBMax;
	};
}
