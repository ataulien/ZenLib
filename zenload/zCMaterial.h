#pragma once
#include "zTypes.h"
#include "zenParser.h"
#include "zenParserPropRead.h"

namespace ZenLoad
{
	class zCMaterial
	{
	public:
		/**
		 * Reads this object from an internal zen
		 */
		static zCMaterialData readObjectData(ZenParser& parser)
		{
			// Read everything the material has to offer
			zCMaterialData materialInfo;
			
			materialInfo.objectClass = "zCMaterial";
			
			ReadObjectProperties(parser,				materialInfo.properties,
				Prop("MaterialName",					materialInfo.matName),
				Prop("MaterialGroup",					materialInfo.matGroup),
				Prop("Color",							materialInfo.color),
				Prop("SmoothAngle",						materialInfo.smoothAngle),
				Prop("Texture",							materialInfo.texture),
				Prop("TextureScale",					materialInfo.texScale),
				Prop("TextureAniFPS",					materialInfo.texAniFPS),
				Prop("TextureAniMapMode",				materialInfo.texAniMapMode),
				Prop("TextureAniMapDir",				materialInfo.texAniMapDir),
				Prop("NoCollisionDetection",			materialInfo.noCollDet),
				Prop("NoLightmap",						materialInfo.noLighmap),
				Prop("LoadDontCollapse",				materialInfo.loadDontCollapse),
				Prop("DetailObject",					materialInfo.detailObject),
				Prop("DetailTextureScale",				materialInfo.detailTextureScale),
				Prop("ForceOccluder",					materialInfo.forceOccluder),
				Prop("EnvironmentMapping",				materialInfo.environmentMapping),
				Prop("EnvironmentalMappingStrength",	materialInfo.environmentalMappingStrength),
				Prop("WaveMode",						materialInfo.waveMode),
				Prop("WaveSpeed",						materialInfo.waveSpeed),
				Prop("WaveMaxAmplitude",				materialInfo.waveMaxAmplitude),
				Prop("WaveGridSize",					materialInfo.waveGridSize),
				Prop("IgnoreSun",						materialInfo.ignoreSun),
				Prop("AlphaFunc",						materialInfo.alphaFunc),
				Prop("DefaultMapping",					materialInfo.defaultMapping));

			return materialInfo;
		}

	private:
	};

}
