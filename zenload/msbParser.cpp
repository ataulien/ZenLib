#include <cmath>

#include "msbParser.h"
#include "zenParser.h"

namespace ZenLoad
{

MsbParser::MsbParser(ZenParser &zen)
    : m_Zen(zen)
{

}

MsbParser::EChunkType MsbParser::parse()
{
    if (m_Zen.getSeek() >= m_Zen.getFileSize())
        return CHUNK_EOF;

    BinaryChunkInfo chunkInfo;
    m_Zen.readStructure(chunkInfo);

    // store position after header so that we can skip the chunk
    size_t chunkEnd = m_Zen.getSeek() + chunkInfo.length;

    switch(chunkInfo.id)
    {
    case CHUNK_ANI:
        readAni();
        return CHUNK_ANI;
    case CHUNK_ANI_ALIAS:
        readAlias();
        return CHUNK_ANI_ALIAS;
    case CHUNK_EVENT_SFX:
        readSfx();
        return CHUNK_EVENT_SFX;
    default:
        m_Zen.setSeek(chunkEnd);
        return parse(); // skip unknown chunk
    }

    return CHUNK_EOF;
}

static uint32_t makeAniFlags(ZenParser &zen)
{
    uint32_t flags = 0;
    std::string flag_str = zen.readString();
    for (auto ch : flag_str)
    {
        switch (ch)
        {
        case 'M':
            flags |= MSB_MOVE_MODEL;
            break;
        case 'R':
            flags |= MSB_ROTATE_MODEL;
            break;
        case 'E':
            flags |= MSB_QUEUE_ANI;
            break;
        case 'F':
            flags |= MSB_FLY;
            break;
        case 'I':
            flags |= MSB_IDLE;
            break;
        }
    }
    return flags;
}

static EMsbAniDir makeAniDir(ZenParser &zen)
{
    std::string str = zen.readString();
    return (!str.empty() && str[0] == 'R') ? MSB_BACKWARD : MSB_FORWARD;
}

void MsbParser::readAni()
{
    m_Ani.m_Name = m_Zen.readLine(true);
    m_Ani.m_Layer = m_Zen.readBinaryDWord();
    m_Ani.m_Next = m_Zen.readLine(true);
    m_Ani.m_BlendIn = m_Zen.readBinaryFloat();
    m_Ani.m_BlendOut = m_Zen.readBinaryFloat();
    m_Ani.m_Flags = makeAniFlags(m_Zen);
    m_Ani.m_Asc = m_Zen.readLine(true);
    m_Ani.m_Dir = makeAniDir(m_Zen);
    m_Ani.m_FirstFrame = m_Zen.readBinaryDWord();
    m_Ani.m_LastFrame = m_Zen.readBinaryDWord();
    m_Ani.m_MaxFps = m_Zen.readBinaryFloat();
    m_Ani.m_Speed = m_Zen.readBinaryFloat();
    m_Ani.m_ColVolScale = m_Zen.readBinaryFloat();
}

void MsbParser::readAlias()
{
    m_Alias.m_Name = m_Zen.readLine(true);
    m_Alias.m_Layer = m_Zen.readBinaryDWord();
    m_Alias.m_Next = m_Zen.readLine(true);
    m_Alias.m_BlendIn = m_Zen.readBinaryFloat();
    m_Alias.m_BlendOut = m_Zen.readBinaryFloat();
    m_Alias.m_Flags = makeAniFlags(m_Zen);
    m_Alias.m_Alias = m_Zen.readLine(true);
    m_Ani.m_Dir = makeAniDir(m_Zen);
}

void MsbParser::readSfx()
{
    m_Sfx.m_Frame = m_Zen.readBinaryDWord();
    m_Sfx.m_Name = m_Zen.readLine(true);
    m_Sfx.m_Value1 = m_Zen.readLine(true);
    m_Sfx.m_Value2 = m_Zen.readLine(true);
}

} // namespace ZenLoad
