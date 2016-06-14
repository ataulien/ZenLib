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

	public:
		/**
		* Reads this object from an internal zen
		*/
		static zCVobData readObjectData(ZenParser& parser)
		{
			zCVobData info;

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
	
				info.properties.insert(std::make_pair("PresetName", info.presetName));
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
				info.properties.insert(std::make_pair("IsAmbient", std::to_string(info.isAmbient ? 1 : 0)));
			}
			else
			{
				ReadObjectProperties(parser, info.properties,
					Prop("PresetName", info.presetName));

				parser.getImpl()->readEntry("", info.bbox, sizeof(float) * 6, ZenLoad::ParserImpl::ZVT_RAW_FLOAT);
				parser.getImpl()->readEntry("", &info.rotationMatrix3x3, sizeof(float) * 9, ZenLoad::ParserImpl::ZVT_RAW);
				
				ReadObjectProperties(parser, info.properties,
					Prop("Position", info.position),
					Prop("VobName", info.vobName),
					Prop("VisualName", info.visual),
					Prop("ShowVisual", info.showVisual),
					Prop("VisualCamAlign", info.visualCamAlign),
					Prop("VisualAniMode", info.visualAniMode),
					Prop("VisualAniModeStrength", info.visualAniModeStrength),
					Prop("VobFarClipScale", info.vobFarClipScale),
					Prop("CollisionDetectionStatic", info.cdStatic),
					Prop("CollisionDetectionDyn", info.cdDyn),
					Prop("StaticVob", info.staticVob),
					Prop("DynamicShadow", info.dynamicShadow),
					Prop("zBias", info.zBias),
					Prop("IsAmbient", info.isAmbient)); // TODO: References!
			}
			parser.skipChunk();

			// Generate world-matrix
			info.worldMatrix = info.rotationMatrix3x3.toMatrix(info.position);

			return info;
		}

	private:
	};
}
