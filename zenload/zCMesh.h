#pragma once 
#include <vector>
#include "utils/mathlib.h"
#include "zTypes.h"
#include "zenParser.h"

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
		 * @param skipPolys These polygons will be skipped while loading. These need to be ordered! (Used to load world witout LOD in G1)
		 * @param force32bitIndices Loads all indices as 32-bit, even though the ZEN may come from Gothic 1
		 */
		void readObjectData(ZenParser& parser, const std::vector<size_t>& skipPolys = std::vector<size_t>(),
							bool forceG132bitIndices = false);

		/**
		 * Simply skips all data found here
		 * @param parser
		 */
		static void skip(ZenParser& parser);

		/**
		@ brief returns the vector of vertex-positions
		*/
		const std::vector<ZMath::float3>& getVertices() const { return m_Vertices; }

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
		const std::vector<int16_t>& getTriangleMaterialIndices() const { return m_TriangleMaterialIndices; }

		/**
		* @brief returns the vector of triangle-material-indices
		*/
		const std::vector<int16_t>& getTriangleLightmapIndices() const { return m_TriangleLightmapIndices; }

		/**
		 * @brief returns the vector of the materials used by this mesh
		 */
		const std::vector<zCMaterialData>& getMaterials() const { return m_Materials; }



		/**
		 * @brief getter for the boudingboxes
		 */
		void getBoundingBox(ZMath::float3& min, ZMath::float3& max){min = m_BBMin; max = m_BBMax;}

		/**
		 * @brief Creates packed submesh-data
		 */
		void packMesh(PackedMesh& mesh, float scale = 1.0f, bool removeDoubles=true);
	private:

		/**
		 * @brief vector of vertex-positions for this mesh
		 */
		std::vector<ZMath::float3> m_Vertices;

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
		std::vector<int16_t> m_TriangleMaterialIndices;

		/**
		 * @brief Index of the lightmap used by the triangles in order. -1 of no lightmap registered.
		 */
		std::vector<int16_t> m_TriangleLightmapIndices;

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
		ZMath::float3 m_BBMin;
		ZMath::float3 m_BBMax;
	};
}
