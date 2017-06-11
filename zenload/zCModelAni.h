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

// FIXME: COMPATIBILITY FOR MASTER - REMOVE LATER!

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
    class zCModelAni
    {
    public:

        struct zCModelAniEvent
        {
            enum { ANIEVENT_MAXSTRING = 4 };

            zTMdl_AniEventType aniEventType;
            uint32_t frameNr;
            std::string tagString;
            std::string string[ANIEVENT_MAXSTRING];
            float values[ANIEVENT_MAXSTRING];
            float prob;

            void load(ZenParser& parser);
        };

        struct ModelAniHeader
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

        struct AniSample
        {
            ZMath::float4 rotation; // Quaternion
            ZMath::float3 position;
        };

        zCModelAni(){}

        /**
        * @brief Loads the mesh from the given VDF-Archive
        */
        zCModelAni(const std::string& fileName, const VDFS::FileIndex& fileIndex, float scale = 1.0f);

        /**
         * @brief Reads the mesh-object from the given binary stream
         * @param fromZen Whether this mesh is supposed to be read from a zenfile. In this case, information about the binary chunk is also read.
         */
        void readObjectData(ZenParser& parser);

        /**
         * @return generic information about this animation
         */
        const ModelAniHeader& getModelAniHeader() const { return m_ModelAniHeader; }

        /**
         * @return Animation-data. Access: sampleIdx * numNodes + node
         */
        const std::vector<AniSample>& getAniSamples() const { return m_AniSamples; }

        /**
         * @return Indices of the samples to the actual nodes
         */
        const std::vector<uint32_t>& getNodeIndexList() const { return m_NodeIndexList; }

        /**
         * @return Whether the animation was correctly loaded
         */
        bool isValid(){ return m_ModelAniHeader.version != 0; }

    private:
        /**
         * @brief File information
         */
        ModelAniHeader m_ModelAniHeader;

        /**
         * @brief Ani-Events
         */
        std::vector<zCModelAniEvent> m_AniEvents;

        std::vector<uint32_t> m_NodeIndexList;
        std::vector<AniSample> m_AniSamples;
    };
}