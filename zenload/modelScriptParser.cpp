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

ModelScriptBinParser::EChunkType ModelScriptTextParser::parse()
{
    m_Token = m_NextToken = Token();
    m_Line = 1;

    // read
    Result res = token();
    if (res != Success)
        return CHUNK_ERROR; 

    while (true)
    {
        Result res = token();
        if (res == End)
            break;

        if (res == Error)
            return CHUNK_ERROR;

        switch (m_Token.type)
        {
        case TokenText:
            if (m_Token.text == "MODEL")
            {
                res = parseModel();
                if (res == Success)
                    return CHUNK_MODEL;

                return (res == End) ? CHUNK_EOF : CHUNK_ERROR;
            }

            // fallthrough for unrecognized tokens!

        default:
            LogError() << "invalid token '" << m_Token.text << "' in file at line " << m_Line;
            return CHUNK_ERROR;
        }

        LogInfo() << "token: " << m_Token.text << " " << m_Token.line;
    }

    return CHUNK_EOF;
}

ModelScriptTextParser::ModelScriptTextParser(ZenParser &zen)
    : ModelScriptParser(zen)
{

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

                m_NextToken.text.push_back(toupper(ch));
            }

            continue;

        } else
        if (ch == '/' && (m_Zen.getSeek() < m_Zen.getFileSize() - 1) && (m_Zen.getData()[m_Zen.getSeek() + 1] == '/'))
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

ModelScriptTextParser::Result ModelScriptTextParser::parseObjectStart()
{
    return expectChar('{');
}

ModelScriptTextParser::Result ModelScriptTextParser::parseObjectEnd()
{
    return expectChar('}');
}

ModelScriptTextParser::Result ModelScriptTextParser::parseArguments()
{
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

ModelScriptTextParser::Result ModelScriptTextParser::parseModel()
{
    Result res = parseArguments();
    if (res != Success)
        return Error;

    // TODO: assign

    res = parseObjectStart();
    if (res != Success)
        return Error;

    while (true)
    {
        res = token();
        if (res != Success)
        {
            if (res == Error)
                return res;
            break;
        }

        if (m_Token.text == "MESHANDTREE")
        {
            res = parseArguments();
            if (res != Success)
                break;

        } else
        if (m_Token.text == "REGISTERMESH")
        {
            res = parseArguments();
            if (res != Success)
                break;

        } else
        if (m_Token.text == "ANIENUM")
        {
            res = parseAnimEnum();
            if (res != Success)
                break;
        } else
        {
            LogError() << "invalid token '" << m_Token.text << "' in Model at line " << m_Line;
            return Error;
        }

        if (!m_NextToken.text.empty() && m_NextToken.text[0] == '}')
            break;
    }

    if (res == Error)
        return res;

    return parseObjectEnd();
#if 0

               if (m_Token.text == "ANI")
               {

               } else
               if (m_Token.text == "ANIALIAS")
               {

               } else
               if (m_Token.text == "ANIBLEND")
               {

               } else
               if (m_Token.text == "EVENTSFX")
               {

               } else
               if (m_Token.text == "EVENTSFXGRND")
               {

               }
#endif
}

ModelScriptTextParser::Result ModelScriptTextParser::parseAnimEnum()
{
    Result res = parseArguments();
    if (res != Success)
        return Error;

    res = parseObjectStart();
    if (res != Success)
        return Error;

    // TODO

    return parseObjectEnd();
}

} // namespace ZenLoad
