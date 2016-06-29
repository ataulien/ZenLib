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
	class zCProgMeshProto
	{
	public:
		struct SubMesh
		{
			zCMaterialData m_Material;
			std::vector<zTriangle> m_TriangleList;
			std::vector<zWedge> m_WedgeList;
			std::vector<float> m_ColorList;
			std::vector<uint16_t> m_TrianglePlaneIndexList;
			std::vector<zTPlane> m_TrianglePlaneList;
			std::vector<zTriangleEdges>	m_TriEdgeList;
			std::vector<zEdge> m_EdgeList;
			std::vector<float> m_EdgeScoreList;	
			std::vector<uint16_t> m_WedgeMap;			
		};

			
		zCProgMeshProto(){}

		/**
		 * @brief Loads the mesh from the given VDF-Archive
		 */
		zCProgMeshProto(const std::string& fileName, const VDFS::FileIndex& fileIndex);

		/**
		 * @brief Reads the mesh-object from the given binary stream
		 */
		void readObjectData(ZenParser& parser);

		/**
		@ brief returns the vector of vertex-positions
		*/
		const std::vector<ZMath::float3>& getVertices() const { return m_Vertices; }

		/**
		@ brief returns the vector of features
		*/
		const std::vector<zTMSH_FeatureChunk>& getFeatures() const { return m_Features; }

		/**
		 * @brief returns the vector of the materials used by this mesh
		 */
		const std::vector<zCMaterialData>& getMaterials() const { return m_Materials; }

		/**
		 * @brief getter for the boudingboxes
		 */
		void getBoundingBox(ZMath::float3& min, ZMath::float3& max)const{min = m_BBMin; max = m_BBMax;}

		/**
		 * @brief Returns the submesh with the given index
		 */
		const SubMesh& getSubmesh(size_t idx)const{return m_SubMeshes[idx];}
		size_t getNumSubmeshes()const{return m_SubMeshes.size(); }

		/**
		 * @brief Returns the global position-list
		 */
		const std::vector<ZMath::float3> getPositionList()const{return m_Vertices;}

		/**
		* @brief Creates packed submesh-data
		*/
		void packMesh(PackedMesh& mesh, float scale = 1.0f) const;

		/**
		* @brief Packs vertices only
		*/
		void packVertices(std::vector<WorldVertex>& vxs, std::vector<uint32_t>& ixs, uint32_t indexStart, std::vector<uint32_t>& submeshIndexStarts, float scale = 1.0f) const;

		/**
		 * @return BBox min/max
		 */
		ZMath::float3 getBBoxMin(){return m_BBMin;}
		ZMath::float3 getBBoxMax(){return m_BBMax;}

	private:


		/**
		 * @brief vector of vertex-positions for this mesh
		 */
		std::vector<ZMath::float3> m_Vertices;
		std::vector<ZMath::float3> m_Normals;

		/** 
		 * @brief Featues for the vertices with the corresponding index
		 */
		std::vector<zTMSH_FeatureChunk> m_Features;

		/** 
		* @brief List of submeshes
		*/
		std::vector<SubMesh> m_SubMeshes;

		/**
		* @brief All materials used by this mesh
		*/
		std::vector<zCMaterialData> m_Materials;

		/**
		 * @brief Whether this mesh is using alphatest
		 */
		uint8_t m_IsUsingAlphaTest;

		/**
		 * @brief Bounding-box of this mesh
		 */
		ZMath::float3 m_BBMin;
		ZMath::float3 m_BBMax;
	};
}
