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

    struct zCModelAniSample
    {
        ZMath::float4 rotation; // Quaternion
        ZMath::float3 position;
    };

    class zCModelAni
    {
    public:

        zCModelAni();

        void setScale(float scale) { m_Scale = scale; }

        /** Loads a model animation file (.MAN).
         *
         * You need to call either loadMSB() and loadMDH() afterwards or
         * call loadMDS() to load additional required data.
         *
         */
        bool loadMAN(const VDFS::FileIndex& fileIndex, const std::string& fileName);

        /** Loads a model hierarchy (.MDH) file.
         *
         */
        bool loadMDH(const std::string& fileName);

        /** Loads a compiled MDS (.MSB) file.
         *
         */
        bool loadMSB(const std::string& fileName);

        /** Loads a model script (.MDS) file.
         *
         */
        bool loadMDS(const std::string& fileName);

        /**
         * @return generic information about this animation
         */
        const zCModelAniHeader& getModelAniHeader() const { return m_ModelAniHeader; }

        /**
         * @return Animation-data. Access: sampleIdx * numNodes + node
         */
        const std::vector<zCModelAniSample>& getAniSamples() const { return m_AniSamples; }

        /**
         * @return Indices of the samples to the actual nodes
         */
        const std::vector<uint32_t>& getNodeIndexList() const { return m_NodeIndexList; }

        /**
         * @return Whether the animation was correctly loaded
         */
        bool isValid(){ return m_ModelAniHeader.version != 0; }

    private:

        float m_Scale = 1.0f;

        /**
         * @brief File information
         */
        zCModelAniHeader m_ModelAniHeader;

        /**
         * @brief Ani-Events
         */
        std::vector<zCModelAniEvent> m_AniEvents;

        std::vector<uint32_t> m_NodeIndexList;
        std::vector<zCModelAniSample> m_AniSamples;


        bool parse(const VDFS::FileIndex &fileIndex, const std::string &fileName, std::function<bool (ZenParser &)> fn);
    };
}
