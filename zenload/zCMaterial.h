#pragma once

#include "zTypes.h"
#include "zenParser.h"
#include "zenParserPropRead.h"

namespace ZenLoad
{
    enum class MaterialGroup : int {
        UNDEF	= 0	,
        METAL,
        STONE,
        WOOD,
        EARTH,
        WATER,
        SNOW,
        NUM_MAT_GROUPS
    };

    static const std::string MaterialGroupNames[(int)MaterialGroup::NUM_MAT_GROUPS] = {
            "UNDEF",
            "METAL",
            "STONE",
            "WOOD",
            "EARTH",
            "WATER",
            "SNOW",
    };

    class zCMaterial
    {
    public:

        /**
         * Converts the given material-group enum value to the regarding string
         */
        static std::string getMatGroupString(MaterialGroup group)
        {
            if((int)group >= (int)MaterialGroup::NUM_MAT_GROUPS)
            {
                return MaterialGroupNames[(int)MaterialGroup::UNDEF];
            }

            return MaterialGroupNames[(int)group];
        }

        /**
         * Reads this object from an internal zen
         */
        static zCMaterialData readObjectData(ZenParser& parser, uint16_t version = 0)
        {
            // Read everything the material has to offer
            zCMaterialData materialInfo;

            materialInfo.objectClass = "zCMaterial";

            if (version != 39939) // Gothic 1
            {
                ReadObjectProperties(parser, materialInfo.properties,
                                     Prop("MaterialName", materialInfo.matName),
                                     Prop("MaterialGroup", materialInfo.matGroup),
                                     Prop("Color", materialInfo.color),
                                     Prop("SmoothAngle", materialInfo.smoothAngle),
                                     Prop("Texture", materialInfo.texture),
                                     Prop("TextureScale", materialInfo.texScale),
                                     Prop("TextureAniFPS", materialInfo.texAniFPS),
                                     Prop("TextureAniMapMode", materialInfo.texAniMapMode),
                                     Prop("TextureAniMapDir", materialInfo.texAniMapDir),
                                     Prop("NoCollisionDetection", materialInfo.noCollDet),
                                     Prop("NoLightmap", materialInfo.noLighmap),
                                     Prop("LoadDontCollapse", materialInfo.loadDontCollapse),
                                     Prop("DetailObject", materialInfo.detailObject),
                                     Prop("DefaultMapping", materialInfo.defaultMapping));
            } else
            {
                ReadObjectProperties(parser, materialInfo.properties,
                                     Prop("MaterialName", materialInfo.matName),
                                     Prop("MaterialGroup", materialInfo.matGroup),
                                     Prop("Color", materialInfo.color),
                                     Prop("SmoothAngle", materialInfo.smoothAngle),
                                     Prop("Texture", materialInfo.texture),
                                     Prop("TextureScale", materialInfo.texScale),
                                     Prop("TextureAniFPS", materialInfo.texAniFPS),
                                     Prop("TextureAniMapMode", materialInfo.texAniMapMode),
                                     Prop("TextureAniMapDir", materialInfo.texAniMapDir),
                                     Prop("NoCollisionDetection", materialInfo.noCollDet),
                                     Prop("NoLightmap", materialInfo.noLighmap),
                                     Prop("LoadDontCollapse", materialInfo.loadDontCollapse),
                                     Prop("DetailObject", materialInfo.detailObject),
                                     Prop("DetailTextureScale", materialInfo.detailTextureScale),
                                     Prop("ForceOccluder", materialInfo.forceOccluder),
                                     Prop("EnvironmentMapping", materialInfo.environmentMapping),
                                     Prop("EnvironmentalMappingStrength", materialInfo.environmentalMappingStrength),
                                     Prop("WaveMode", materialInfo.waveMode),
                                     Prop("WaveSpeed", materialInfo.waveSpeed),
                                     Prop("WaveMaxAmplitude", materialInfo.waveMaxAmplitude),
                                     Prop("WaveGridSize", materialInfo.waveGridSize),
                                     Prop("IgnoreSun", materialInfo.ignoreSun),
                                     Prop("AlphaFunc", materialInfo.alphaFunc),
                                     Prop("DefaultMapping", materialInfo.defaultMapping));

            }


            return materialInfo;
        }

    private:
    };

}
