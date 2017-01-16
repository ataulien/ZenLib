#include <cmath>

#include "modelAnimationParser.h"
#include "zenParser.h"

namespace ZenLoad
{

static const float SAMPLE_ROT_BITS			= float(1 << 16) - 1.0f;
static const float SAMPLE_ROT_SCALER		= (float(1.0f) / SAMPLE_ROT_BITS) * 2.0f * ZMath::Pi;
static const float SAMPLE_QUAT_SCALER		= (1.0f / SAMPLE_ROT_BITS) * 2.1f;
static const uint16_t SAMPLE_QUAT_MIDDLE      = (1 << 15) - 1;

ModelAnimationParser::ModelAnimationParser(ZenParser &zen)
    : m_Zen(zen)
{

}

ModelAnimationParser::EChunkType ModelAnimationParser::parse()
{
    if (m_Zen.getSeek() >= m_Zen.getFileSize())
        return CHUNK_EOF;

    BinaryChunkInfo chunkInfo;
    m_Zen.readStructure(chunkInfo);

    // store position after header so that we can skip the chunk
    size_t chunkEnd = m_Zen.getSeek() + chunkInfo.length;

    switch(chunkInfo.id)
    {
    case CHUNK_HEADER:
        readHeader();
        return CHUNK_HEADER;
    case CHUNK_RAWDATA:
        readRawData();
        return CHUNK_RAWDATA;
    default:
        m_Zen.setSeek(chunkEnd);
        return parse(); // skip unknown chunk
    }

    return CHUNK_EOF;
}

void ModelAnimationParser::readHeader()
{
    m_Header.version = m_Zen.readBinaryWord();

    m_Header.aniName = m_Zen.readLine(true);

    m_Header.layer = m_Zen.readBinaryDWord();
    m_Header.numFrames = m_Zen.readBinaryDWord();
    m_Header.numNodes = m_Zen.readBinaryDWord();
    m_Header.fpsRate = m_Zen.readBinaryFloat();
    m_Header.fpsRateSource = m_Zen.readBinaryFloat();
    m_Header.samplePosRangeMin = m_Zen.readBinaryFloat();
    m_Header.samplePosScaler = m_Zen.readBinaryFloat();
    m_Zen.readBinaryRaw(m_Header.aniBBox, sizeof(m_Header.aniBBox));
    m_Header.nextAniName = m_Zen.readLine(true);
}

static void SampleUnpackTrans(const uint16_t* in, ZMath::float3& out, float samplePosScaler, float samplePosRangeMin)
{
    out.x = float(in[0]) * samplePosScaler + samplePosRangeMin;
    out.y = float(in[1]) * samplePosScaler + samplePosRangeMin;
    out.z = float(in[2]) * samplePosScaler + samplePosRangeMin;
}

static void SampleUnpackQuat(const uint16_t* in, ZMath::float4& out)
{
    out.x = (int(in[0]) - SAMPLE_QUAT_MIDDLE) * SAMPLE_QUAT_SCALER;
    out.y = (int(in[1]) - SAMPLE_QUAT_MIDDLE) * SAMPLE_QUAT_SCALER;
    out.z = (int(in[2]) - SAMPLE_QUAT_MIDDLE) * SAMPLE_QUAT_SCALER;

    float len_q = out.x * out.x + out.y * out.y + out.z * out.z;

    if (len_q > 1.0f)
    {
        float l = 1.0f / sqrtf(len_q);
        out.x *= l;
        out.y *= l;
        out.z *= l;
        out.w = 0;
    }
    else
    {
        out.w = sqrtf(1.0f - len_q);
    }
}

void ModelAnimationParser::readRawData()
{
    m_Header.nodeChecksum = m_Zen.readBinaryDWord();

    m_NodeIndex.resize(m_Header.numNodes);
    m_Zen.readBinaryRaw(m_NodeIndex.data(), m_NodeIndex.size() * sizeof(uint32_t));

    uint32_t numSamples = m_Header.numNodes * m_Header.numFrames;
    m_Samples.resize(numSamples);

    for(size_t i = 0; i < numSamples; i++)
    {
        zTMdl_AniSample sample;
        m_Zen.readBinaryRaw(&sample, sizeof(zTMdl_AniSample));
        SampleUnpackTrans(sample.position, m_Samples[i].position, m_Header.samplePosScaler, m_Header.samplePosRangeMin);
        m_Samples[i].position *= m_Scale;
        SampleUnpackQuat(sample.rotation, m_Samples[i].rotation);
    }
}

} // namespace ZenLoad
