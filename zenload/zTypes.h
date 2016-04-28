#pragma once
#include <inttypes.h>
#include "utils/mathlib.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace ZenLoad
{
	/**
	 * @brief Maximum amount of nodes a skeletal mesh can render
	 */
	const size_t MAX_NUM_SKELETAL_NODES = 96;

	struct WorldVertex
	{
		Math::float3 Position;
		Math::float3 Normal;
		Math::float2 TexCoord;
		uint32_t Color;
	};

	struct SkeletalVertex
	{
		Math::float3 Normal;
		Math::float2 TexCoord;
		uint32_t Color;
		Math::float3 LocalPositions[4];
		unsigned char BoneIndices[4];
		float Weights[4];
	};
	
	struct zMAT3
	{
		float v[3][3];

		Math::float3 getUpVector()const { return Math::float3(v[0][1], v[1][1], v[2][1]); };
		Math::float3 getRightVector()const { return Math::float3(v[0][0], v[1][0], v[2][0]); };
		Math::float3 getAtVector()const { return Math::float3(v[0][2], v[1][2], v[2][2]); };
		
		void setAtVector (const Math::float3& a) { v[0][2] = a.x; v[1][2] = a.y; v[2][2] = a.z; }
		void setUpVector(const Math::float3& a) { v[0][1] = a.x; v[1][1] = a.y; v[2][1] = a.z; }
		void setRightVector (const Math::float3& a) { v[0][0] = a.x; v[1][0] = a.y;	v[2][0] = a.z;}

		Math::Matrix toMatrix(const Math::float3& position = Math::float3(0,0,0)) const 
		{
			Math::Matrix m = Math::Matrix::CreateIdentity();
			m.Up(getUpVector());
			m.Forward(-1.0f * getAtVector());
			m.Right(getRightVector());
			m.Translation(position);

			return m;
		}
	};

	/**
	 * @brief Base for an object parsed from a zen-file.
	 *		  Contains a map of all properties and their values as text
	 */
	struct ParsedZenObject
	{
		std::unordered_map<std::string, std::string> properties;
		std::string objectClass;
	};


	/**
	* @brief All kinds of information found in a zCMaterial
	*/
	struct zCMaterialData : public ParsedZenObject
	{
		std::string		matName;
		uint8_t			matGroup;
		uint32_t		color;
		float			smoothAngle;
		std::string		texture;
		std::string		texScale;
		float			texAniFPS;
		uint8_t			texAniMapMode;
		std::string		texAniMapDir;
		uint8_t			noCollDet;
		uint8_t			noLighmap;
		uint8_t			loadDontCollapse;
		std::string		detailObject;
		float			detailTextureScale;
		uint8_t			forceOccluder;
		uint8_t			environmentMapping;
		float			environmentalMappingStrength;
		uint8_t			waveMode;
		uint8_t			waveSpeed;
		float			waveMaxAmplitude;
		float			waveGridSize;
		uint8_t			ignoreSun;
		uint8_t			alphaFunc;
		Math::float2	defaultMapping;
	};

	struct zCVisualData : public ParsedZenObject
	{

	};

	struct zCAIBaseData : public ParsedZenObject
	{

	};

	struct zCEventManagerData : public ParsedZenObject
	{

	};

//#pragma pack(push, 1)
	/**
	 * @brief Data of zCVob
	 */
	struct zCVobData : public ParsedZenObject
	{
		uint32_t pack;
		std::string presetName;
		Math::float3 bbox[2];
		Math::Matrix rotationMatrix;
		zMAT3 rotationMatrix3x3;
		Math::Matrix worldMatrix;
		Math::float3 position;
		std::string vobName;
		std::string visual;
		bool showVisual;
		uint8_t visualCamAlign;
		uint8_t visualAniMode;
		float visualAniModeStrength;
		float vobFarClipScale;
		bool cdStatic;
		bool cdDyn;
		bool staticVob;
		uint8_t dynamicShadow;
		int32_t zBias;
		bool isAmbient;

		// References
		size_t visualReference;
		size_t aiReference;
		size_t eventMgrReference;

		bool physicsEnabled;

		std::vector<zCVobData> childVobs;
	};
//#pragma pack(pop)



	/**
	* @brief All kinds of information found in a oCWorld
	*/
	struct oCWorldData : public ParsedZenObject
	{
		std::vector<zCVobData> rootVobs;
	};

#pragma pack(push, 1)
	// Information about the whole file we are reading here
	struct BinaryFileInfo
	{
		uint32_t version;
		uint32_t size;
	};

	// Information about a single chunk 
	struct BinaryChunkInfo
	{
		uint16_t id;
		uint32_t length;
	};

	struct PolyFlags 
	{
		uint8_t			portalPoly			: 2;		
		uint8_t			occluder			: 1;		
		uint8_t			sectorPoly			: 1;		
		uint8_t			mustRelight			: 1;		
		uint8_t			portalIndoorOutdoor	: 1;		
		uint8_t			ghostOccluder		: 1;		
		uint8_t			noDynLightNear		: 1;		
		uint16_t		sectorIndex			: 16;
	};

	struct zTMSH_FeatureChunk 
	{
		float			uv[2];
		uint32_t		lightStat;
		Math::float3	vertNormal;
	};

#pragma pack(pop)

	/**
	* @brief Information about a triangle in the World. Contains whether the triangle 
	*		  belongs to an outside/inside location, the static lighting colors of the edges,
	*		  material-information and to which sector this belongs, amongst others
	*/
	struct WorldTriangle
	{
		/**
		* @brief Returns whether this triangle is outside or inside
		*/
		bool isOutside() const { return !flags.sectorPoly; }

		/**
		* @brief Returns the interpolated lighting value for the given position on the triangle
		*/
		Math::float4 interpolateLighting(const Math::float3& position) const 
		{
			float u,v,w;
			Math::barycentric(position, vertices[0].Position, vertices[1].Position, vertices[2].Position, u, v, w);

			Math::float4 c[3];
			c[0].fromABGR8(vertices[0].Color);
			c[1].fromABGR8(vertices[1].Color);
			c[2].fromABGR8(vertices[2].Color);

			return u * c[0] + v * c[1] + w * c[2];
		}

		/**
		* @brief Flags taken from the original ZEN-File
		*/
		PolyFlags flags;

		/**
		* @brief Vertices belonging to this triangle
		*/
		WorldVertex vertices[3];
	};

	/**
	* @brief Simple generic packed mesh, containing all useful information of a (lod-level of) zCMesh and zCProgMeshProto
	*/
	// FIXME: Probably move this to renderer-package
	struct PackedMesh
	{
		struct SubMesh
		{
			zCMaterialData material;			
			std::vector<uint32_t> indices;		
		};

		std::vector<WorldTriangle> triangles; // Use index / 3 to access these
		std::vector<WorldVertex> vertices;
		std::vector<SubMesh> subMeshes;
	};

	struct PackedSkeletalMesh
	{
		struct SubMesh
		{
			zCMaterialData material;			
			std::vector<uint32_t> indices;		
		};

		std::vector<SkeletalVertex> vertices;
		std::vector<SubMesh> subMeshes;
	};

#pragma pack(push, 4)

	struct VobObjectInfo
	{
		Math::Matrix worldMatrix;
		Math::float4 color;
	};

	struct SkeletalMeshInstanceInfo
	{
		Math::Matrix worldMatrix;
		Math::Matrix nodeTransforms[MAX_NUM_SKELETAL_NODES];
		Math::float4 color;
	};

	struct zDate
	{
		uint32_t year;
		uint16_t month;
		uint16_t day;	
		uint16_t hour;
		uint16_t minute;
		uint16_t second;
	};

	struct zTPlane 
	{
		float distance;
		Math::float3 normal;
	};
#pragma pack(pop)

	struct zWedge 
	{
		Math::float3 m_Normal;
		Math::float2 m_Texcoord;
		uint16_t m_VertexIndex;
	};

	struct zTriangle
	{
		uint16_t m_Wedges[3];
	};													

	struct zTriangleEdges 
	{
		uint16_t m_Edges[3];
	};													

	struct zEdge 
	{
		uint16_t m_Wedges[2];
	};		

#pragma pack(push, 4)
	struct zTNodeWedgeNormal 
	{
		Math::float3	m_Normal;
		int				m_NodeIndex;
	};

	struct zTLODParams 
	{
		float m_LodStrength;
		float m_ZDisplace2;	
		float m_MorphPercent;	
		int32_t   m_MinNumVertices;	
	};

#pragma pack(pop)

#pragma pack (push, 1)
	class zCModelNodeInst 
	{
	public:

	};

	struct zTMdl_AniSample 
	{
		uint16_t	rotation[3]; // 16bit quantized euler angles
		uint16_t	position[3]; // 16bit quantized float3
	};
#pragma pack (pop)

#pragma pack (push, 1)
	struct zTWeightEntry 
	{
		// Weight and position of the vertex.
		// This vertexposition is in the local space of the joint-Matrix!
		float weight;
		Math::float3 localVertexPosition;

		// Nodeindex this belongs to
		unsigned char nodeIndex;
	};
#pragma pack (pop)

	enum zTMdl_AniEventType 
	{
		zMDL_EVENT_TAG,
		zMDL_EVENT_SOUND,				
		zMDL_EVENT_SOUND_GRND,			
		zMDL_EVENT_ANIBATCH,
		zMDL_EVENT_SWAPMESH,			
		zMDL_EVENT_HEADING,				
		zMDL_EVENT_PFX,
		zMDL_EVENT_PFX_GRND,
		zMDL_EVENT_PFX_STOP,
		zMDL_EVENT_SETMESH,
		zMDL_EVENT_MM_STARTANI,
		zMDL_EVENT_CAM_TREMOR,			
	};
}
