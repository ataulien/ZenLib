#pragma once
#include <inttypes.h>
#include "utils/mathlib.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace ZenLoad
{
	enum class WorldVersion
	{
		VERSION_G1_08k = 1,
		VERSION_G26fix = 2
	};

	/**
	 * @brief Maximum amount of nodes a skeletal mesh can render
	 */
	const size_t MAX_NUM_SKELETAL_NODES = 96;

	struct WorldVertex
	{
		ZMath::float3 Position;
		ZMath::float3 Normal;
		ZMath::float2 TexCoord;
		uint32_t Color;
	};

	struct SkeletalVertex
	{
		ZMath::float3 Normal;
		ZMath::float2 TexCoord;
		uint32_t Color;
		ZMath::float3 LocalPositions[4];
		unsigned char BoneIndices[4];
		float Weights[4];
	};
	
	struct zMAT3
	{
		float v[3][3];

		ZMath::float3 getUpVector()const { return ZMath::float3(v[0][1], v[1][1], v[2][1]); };
		ZMath::float3 getRightVector()const { return ZMath::float3(v[0][0], v[1][0], v[2][0]); };
		ZMath::float3 getAtVector()const { return ZMath::float3(v[0][2], v[1][2], v[2][2]); };
		
		void setAtVector (const ZMath::float3& a) { v[0][2] = a.x; v[1][2] = a.y; v[2][2] = a.z; }
		void setUpVector(const ZMath::float3& a) { v[0][1] = a.x; v[1][1] = a.y; v[2][1] = a.z; }
		void setRightVector (const ZMath::float3& a) { v[0][0] = a.x; v[1][0] = a.y;	v[2][0] = a.z;}

		ZMath::Matrix toMatrix(const ZMath::float3& position = ZMath::float3(0,0,0)) const
		{
			ZMath::Matrix m = ZMath::Matrix::CreateIdentity();
			m.Up(getUpVector());
                        
                        ZMath::float3 fwd = getAtVector();
                        fwd.x *= -1.0f;
                        fwd.y *= -1.0f;
                        fwd.z *= -1.0f;
                        
			m.Forward(fwd);
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
		bool			noCollDet;
		bool			noLighmap;
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
		ZMath::float2	defaultMapping;
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
		ZMath::float3 bbox[2];
		ZMath::Matrix rotationMatrix;
		zMAT3 rotationMatrix3x3;
		ZMath::Matrix worldMatrix;
		ZMath::float3 position;
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

		// Sub-classes
		struct
		{
			std::string instanceName;
		}oCItem;

		struct
		{
			std::string focusName;
			int32_t 	hitpoints;
			int32_t 	damage;
			bool 		moveable;
			bool		takeable;
			bool		focusOverride;
			uint8_t		soundMaterial;
			std::string	visualDestroyed;
			std::string	owner;
			std::string ownerGuild;
			bool		isDestroyed;
		}oCMOB;

		struct
		{
			int 		stateNum;
			std::string triggerTarget;
			std::string useWithItem;
			std::string conditionFunc;
			std::string onStateFunc;
			bool		rewind;
		}oCMobInter;

		std::vector<zCVobData> childVobs;
	};

	struct zCBspNode
	{
		enum : size_t { INVALID_NODE = static_cast<size_t>(-1) };

		ZMath::float4 plane;
		size_t front, back;
		size_t parent;

		ZMath::float3 bbox3dMin, bbox3dMax;

		size_t treePolyIndex;
		size_t numPolys;

		bool isLeaf()
		{
			return front == INVALID_NODE && back == INVALID_NODE;
		}
	};

	struct zCBspTreeData
	{
		enum TreeMode
		{
			Indoor = 0,
			Outdoor = 1
		};

		/**
		 * Whether this tree is an indoor or outdoor location
		 */
		TreeMode mode;

		std::vector<zCBspNode> nodes;
		std::vector<uint32_t > leafIndices;
		std::vector<uint32_t> treePolyIndices;
	};

	struct zCBspTreeData2
	{
		enum TreeMode
		{
			Indoor = 0,
			Outdoor = 1
		};

		/**
		 * Whether this tree is an indoor or outdoor location
		 */
		TreeMode mode;

		std::vector<zCBspNode> nodes;
		std::vector<uint8_t > leafIndices;
		std::vector<uint16_t> treePolyIndices;
	};
//#pragma pack(pop)

	struct oCMsgConversationData : public ParsedZenObject
	{
		uint8_t subType;
		std::string text;
		std::string name;
	};

	struct zCCSRoleData : public ParsedZenObject
	{
		bool mustBeAlive;
		std::string roleName;
		uint8_t roleType;
	};

	struct zCCSBlockSyncData : public ParsedZenObject
	{
		std::vector<uint32_t> roleAss;
	};

	struct zCCSAtomicBlockData : public ParsedZenObject
	{
		oCMsgConversationData command;
	};

	struct zCCSBlockData : public ParsedZenObject
	{
		std::string blockName;

		// This structure is different in the original files, but this is the only
		// configuration happening while loading
		zCCSAtomicBlockData atomicBlockData;
	};


	/*struct zCCutsceneData
	{
		zCCSPropsData props;
		std::vector<zCCSRoleData> roles;
		std::vector<zCVobData> roleVobs;
		zCVobData mainRoleVob;
	};*/

	/*struct zCEventManagerData
	{
		bool cleared;
		bool active;
		zCCutsceneData cutsceneData;
	};*/

	struct zCCSLibData : public ParsedZenObject
	{
		std::vector<zCCSAtomicBlockData> blocks; // Gothic is only using atomic blocks, as it seems
	};



	struct zCWaypointData : public ParsedZenObject
	{
		std::string wpName;
		int32_t waterDepth;
		bool underWater;
		ZMath::float3 position;
		ZMath::float3 direction;
	};

	struct zCWayNetData : public ParsedZenObject
	{
		uint32_t waynetVersion;
		std::vector<zCWaypointData> waypoints;
		std::vector<std::pair<size_t, size_t>> edges;
	};

	/**
	* @brief All kinds of information found in a oCWorld
	*/
	struct oCWorldData : public ParsedZenObject
	{
		std::vector<zCVobData> rootVobs;
		zCWayNetData waynet;
		zCBspTreeData bspTree;
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

	/**
	 * @brief Merged flags-struct to contain all flags from all versions of the files
	 */
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
		uint8_t			lodFlag				: 1;
		uint8_t			normalMainAxis		: 2;
	};

	struct PolyFlags2_6fix
	{
		uint8_t			portalPoly			: 2;		
		uint8_t			occluder			: 1;		
		uint8_t			sectorPoly			: 1;		
		uint8_t			mustRelight			: 1;		
		uint8_t			portalIndoorOutdoor	: 1;		
		uint8_t			ghostOccluder		: 1;		
		uint8_t			noDynLightNear		: 1;		
		uint16_t		sectorIndex			: 16;

        PolyFlags2_6fix(){}
        PolyFlags2_6fix(const PolyFlags& p)
        {
            portalPoly = p.portalPoly;
            occluder = p.occluder;
            sectorPoly = p.sectorPoly ;
            mustRelight = p.mustRelight;
            portalIndoorOutdoor = p.portalIndoorOutdoor;
            ghostOccluder = p.ghostOccluder;
            noDynLightNear = p.noDynLightNear;
            sectorIndex = p.sectorIndex;
        }

		PolyFlags generify() const
		{
			PolyFlags p;
			memset(&p, 0, sizeof(p));

			p.portalPoly = portalPoly;
			p.occluder = occluder;
			p.sectorPoly = sectorPoly;
			p.mustRelight = mustRelight;
			p.portalIndoorOutdoor = portalIndoorOutdoor;
			p.ghostOccluder = ghostOccluder;
			p.noDynLightNear = noDynLightNear;
			p.sectorIndex = sectorIndex;

			return p;
		}
	};

	struct PolyFlags1_08k
	{
		uint8_t			portalPoly			: 2;
		uint8_t			occluder			: 1;
		uint8_t			sectorPoly			: 1;
		uint8_t			lodFlag				: 1;
		uint8_t			portalIndoorOutdoor	: 1;
		uint8_t			ghostOccluder		: 1;
		uint8_t			normalMainAxis		: 2;
		uint16_t		sectorIndex			: 16;


        PolyFlags1_08k(){}
        PolyFlags1_08k(const PolyFlags& p)
        {
            portalPoly = p.portalPoly;
            occluder = p.occluder;
            sectorPoly = p.sectorPoly ;
            lodFlag = p.lodFlag;
            portalIndoorOutdoor = p.portalIndoorOutdoor;
            ghostOccluder = p.ghostOccluder;
            normalMainAxis = p.normalMainAxis;
            sectorIndex = p.sectorIndex;
        }

        PolyFlags generify() const
        {
            PolyFlags p;
            memset(&p, 0, sizeof(p));

            p.portalPoly = portalPoly;
            p.occluder = occluder;
            p.sectorPoly = sectorPoly;
            p.lodFlag = lodFlag;
            p.portalIndoorOutdoor = portalIndoorOutdoor;
            p.ghostOccluder = ghostOccluder;
            p.normalMainAxis = normalMainAxis;
            p.sectorIndex = sectorIndex;

			return p;
        }
	};



	struct zTMSH_FeatureChunk 
	{
		float			uv[2];
		uint32_t		lightStat;
		ZMath::float3	vertNormal;
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
		
                ZMath::float4 interpolateLighting(const ZMath::float3& position) const
		{
			/*float u,v,w;
			ZMath::barycentric(position, vertices[0].Position, vertices[1].Position, vertices[2].Position, u, v, w);

			ZMath::float4 c[3];
			c[0].fromABGR8(vertices[0].Color);
			c[1].fromABGR8(vertices[1].Color);
			c[2].fromABGR8(vertices[2].Color);

			return u * c[0] + v * c[1] + w * c[2];*/
                        
                        return ZMath::float4(1,1,1,1); // TODO: Implementation missing without GLM
		}

		/**
		* @brief Flags taken from the original ZEN-File
		*/
		PolyFlags flags;

		/**
		 * @brief Index to the lightmap stored in the zCMesh
		 */
		int16_t lightmapIndex;

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
			std::vector<int16_t> triangleLightmapIndices; // Index values to the texture found in zCMesh
		};

		std::vector<WorldTriangle> triangles; // Use index / 3 to access these
		std::vector<WorldVertex> vertices;
		std::vector<SubMesh> subMeshes;
		ZMath::float3 bbox[2];
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
		ZMath::Matrix worldMatrix;
		ZMath::float4 color;
	};

	struct SkeletalMeshInstanceInfo
	{
		ZMath::Matrix worldMatrix;
		ZMath::Matrix nodeTransforms[MAX_NUM_SKELETAL_NODES];
		ZMath::float4 color;
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
		ZMath::float3 normal;
	};
#pragma pack(pop)

	struct zWedge 
	{
		ZMath::float3 m_Normal;
		ZMath::float2 m_Texcoord;
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
		ZMath::float3	m_Normal;
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
		ZMath::float3 localVertexPosition;

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
