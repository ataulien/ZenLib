#pragma once
#include "zTypes.h"
#include "zenParser.h"
#include "zenParserPropRead.h"

namespace ZenLoad
{
	class zCVob
	{
#pragma pack (push, 1)
		struct packedVobData
		{
			ZMath::float3	bbox3DWS[2];
			ZMath::float3	positionWS;
			zMAT3			trafoRotWS;

			struct packedBitField
			{
				uint8_t		showVisual			: 1;		
				uint8_t		visualCamAlign		: 2;
				uint8_t		cdStatic			: 1;
				uint8_t		cdDyn				: 1;
				uint8_t		staticVob			: 1;
				uint8_t		dynShadow			: 2;
				uint8_t		hasPresetName		: 1;
				uint8_t		hasVobName			: 1;
				uint8_t		hasVisualName		: 1;
				uint8_t		hasRelevantVisualObject	: 1;
				uint8_t		hasAIObject			: 1;
				uint8_t		hasEventManObject	: 1;		
				uint8_t		physicsEnabled		: 1;		
				uint8_t		visualAniMode		: 2;
				uint8_t		zbias				: 5;	
				uint8_t		bAmbient			: 1;
			} bitfield;

			float			visualAniStrength;
			float			vobFarClipZ;

		};
#pragma pack (pop)

		enum {
			VERSION_G1_08k = 12289,
			VERSION_G26fix = 0 // TODO
		};

	public:
		/**
		* Reads this object from an internal zen
		*/
		static void readObjectData(zCVobData& info, ZenParser& parser, WorldVersion version, const ZenParser::ChunkHeader& header)
		{
			info.objectClass = "zCVob";

			info.rotationMatrix = ZMath::Matrix::CreateIdentity();

			// Read how many vobs this one has as child
			parser.getImpl()->readEntry("", &info.pack, sizeof(info.pack), ZenLoad::ParserImpl::ZVT_INT);
			
			if(info.pack)
			{
				packedVobData pd;
				parser.getImpl()->readEntry("", &pd, sizeof(pd), ZenLoad::ParserImpl::ZVT_RAW);

				info.bbox[0] = pd.bbox3DWS[0];
				info.bbox[1] = pd.bbox3DWS[1];
				info.position = pd.positionWS;
				info.rotationMatrix3x3 = pd.trafoRotWS;
				info.rotationMatrix = info.rotationMatrix3x3.toMatrix();
				info.showVisual = pd.bitfield.showVisual;
				info.visualCamAlign = pd.bitfield.visualCamAlign;
				info.cdStatic = pd.bitfield.cdStatic;
				info.cdDyn = pd.bitfield.cdDyn;
				info.staticVob = pd.bitfield.staticVob;
				info.dynamicShadow = pd.bitfield.dynShadow;
				info.physicsEnabled = pd.bitfield.physicsEnabled;
				info.visualAniMode = pd.bitfield.visualAniMode;
				info.zBias = pd.bitfield.zbias;
				info.isAmbient = pd.bitfield.bAmbient;
				info.visualAniModeStrength = pd.visualAniStrength;
				info.vobFarClipScale = pd.vobFarClipZ;

				if(pd.bitfield.hasPresetName)
					parser.getImpl()->readEntry("", &info.presetName, 0, ZenLoad::ParserImpl::ZVT_STRING);

				if(pd.bitfield.hasVobName)
					parser.getImpl()->readEntry("", &info.vobName, 0, ZenLoad::ParserImpl::ZVT_STRING);

				if(pd.bitfield.hasVisualName)
					parser.getImpl()->readEntry("", &info.visual, 0, ZenLoad::ParserImpl::ZVT_STRING);

				if(pd.bitfield.hasRelevantVisualObject)
				{
					// Skip visual-chunk
					ZenParser::ChunkHeader tmph;
					parser.readChunkStart(tmph);
					parser.skipChunk();
				}

				if(pd.bitfield.hasAIObject)
				{
					// Skip ai-chunk
					ZenParser::ChunkHeader tmph;
					parser.readChunkStart(tmph);
					parser.skipChunk();
				}

				/*info.properties.insert(std::make_pair("PresetName", info.presetName));
				info.properties.insert(std::make_pair("BBoxMin", info.bbox[0].toString()));
				info.properties.insert(std::make_pair("BBoxMax", info.bbox[1].toString()));
				info.properties.insert(std::make_pair("RotationMatrix", info.rotationMatrix.toString()));
				info.properties.insert(std::make_pair("Position", info.position.toString()));
				info.properties.insert(std::make_pair("VobName", info.vobName));
				info.properties.insert(std::make_pair("VisualName", info.visual));
				info.properties.insert(std::make_pair("ShowVisual", std::to_string(info.showVisual ? 1 : 0)));
				info.properties.insert(std::make_pair("VisualCamAlign", std::to_string(info.visualCamAlign)));
				info.properties.insert(std::make_pair("VisualAniMode", std::to_string(info.visualAniMode)));
				info.properties.insert(std::make_pair("VisualAniModeStrength", std::to_string(info.visualAniModeStrength)));
				info.properties.insert(std::make_pair("VobFarClipScale", std::to_string(info.vobFarClipScale)));
				info.properties.insert(std::make_pair("CollisionDetectionStatic", std::to_string(info.cdStatic ? 1 : 0)));
				info.properties.insert(std::make_pair("CollisionDetectionDyn", std::to_string(info.cdDyn ? 1 : 0)));
				info.properties.insert(std::make_pair("StaticVob", std::to_string(info.staticVob ? 1 : 0)));
				info.properties.insert(std::make_pair("DynamicShadow", std::to_string(info.dynamicShadow ? 1 : 0)));
				info.properties.insert(std::make_pair("zBias", std::to_string(info.zBias)));
				info.properties.insert(std::make_pair("IsAmbient", std::to_string(info.isAmbient ? 1 : 0)));*/
			}
			else
			{
				ReadObjectProperties(parser, info.properties,
					Prop("PresetName", info.presetName));

				parser.getImpl()->readEntry("", info.bbox, sizeof(info.bbox), ZenLoad::ParserImpl::ZVT_RAW_FLOAT);
				parser.getImpl()->readEntry("", &info.rotationMatrix3x3, sizeof(info.rotationMatrix3x3), ZenLoad::ParserImpl::ZVT_RAW);

				parser.getImpl()->readEntry("", &info.position, sizeof(float) * 3, ZenLoad::ParserImpl::ZVT_VEC3);

				info.rotationMatrix = info.rotationMatrix3x3.toMatrix();

				if(version != WorldVersion::VERSION_G1_08k)
				{
					ReadObjectProperties(parser, info.properties,
										 Prop("vobName", info.vobName),
										 Prop("visual", info.visual),
										 Prop("showVisual", info.showVisual),
										 Prop("visualCamAlign", info.visualCamAlign),
										 Prop("visualAniMode", info.visualAniMode),
										 Prop("visualAniModeStrength", info.visualAniModeStrength),
										 Prop("vobFarClipZScale", info.vobFarClipScale),
										 Prop("cdStatic", info.cdStatic),
										 Prop("cdDyn", info.cdDyn),
										 Prop("staticVob", info.staticVob),
										 Prop("dynShadow", info.dynamicShadow),
										 Prop("zBias", info.zBias),
										 Prop("isAmbient", info.isAmbient)); // TODO: References!
				} else
				{
					ReadObjectProperties(parser, info.properties,
										 Prop("vobName", info.vobName),
										 Prop("visual", info.visual),
										 Prop("showVisual", info.showVisual),
										 Prop("visualCamAlign", info.visualCamAlign),
										 Prop("cdStatic", info.cdStatic),
										 Prop("cdDyn", info.cdDyn),
										 Prop("staticVob", info.staticVob),
										 Prop("dynShadow", info.dynamicShadow)); // TODO: References!
				}

				// Skip visual-chunk
				ZenParser::ChunkHeader tmph;
				parser.readChunkStart(tmph);
				parser.skipChunk();

				// Skip ai-chunk
				parser.readChunkStart(tmph);
				parser.skipChunk();

			}

			/*// Skip visual-chunk
			ZenParser::ChunkHeader tmph;
			parser.readChunkStart(tmph);
			parser.skipChunk();

			// Skip ai-chunk
			parser.readChunkStart(tmph);
			parser.skipChunk();*/

			// Check subclasses
			if(header.classname == "oCItem:zCVob")
			{
				parser.getImpl()->readEntry("itemInstance", &info.oCItem.instanceName);
			}

			if(header.classname.find("oCMOB:") != std::string::npos)
			{
				parser.getImpl()->readEntry("focusName", &info.oCMOB.focusName);
				parser.getImpl()->readEntry("hitpoints", &info.oCMOB.hitpoints);
				parser.getImpl()->readEntry("damage", &info.oCMOB.damage);
				parser.getImpl()->readEntry("moveable", &info.oCMOB.moveable);
				parser.getImpl()->readEntry("takeable", &info.oCMOB.takeable);
				parser.getImpl()->readEntry("focusOverride", &info.oCMOB.focusOverride);
				parser.getImpl()->readEntry("soundMaterial", &info.oCMOB.soundMaterial);
				parser.getImpl()->readEntry("visualDestroyed", &info.oCMOB.visualDestroyed);
				parser.getImpl()->readEntry("owner", &info.oCMOB.owner);
				parser.getImpl()->readEntry("ownerGuild", &info.oCMOB.ownerGuild);
				parser.getImpl()->readEntry("isDestroyed", &info.oCMOB.isDestroyed);
			}

			if(header.classname.find("oCMobInter:") != std::string::npos)
			{
				/*if(version == WorldVersion::VERSION_G1_08k)
				{
					int32_t tmp;
					parser.getImpl()->readEntry("state", &tmp);
					parser.getImpl()->readEntry("stateTarget", &tmp);
				}*/

				parser.getImpl()->readEntry("stateNum", &info.oCMobInter.stateNum);
				parser.getImpl()->readEntry("triggerTarget", &info.oCMobInter.triggerTarget);
				parser.getImpl()->readEntry("useWithItem", &info.oCMobInter.useWithItem);
				parser.getImpl()->readEntry("conditionFunc", &info.oCMobInter.conditionFunc);
				parser.getImpl()->readEntry("onStateFunc", &info.oCMobInter.onStateFunc);
				parser.getImpl()->readEntry("rewind", &info.oCMobInter.rewind);
			}

			parser.skipChunk();

			// Generate world-matrix
			info.worldMatrix = info.rotationMatrix3x3.toMatrix(info.position);
		}

	private:
	};
}
