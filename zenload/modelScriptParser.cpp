#include <cmath>

#include <utils/logger.h>

#include "modelScriptParser.h"
#include "zenParser.h"

namespace ZenLoad
{

ModelScriptParser::ModelScriptParser(ZenParser &zen)
    : m_Zen(zen)
{

}

ModelScriptParser::~ModelScriptParser()
{

}

ModelScriptBinParser::ModelScriptBinParser(ZenParser &zen)
    : ModelScriptParser(zen)
{

}

ModelScriptBinParser::EChunkType ModelScriptBinParser::parse()
{
    if (m_Zen.getSeek() >= m_Zen.getFileSize())
        return CHUNK_EOF;

    BinaryChunkInfo chunk;
    m_Zen.readStructure(chunk);

    // store position after header so that we can skip the chunk
    size_t chunk_end = m_Zen.getSeek() + chunk.length;

    switch(chunk.id)
    {
    case CHUNK_ANI:
        readAni();
        m_Zen.setSeek(chunk_end);
        return CHUNK_ANI;
    case CHUNK_ANI_ALIAS:
        readAlias();
        m_Zen.setSeek(chunk_end);
        return CHUNK_ANI_ALIAS;
    case CHUNK_EVENT_SFX:
        readSfx();
        m_Zen.setSeek(chunk_end);
        return CHUNK_EVENT_SFX;
    default:
        m_Zen.setSeek(chunk_end);
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

static EModelScriptAniDir makeAniDir(ZenParser &zen)
{
    std::string str = zen.readString();
    return (!str.empty() && str[0] == 'R') ? MSB_BACKWARD : MSB_FORWARD;
}

void ModelScriptBinParser::readAni()
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

void ModelScriptBinParser::readAlias()
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

void ModelScriptBinParser::readSfx()
{
    m_Sfx.m_Frame = m_Zen.readBinaryDWord();
    m_Sfx.m_Name = m_Zen.readLine(true);
    m_Sfx.m_Value1 = m_Zen.readLine(true);
    m_Sfx.m_Value2 = m_Zen.readLine(true);
}

ModelScriptTextParser::ModelScriptTextParser(ZenParser &zen)
    : ModelScriptParser(zen),
      m_Line(1)
{    
    // read current token into next
    token();

    m_Context.push_back(ContextFile);

    LogInfo() << "MDS\n" << reinterpret_cast<const char*>(&zen.getData()[0]);
}

bool ModelScriptTextParser::isEof() const
{
    return m_Zen.getSeek() >= m_Zen.getFileSize();
}

ModelScriptTextParser::Result ModelScriptTextParser::token()
{
    Result res = skipSpace();
    if (res != Success)
        return res;

    m_Token = m_NextToken;
    m_NextToken.text.clear();
    m_NextToken.line = m_Line;

    while (!isEof())
    {
        char ch = m_Zen.readBinaryByte();
        if (ch == '"')
        {
            while (true)
            {
                if (isEof())
                {
                    LogError() << "parser error in line " << m_Line << ": unexpected end in quoted string";
                    return Error;
                }

                ch = m_Zen.readBinaryByte();
                if (ch == '"')
                    break;

                if (ch == '\n')
                    m_Line++;

                m_NextToken.text.push_back(ch);
            }

            continue;

        } else
        if (ch == '/' && (m_Zen.getSeek() < m_Zen.getFileSize() - 1) && (m_Zen.getData()[m_Zen.getSeek()] == '/'))
        {
            while (true)
            {
                while (m_Zen.readBinaryByte() != '\n')
                {
                    if (isEof())
                        return End;
                }
            }
        } else
        if (isspace(ch) || ch == '(' || ch == ')' || ch == '{' || ch == '}')
        {
            if (m_NextToken.text.empty())
            {
                if (!isspace(ch))
                    m_NextToken.text.push_back(ch);

            } else
            {
                // we already have token text, the next token will be the special char
                m_Zen.setSeek(m_Zen.getSeek() - 1);
            }
            break;
        }

        m_NextToken.text.push_back(toupper(ch));
    }

    return m_NextToken.text.empty() ? End : Success;
}

ModelScriptTextParser::Result ModelScriptTextParser::skipSpace()
{
    while (isspace(m_Zen.getData()[m_Zen.getSeek()]))
    {
        if (m_Zen.getData()[m_Zen.getSeek()] == '\n')
            m_Line++;

        if (isEof())
            return End;

        m_Zen.readBinaryByte();
    }

    return Success;
}

ModelScriptTextParser::Result ModelScriptTextParser::expectChar(char ch)
{
    Result res = token();
    if (res != Success)
    {
        LogError() << "unexpected end in line " << m_Line << ", expected " << ch;
        return Error;
    }

    if (m_Token.text.empty() || m_Token.text[0] != ch)
    {
        LogError() << "invalid token '" << m_Token.text << "' in line " << m_Line << ", expected " << ch;
        return Error;
    }

    return Success;
}

bool ModelScriptTextParser::nextIs(char ch) const
{
    return !m_NextToken.text.empty() && m_NextToken.text[0] == ch;
}

ModelScriptTextParser::Result ModelScriptTextParser::parseObjectStart()
{
    return expectChar('{');
}

ModelScriptTextParser::Result ModelScriptTextParser::parseObjectEnd()
{
    return expectChar('}');
}

ModelScriptBinParser::EChunkType ModelScriptTextParser::parse()
{
    if (isEof())
        return CHUNK_EOF;

    if (!m_Token.text.empty() && m_Token.text[0] == '}')
    {
        // skip '}'
        if (!token())
            return CHUNK_ERROR;

        if (m_Context.empty())
        {
            LogError() << "unexpected '}' at line " << m_Line;
            return CHUNK_ERROR;
        }

        m_Context.pop_back();

        return parse();
    }

    // token is consumed by the functions below
    Result res = token();
    if (res != Success)
        return CHUNK_ERROR;

    switch (m_Context.back())
    {
    case ContextFile:
        return parseFileChunk();
    case ContextModel:
        return parseModelChunk();
    case ContextAniEnum:
        return parseAniEnumChunk();
    }

    return CHUNK_EOF;
}

ModelScriptTextParser::Result ModelScriptTextParser::parseArguments()
{
    if (!nextIs('('))
    {
        // no args, just object name
        return Success;
    }

    Result res = expectChar('(');
    if (res != Success)
        return res;

    m_ArgCount = 0;
    while (true)
    {
        res = token();
        if (res != Success)
        {
            if (res == End)
            {
                LogError() << "invalid end in argument list in line " << m_Line;
                return Error;
            }
            return res;
        }

        if (m_ArgCount < m_Args.size())
        {
            std::string &arg = m_Args[m_ArgCount];
            arg.clear();
            arg.insert(arg.begin(), m_Token.text.begin(), m_Token.text.end());
        } else
            m_Args.emplace_back(m_Token.text);

        m_ArgCount++;

        if (!m_NextToken.text.empty() && m_NextToken.text[0] == ')')
            break;
    }

    return expectChar(')');
}

ModelScriptParser::EChunkType ModelScriptTextParser::parseFileChunk()
{
    if (m_Token.text == "MODEL")
    {
        if (!parseModelStart())
            return CHUNK_ERROR;

        return CHUNK_MODEL;
    }

    if (!m_Strict && !m_Token.text.empty() && m_Token.text[0] == '(')
    {
        // fixes duplicate '}' after anim, which will make the parser think
        // it's on the file level
        m_Context.push_back(ContextAniEnum);
        return parseAniEnumChunk();
    }

    LogError() << "invalid token '" << m_Token.text << "' in file at line " << m_Line;
    return CHUNK_ERROR;
}

bool ModelScriptTextParser::parseModelStart()
{
    Result res = parseArguments();
    if (res != Success)
        return false;

    // TODO: assign

    res = parseObjectStart();
    if (res != Success)
        return false;

    m_Context.push_back(ContextModel);

    return true;
}

ModelScriptParser::EChunkType ModelScriptTextParser::parseModelChunk()
{
    if (m_Token.text == "MESHANDTREE")
    {
        Result res = parseArguments();
        // TODO
        return (res != Success) ? CHUNK_ERROR : CHUNK_MESH_AND_TREE;

    } else
    if (m_Token.text == "REGISTERMESH")
    {
        Result res = parseArguments();
        // TODO
        return (res != Success) ? CHUNK_ERROR : CHUNK_REGISTER_MESH;

    } else
    if (m_Token.text == "ANIENUM")
    {
        if (!parseAniEnumStart())
            return CHUNK_ERROR;

        return CHUNK_ANI_ENUM;
    }

    LogError() << "invalid token '" << m_Token.text << "' in Model at line " << m_Line;
    return CHUNK_ERROR;
}

bool ModelScriptTextParser::parseAniEnumStart()
{
    Result res = parseArguments();
    if (res != Success)
        return false;

    // TODO: assign

    res = parseObjectStart();
    if (res != Success)
        return false;

    m_Context.push_back(ContextAniEnum);

    return true;
}

ModelScriptTextParser::EChunkType ModelScriptTextParser::parseAniEnumChunk()
{
    if (m_Token.text == "ANI")
    {
        return (parseAni() != Success) ? CHUNK_ERROR : CHUNK_ANI;
    } else
    if (m_Token.text == "ANIALIAS")
    {
        return (parseAniAlias() != Success) ? CHUNK_ERROR : CHUNK_ANI_ALIAS;
    } else
    if (m_Token.text == "ANIBLEND")
    {
        return (parseAniBlend() != Success) ? CHUNK_ERROR : CHUNK_ANI_BLEND;
    }

    LogError() << "invalid token '" << m_Token.text << "' in aniEnum at line " << m_Line;
    return CHUNK_ERROR;
}

ModelScriptTextParser::Result ModelScriptTextParser::parseAniEvents()
{
    if (!nextIs('{'))
        return Success; // no events

    Result res = parseObjectStart();
    if (res != Success)
        return Error;

    while (true)
    {
        res = token();
        if (res != Success)
            break;

        if (m_Token.text == "*EVENTSWAPMESH")
        {
            res = parseSwapMeshEvent();
            if (res != Success)
                break;
        } else
        if (m_Token.text == "*EVENTPFX")
        {
            res = parsePfxEvent();
            if (res != Success)
                break;
        } else
        if (m_Token.text == "*EVENTPFXSTOP")
        {
            res = parsePfxStopEvent();
            if (res != Success)
                break;
        } else
        if (m_Token.text == "*EVENTSFX")
        {
            res = parseSfxEvent();
            if (res != Success)
                break;

        } else
        if (m_Token.text == "*EVENTSFXGRND")
        {
            res = parseSfxGrndEvent();
            if (res != Success)
                break;

        } else
        if (m_Token.text == "*EVENTTAG")
        {
            res = parseTagEvent();
            if (res != Success)
                break;

        } else
        {
            LogError() << "invalid token '" << m_Token.text << "' in ani at line " << m_Line;
            return Error;
        }

        if (nextIs('}'))
            break;
    }

    return parseObjectEnd();
}

ModelScriptTextParser::Result ModelScriptTextParser::parseAni()
{
    Result res = parseArguments();
    if (res != Success)
        return Error;

    if (m_ArgCount != 10)
    {
        LogError() << "invalid number of arguments for ani at line " << m_Line;
        return Error;
    }

    m_Ani.m_Name = m_Args[0];
    /*m_Ani.m_Layer = m_Args[1];*/
    m_Ani.m_Next = m_Args[2];
    /*m_Ani.m_BlendIn = m_Args[3];*/
    /*m_Ani.m_BlendOut = m_Args[4];*/
    /*m_Ani.m_Flags = m_Args[5];*/
    /*m_Ani.m_Asc = m_Args[6];*/
    /*m_Ani.m_Dir = m_Args[7];*/
    /*m_Ani.mStartFrame = m_Args[8];*/
    /*m_Ani.mEndFrame = m_Args[9];*/

    return parseAniEvents();
}

ModelScriptTextParser::Result ModelScriptTextParser::parseAniAlias()
{
    Result res = parseArguments();
    if (res != Success)
        return Error;

    return parseAniEvents();
}

ModelScriptTextParser::Result ModelScriptTextParser::parseAniBlend()
{
    Result res = parseArguments();
    if (res != Success)
        return Error;

    return parseAniEvents();
}

ModelScriptTextParser::Result ModelScriptTextParser::parseSwapMeshEvent()
{
    Result res = parseArguments();
    if (res != Success)
        return Error;

    // TODO: assign

    return Success;
}

ModelScriptTextParser::Result ModelScriptTextParser::parsePfxEvent()
{
    Result res = parseArguments();
    if (res != Success)
        return Error;

    // TODO: assign

    return Success;
}

ModelScriptTextParser::Result ModelScriptTextParser::parsePfxStopEvent()
{
    Result res = parseArguments();
    if (res != Success)
        return Error;

    // TODO: assign

    return Success;
}

ModelScriptTextParser::Result ModelScriptTextParser::parseSfxEvent()
{
    Result res = parseArguments();
    if (res != Success)
        return Error;

    // TODO: assign

    return Success;
}

ModelScriptTextParser::Result ModelScriptTextParser::parseSfxGrndEvent()
{
    Result res = parseArguments();
    if (res != Success)
        return Error;

    // TODO: assign

    return Success;
}

ModelScriptTextParser::Result ModelScriptTextParser::parseTagEvent()
{
    Result res = parseArguments();
    if (res != Success)
        return Error;

    // TODO: assign

    return Success;
}

} // namespace ZenLoad
