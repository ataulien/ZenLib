#pragma once
#include <functional>
#include <string>
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

    enum
    {
        ANIEVENT_MAXSTRING = 4
    };

    struct zCModelAniHeader
    {
        uint16_t version;

        std::string aniName;

        uint32_t layer;
        uint32_t numFrames;
        uint32_t numNodes;
        float fpsRate;
        float fpsRateSource;
        float samplePosRangeMin;
        float samplePosScaler;

        ZMath::float3 aniBBox[2];

        std::string nextAniName;

        uint32_t nodeChecksum;
    };

    struct zCModelAniSample
    {
        ZMath::float4 rotation; // Quaternion
        ZMath::float3 position;
    };
}
