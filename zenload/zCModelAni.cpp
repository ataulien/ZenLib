#include <algorithm>
#include <cmath>
#include <iomanip>
#include <string>

#include "utils/logger.h"
#include "vdfs/fileIndex.h"
#include "zCModelAni.h"
#include "zCProgMeshProto.h"
#include "zenParser.h"
#include "zTypes.h"

using namespace ZenLoad;

/*
static const uint16_t MSID_MESHSOFTSKIN = 0xE100;
static const uint16_t MSID_MESHSOFTSKIN_END = 0xE110;

static const uint16_t MSID_MODELANI = 0xA000;
*/

static const uint16_t MSID_MAN_HEADER = 0xA020;
static const uint16_t MSID_MAN_SOURCE = 0xA010;
static const uint16_t MSID_MAN_ANIEVENTS = 0xA030;
static const uint16_t MSID_MAN_RAWDATA = 0xA090;

static const float SAMPLE_ROT_BITS			= float(1 << 16) - 1.0f;
static const float SAMPLE_ROT_SCALER		= (float(1.0f) / SAMPLE_ROT_BITS) * 2.0f * ZMath::Pi;
static const float SAMPLE_QUAT_SCALER		= (1.0f / SAMPLE_ROT_BITS) * 2.1f;
static const uint16_t SAMPLE_QUAT_MIDDLE      = (1 << 15) - 1;

void zCModelAniEvent::load(ZenParser& parser)
{
    aniEventType = static_cast<zTMdl_AniEventType>(parser.readBinaryDWord());
    frameNr = parser.readBinaryDWord();
    tagString = parser.readLine(true);

    for (int i=0; i<ANIEVENT_MAXSTRING; i++)
        string[i] = parser.readLine(true);

    for (int i=0; i<ANIEVENT_MAXSTRING; i++)
        values[i] = parser.readBinaryFloat();

    prob = parser.readBinaryFloat();
}

void SampleUnpackTrans(const uint16_t* in, ZMath::float3& out, float samplePosScaler, float samplePosRangeMin)
{
    out.x = float(in[0]) * samplePosScaler + samplePosRangeMin;
    out.y = float(in[1]) * samplePosScaler + samplePosRangeMin;
    out.z = float(in[2]) * samplePosScaler + samplePosRangeMin;
};

void SampleUnpackQuat(const uint16_t* in, ZMath::float4& out)
{
    out.x = (int(in[0]) - SAMPLE_QUAT_MIDDLE) * SAMPLE_QUAT_SCALER;
    out.y = (int(in[1]) - SAMPLE_QUAT_MIDDLE) * SAMPLE_QUAT_SCALER;
    out.z = (int(in[2]) - SAMPLE_QUAT_MIDDLE) * SAMPLE_QUAT_SCALER;

    float len_q = out.x * out.x + out.y * out.y + out.z * out.z;

    if (len_q > 1.0f)
    {
        float l = 1.0f / sqrt(len_q);
        out.x *= l;
        out.y *= l;
        out.z *= l;
        out.w = 0;
    }
    else
    {
        out.w = sqrt(1.0f - len_q);
    }
}

zCModelAni::zCModelAni()
{

}

bool zCModelAni::loadMAN(const VDFS::FileIndex &index, const std::string &fileName)
{
    m_ModelAniHeader.version = 0;

    bool ret = parse(index, fileName, [&](ZenParser &parser) -> bool {

        // Information about a single chunk
        BinaryChunkInfo chunkInfo;

        // Read chunks until we left the virtual binary file or got to the end-chunk
        // Each chunk starts with a header (BinaryChunkInfo) which gives information
        // about what to do and how long the chunk is
        while (parser.getSeek() < parser.getFileSize())
        {
            // Read chunk header and calculate position of next chunk
            parser.readStructure(chunkInfo);

            size_t chunkEnd = parser.getSeek() + chunkInfo.length;

            switch(chunkInfo.id)
            {
            case MSID_MAN_HEADER:
                m_ModelAniHeader.version = parser.readBinaryWord();

                m_ModelAniHeader.aniName = parser.readLine(true);

                m_ModelAniHeader.layer = parser.readBinaryDWord();
                m_ModelAniHeader.numFrames = parser.readBinaryDWord();
                m_ModelAniHeader.numNodes = parser.readBinaryDWord();
                m_ModelAniHeader.fpsRate = parser.readBinaryFloat();
                m_ModelAniHeader.fpsRateSource = parser.readBinaryFloat();
                m_ModelAniHeader.samplePosRangeMin = parser.readBinaryFloat();
                m_ModelAniHeader.samplePosScaler = parser.readBinaryFloat();

                parser.readBinaryRaw(m_ModelAniHeader.aniBBox, sizeof(m_ModelAniHeader.aniBBox));

                m_ModelAniHeader.nextAniName = parser.readLine(true);
                break;

            case MSID_MAN_SOURCE:
                parser.setSeek(chunkEnd); // Skip chunk
                break;

            case MSID_MAN_ANIEVENTS: {
                uint32_t numAniEvents = parser.readBinaryDWord();
                m_AniEvents.resize(numAniEvents);

                for(int i = 0; i < numAniEvents; i++)
                    m_AniEvents[i].load(parser);
                }
                break;

            case MSID_MAN_RAWDATA: {
                    m_ModelAniHeader.nodeChecksum = parser.readBinaryDWord();

                    m_NodeIndexList.resize(m_ModelAniHeader.numNodes);
                    parser.readBinaryRaw(m_NodeIndexList.data(), m_NodeIndexList.size() * sizeof(uint32_t));

                    uint32_t numSamples = m_ModelAniHeader.numNodes * m_ModelAniHeader.numFrames;
                    zTMdl_AniSample* qSamples = new zTMdl_AniSample[numSamples];
                    m_AniSamples.resize(numSamples);


                    parser.readBinaryRaw(qSamples, sizeof(zTMdl_AniSample) * numSamples);

                    for(size_t i = 0; i < numSamples; i++)
                    {
                        SampleUnpackTrans(qSamples[i].position, m_AniSamples[i].position, m_ModelAniHeader.samplePosScaler, m_ModelAniHeader.samplePosRangeMin);
                        SampleUnpackQuat(qSamples[i].rotation, m_AniSamples[i].rotation);
                    }

                    delete[] qSamples;
                }
                break;
            default:
                parser.setSeek(chunkEnd); // Skip chunk

            }
        }
        return true;
    });

    if (!ret)
        return false;

    // Apply scale
    if (m_Scale != 1.0f)
    {
        for(size_t i = 0, end = m_AniSamples.size(); i < end; i++)
        {
            m_AniSamples[i].position = m_AniSamples[i].position * m_Scale;
        }
    }

    return true;
}

bool zCModelAni::loadMSB(const VDFS::FileIndex &index, const std::string &fileName)
{
    LogInfo() << "load MSB " << fileName;

    return parse(index, fileName, [&](ZenParser &parser) -> bool {
#if 0
        // Information about a single chunk
        BinaryChunkInfo chunkInfo;

        // Read chunks until we left the virtual binary file or got to the end-chunk
        // Each chunk starts with a header (BinaryChunkInfo) which gives information
        // about what to do and how long the chunk is
        while (parser.getSeek() < parser.getFileSize())
        {
            // Read chunk header and calculate position of next chunk
            parser.readStructure(chunkInfo);

            //LogInfo() << "chunk " << std::hex << std::setw(4) << std::setfill('0') << chunkInfo.id;

            size_t chunkEnd = parser.getSeek() + chunkInfo.length;
            switch(chunkInfo.id)
            {
            case MSB_MODEL:
                LogInfo() << "MSB model " << parser.getSeek();
                break;

            case MSB_ANI:
                {
                    std::string name = parser.readLine(true);
                    uint32_t layer = parser.readBinaryDWord();
                    std::string next = parser.readLine(true);
                    float blendIn = parser.readBinaryFloat();
                    float blendOut = parser.readBinaryFloat();
                    std::string flags = parser.readLine(true);
                    std::string asc = parser.readLine(true);
                    std::string dir = parser.readLine(true);
                    uint32_t firstFrame = parser.readBinaryDWord();
                    uint32_t lastFrame = parser.readBinaryDWord();
                    float maxFps = parser.readBinaryFloat();
                    float speed = parser.readBinaryFloat();
                    float collVolScale = parser.readBinaryFloat();

#if 1
                    LogInfo() << "MSB ani"
                              << " name: '" << name << "'"
                              << " layer: " << layer
                              << " next: " << next
                              << " blend in: " << blendIn
                              << " blend out: " << blendOut
                              << " flags: " << flags
                              << " dir: " << dir
                              << " asc: " << asc
                              << " first frame: " << firstFrame
                              << " last frame: " << lastFrame
                              << " max FPS: " << maxFps
                              << " max speed: " << speed
                              << " max collision volume scale: " << collVolScale
                              ;
#endif
                }
                break;

            case MSB_ANI_ALIAS:
                {
                    std::string name = parser.readLine(true);
                    uint32_t layer = parser.readBinaryDWord();
                    std::string next = parser.readLine(true);
                    float blendIn = parser.readBinaryFloat();
                    float blendOut = parser.readBinaryFloat();
                    std::string flags = parser.readLine(true);
                    std::string alias = parser.readLine(true);
                    std::string dir = parser.readLine(true);

                    m_ModelAniHeader.aniName = name;
                    m_ModelAniHeader.alias = alias;
                    m_ModelAniHeader.layer = layer;
                    m_ModelAniHeader.nextAniName = next;
                    m_ModelAniHeader.blendIn = blendIn;
                    m_ModelAniHeader.blendOut = blendOut;
                    m_ModelAniHeader.flags = flags;
                    m_ModelAniHeader.alias = alias;
                    m_ModelAniHeader.dir = dir;

#if 1
                    LogInfo() << "MSB ani alias"
                              << " name: '" << name << "'"
                              << " layer: " << layer
                              << " next: " << next
                              << " blend in: " << blendIn
                              << " blend out: " << blendOut
                              << " flags: " << flags
                              << " alias: " << alias
                              << " dir: " << dir
                              ;
#endif
                }
                break;
#if 0

            case MSID_MAN_HEADER:
                m_ModelAniHeader.version = parser.readBinaryWord();

                m_ModelAniHeader.aniName = parser.readLine(true);

                m_ModelAniHeader.layer = parser.readBinaryDWord();
                m_ModelAniHeader.numFrames = parser.readBinaryDWord();
                m_ModelAniHeader.numNodes = parser.readBinaryDWord();
                m_ModelAniHeader.fpsRate = parser.readBinaryFloat();
                m_ModelAniHeader.fpsRateSource = parser.readBinaryFloat();
                m_ModelAniHeader.samplePosRangeMin = parser.readBinaryFloat();
                m_ModelAniHeader.samplePosScaler = parser.readBinaryFloat();

                parser.readBinaryRaw(m_ModelAniHeader.aniBBox, sizeof(m_ModelAniHeader.aniBBox));

                m_ModelAniHeader.nextAniName = parser.readLine(true);
                break;

            case MSID_MAN_SOURCE:
                parser.setSeek(chunkEnd); // Skip chunk
                break;

            case MSID_MAN_ANIEVENTS: {
                uint32_t numAniEvents = parser.readBinaryDWord();
                m_AniEvents.resize(numAniEvents);

                for(int i = 0; i < numAniEvents; i++)
                    m_AniEvents[i].load(parser);
                }
                break;

            case MSID_MAN_RAWDATA: {
                    m_ModelAniHeader.nodeChecksum = parser.readBinaryDWord();

                    m_NodeIndexList.resize(m_ModelAniHeader.numNodes);
                    parser.readBinaryRaw(m_NodeIndexList.data(), m_NodeIndexList.size() * sizeof(uint32_t));

                    uint32_t numSamples = m_ModelAniHeader.numNodes * m_ModelAniHeader.numFrames;
                    zTMdl_AniSample* qSamples = new zTMdl_AniSample[numSamples];
                    m_AniSamples.resize(numSamples);


                    parser.readBinaryRaw(qSamples, sizeof(zTMdl_AniSample) * numSamples);

                    for(size_t i = 0; i < numSamples; i++)
                    {
                        SampleUnpackTrans(qSamples[i].position, m_AniSamples[i].position, m_ModelAniHeader.samplePosScaler, m_ModelAniHeader.samplePosRangeMin);
                        SampleUnpackQuat(qSamples[i].rotation, m_AniSamples[i].rotation);
                    }

                    delete[] qSamples;
                }
                break;
#endif
            default:
                parser.setSeek(chunkEnd); // Skip chunk
            }
        }
#endif
        return true;
    });
}

bool zCModelAni::loadMDS(const VDFS::FileIndex &index, const std::string &fileName)
{
    LogError() << "not implemented, skipped loading " << fileName;
    return false;
}

bool zCModelAni::parse(const VDFS::FileIndex& fileIndex, const std::string &fileName, std::function<bool (ZenParser &)> fn)
{
    bool ret = true;
    try
    {
        std::string ucname = fileName;
        std::transform(ucname.begin(), ucname.end(), ucname.begin(), ::toupper);

        std::vector<uint8_t> data;
        fileIndex.getFileData(ucname, data);

        if(data.empty())
        {
            //LogError() << "file " << fileName << " does not exist";
            return false;
        }

        // Create parser from memory
        // FIXME: There is an internal copy of the data here. Optimize!
        ZenLoad::ZenParser parser(data.data(), data.size());
        ret = fn(parser);
        return true;
    }
    catch(std::exception &e)
    {
        LogError() << "parsing error" << e.what();
        return false;
    }

    return ret;
}
