#include "modelScriptParser.h"

#include <algorithm>
#include <cmath>

#include "zenParser.h"

#include <utils/logger.h>

namespace ZenLib
{
    namespace ZenLoad
    {
        ModelScriptParser::ModelScriptParser(ZenParser& zen)
            : m_Zen(zen)
        {
        }

        ModelScriptParser::~ModelScriptParser()
        {
        }

        ModelScriptBinParser::ModelScriptBinParser(ZenParser& zen)
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

            switch (chunk.id)
            {
                case CHUNK_MESH_AND_TREE:
                    readMeshAndTree();
                    m_Zen.setSeek(chunk_end);
                    return CHUNK_MESH_AND_TREE;
                    break;
                case CHUNK_REGISTER_MESH:
                    readRegisterMesh();
                    m_Zen.setSeek(chunk_end);
                    return CHUNK_REGISTER_MESH;
                    break;
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
                case CHUNK_EVENT_SFX_GRND:
                    readSfx();
                    m_Zen.setSeek(chunk_end);
                    return CHUNK_EVENT_SFX_GRND;
                case CHUNK_EVENT_PFX:
                    readPfx();
                    m_Zen.setSeek(chunk_end);
                    return CHUNK_EVENT_PFX;
                case CHUNK_EVENT_PFX_STOP:
                    readPfxStop();
                    m_Zen.setSeek(chunk_end);
                    return CHUNK_EVENT_PFX_STOP;
                default:
                    m_Zen.setSeek(chunk_end);
                    return parse();  // skip unknown chunk
            }

            return CHUNK_EOF;
        }

        static uint32_t makeAniFlags(ZenParser& zen)
        {
            uint32_t flags = 0;
            std::string flag_str = zen.readLine(true);
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

        static EModelScriptAniDir makeAniDir(ZenParser& zen)
        {
            std::string str = zen.readLine();
            return (!str.empty() && str[0] == 'R') ? MSB_BACKWARD : MSB_FORWARD;
        }

        void ModelScriptBinParser::readMeshAndTree()
        {
            bool dontUseMesh = m_Zen.readBinaryDWord() != 0;
            std::string meshNameASC = m_Zen.readLine();

            // This should absolutely have an extension of .ASC, but sometimes doesn't.
            // Add it, if it's not there.
            if (meshNameASC.find_first_of('.') == std::string::npos)
            {
                meshNameASC += ".ASC";
            }

            if (!dontUseMesh)
            {
                m_MeshesASC.push_back(meshNameASC);
            }

            m_MeshAndTree = meshNameASC;
        }

        void ModelScriptBinParser::readRegisterMesh()
        {
            // Reads "Some_Mesh.ASC"
            std::string mesh = m_Zen.readLine();

            m_MeshesASC.push_back(mesh);
        }

        void ModelScriptBinParser::readAni()
        {
            m_Ani.m_Name = m_Zen.readLine();
            m_Ani.m_Layer = m_Zen.readBinaryDWord();
            m_Ani.m_Next = m_Zen.readLine();
            m_Ani.m_BlendIn = m_Zen.readBinaryFloat();
            m_Ani.m_BlendOut = m_Zen.readBinaryFloat();
            m_Ani.m_Flags = makeAniFlags(m_Zen);
            m_Ani.m_Asc = m_Zen.readLine();
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
            m_Sfx.emplace_back();
            m_Sfx.back().m_Frame = m_Zen.readBinaryDWord();
            m_Sfx.back().m_Name = m_Zen.readLine(true);

            float range = m_Zen.readBinaryFloat();
            float emptySlot = m_Zen.readBinaryFloat();

            m_Sfx.back().m_Range = range / 100.0f;        // Convert to meters
            m_Sfx.back().m_EmptySlot = emptySlot > 0.0f;  // They encoded this as float for some reason...
        }
        void ModelScriptBinParser::readPfx()
        {
            m_Pfx.emplace_back();

            m_Pfx.back().m_Frame = m_Zen.readBinaryDWord();

            m_Pfx.back().m_Num = m_Zen.readBinaryDWord();
            m_Pfx.back().m_Name = m_Zen.readLine(true);
            m_Pfx.back().m_Pos = m_Zen.readLine(true);
            //Like EmptySlot in readSfx encoded in float. No " " around value might be a hint that no string is used
            m_Pfx.back().m_isAttached = m_Zen.readBinaryFloat() > 0.0f;
        }

        void ModelScriptBinParser::readPfxStop()
        {
            m_PfxStop.emplace_back();
            m_PfxStop.back().m_Frame = m_Zen.readBinaryDWord();
            m_PfxStop.back().m_Num = m_Zen.readBinaryDWord();
        }

        ModelScriptTextParser::ModelScriptTextParser(ZenParser& zen)
            : ModelScriptParser(zen)
            , m_Line(1)
        {
            // read current token into next
            token();

            m_Context.push_back(ContextFile);

            //LogInfo() << "MDS\n" << reinterpret_cast<const char*>(&zen.getData()[0]);
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
                            return Success;  // Need to return early here, because emtpy strings ( "" ) happen, which should not be seen as empty lines

                        if (ch == '\n')
                        {
                            LogError() << "parser error in line " << m_Line << ": unexpected newline in quoted string";
                            return Error;
                        }

                        m_NextToken.text.push_back(ch);
                    }

                    continue;
                }
                else if (ch == '/' && (!m_Strict || ((m_Zen.getSeek() < m_Zen.getFileSize() - 1) && (m_Zen.getData()[m_Zen.getSeek()] == '/'))))  // Single / were supported by the original parser as well
                {
                    while (m_Zen.readBinaryByte() != '\n')
                    {
                        if (isEof())
                            return End;
                    }

                    m_Line++;
                    skipSpace();  // Skip any empty lines after the comment
                    continue;     // Don't want comments inside the tokens
                }
                else if (isspace(ch) || ch == '(' || ch == ')' || ch == '{' || ch == '}')
                {
                    if (m_NextToken.text.empty())
                    {
                        if (!isspace(ch))
                            m_NextToken.text.push_back(ch);
                    }
                    else
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
            while (true)
            {
                if (isEof())
                    return End;

                auto& zd = m_Zen.getData()[m_Zen.getSeek()];
                if (!isspace(zd) && zd != '\n')
                    break;

                if (zd == '\n')
                    m_Line++;

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

            // token is consumed by the functions below
            Result res = token();
            if (res != Success)
                return CHUNK_ERROR;

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

            m_Args.clear();
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
                    std::string& arg = m_Args[m_ArgCount];
                    arg.clear();
                    arg.insert(arg.begin(), m_Token.text.begin(), m_Token.text.end());
                }
                else
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

                // Second arg would be DONT_USE_MESH.
                if (m_ArgCount == 1)
                {
                    m_MeshesASC.push_back(m_Args[0]);
                }

                m_MeshAndTree = m_Args[0];

                // TODO
                return (res != Success) ? CHUNK_ERROR : CHUNK_MESH_AND_TREE;
            }
            else if (m_Token.text == "REGISTERMESH")
            {
                Result res = parseArguments();

                if (m_ArgCount == 1)
                {
                    m_MeshesASC.push_back(m_Args[0]);
                }

                return (res != Success) ? CHUNK_ERROR : CHUNK_REGISTER_MESH;
            }
            else if (m_Token.text == "ANIENUM")
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
            }
            else if (m_Token.text == "ANIALIAS")
            {
                return (parseAniAlias() != Success) ? CHUNK_ERROR : CHUNK_ANI_ALIAS;
            }
            else if (m_Token.text == "ANIBLEND")
            {
                return (parseAniBlend() != Success) ? CHUNK_ERROR : CHUNK_ANI_BLEND;
            }
            else if (m_Token.text == "MODELTAG")
            {
                return (parseModelTag() != Success) ? CHUNK_ERROR : CHUNK_MODEL_TAG;
            }
            else if (m_Token.text == "ANIDISABLE")
            {
                return (parseAniDisable() != Success) ? CHUNK_ERROR : CHUNK_MODEL_TAG;
            }
            else if (m_Token.text == "ANICOMB")
            {
                return (parseAniComb() != Success) ? CHUNK_ERROR : CHUNK_MODEL_TAG;
            }

            LogError() << "invalid token '" << m_Token.text << "' in aniEnum at line " << m_Line;
            return CHUNK_ERROR;
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parseAniEvents()
        {
            if (!nextIs('{'))
                return Success;  // no events

            Result res = parseObjectStart();
            if (res != Success)
                return Error;
            while (true)
            {
                // Need to check this first, to fix parsing of empty { } blocks
                if (nextIs('}'))
                    break;

                res = token();
                if (res != Success)
                    break;

                if (m_Token.text == "*EVENTSWAPMESH")
                {
                    res = parseSwapMeshEvent();
                    if (res != Success)
                        break;
                }
                else if (m_Token.text == "*EVENTPFX")
                {
                    //gets never called...
                    res = parsePfxEvent();
                    if (res != Success)
                        break;
                }
                else if (m_Token.text == "*EVENTPFXSTOP")
                {
                    res = parsePfxStopEvent();
                    if (res != Success)
                        break;
                }
                else if (m_Token.text == "*EVENTSFX")
                {
                    res = parseSfxEvent();
                    if (res != Success)
                        break;
                }
                else if (m_Token.text == "*EVENTSFXGRND")
                {
                    res = parseSfxGrndEvent();
                    if (res != Success)
                        break;
                }
                else if (m_Token.text == "*EVENTTAG")
                {
                    res = parseTagEvent();
                    if (res != Success)
                        break;
                }
                else if (m_Token.text == "*EVENTMMSTARTANI")
                {
                    res = parseMMStartAniEvent();
                    if (res != Success)
                        break;
                }
                else if (m_Token.text == "*EVENTCAMTREMOR")
                {
                    res = parseCamTremorEvent();
                    if (res != Success)
                        break;
                }
                else
                {
                    LogError() << "invalid token '" << m_Token.text << "' in ani at line " << m_Line;
                    return Error;
                }
            }

            return parseObjectEnd();
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parseAni()
        {
            Result res = parseArguments();
            if (res != Success)
                return Error;

            // Most of the ani-chunks have 10 args, but they can have an additional one for FPS and some have only 8, omitting start and end-frame
            if (m_ArgCount < 8)
            {
                LogError() << "invalid number of arguments for ani at line " << m_Line;
                return Error;
            }

            m_Ani.m_Name = m_Args[0];

            std::transform(m_Ani.m_Name.begin(), m_Ani.m_Name.end(), m_Ani.m_Name.begin(), ::toupper);

            m_Ani.m_Layer = std::stoi(m_Args[1]);
            m_Ani.m_Next = m_Args[2];
            std::transform(m_Ani.m_Next.begin(), m_Ani.m_Next.end(), m_Ani.m_Next.begin(), ::toupper);

            std::string& flags = m_Args[5];

            m_Ani.m_Flags = 0;
            for (size_t i = 0; i < flags.size(); i++)
            {
                switch (flags[i])
                {
                    case 'M':
                        m_Ani.m_Flags |= EModelScriptAniFlags::MSB_MOVE_MODEL;
                        break;

                    case 'R':
                        m_Ani.m_Flags |= EModelScriptAniFlags::MSB_ROTATE_MODEL;
                        break;

                    case 'E':
                        m_Ani.m_Flags |= EModelScriptAniFlags::MSB_QUEUE_ANI;
                        break;

                    case 'F':
                        m_Ani.m_Flags |= EModelScriptAniFlags::MSB_FLY;
                        break;

                    case 'I':
                        m_Ani.m_Flags |= EModelScriptAniFlags::MSB_IDLE;
                        break;

                    case '.':
                        // This is used as a placeholder
                        break;

                    default:
                        LogWarn() << "Anim: Unknown animation-flag: " << flags[i];
                }
            }

            if (m_Args.size() >= 10)
            {
                std::string firstFrame = m_Args[8];
                std::string lastFrame = m_Args[9];

                m_Ani.m_FirstFrame = std::stoi(firstFrame);
                m_Ani.m_LastFrame = std::stoi(lastFrame);
            }

            /*m_Ani.m_BlendIn = m_Args[3];*/
            /*m_Ani.m_BlendOut = m_Args[4];*/
            /*m_Ani.m_Flags = m_Args[5];*/
            /*m_Ani.m_Asc = m_Args[6];*/
            /**/
            /*m_Ani.mStartFrame = m_Args[8];*/
            /*m_Ani.mEndFrame = m_Args[9];*/
            // TODO: Read FPS

            return parseAniEvents();
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parseAniAlias()
        {
            Result res = parseArguments();
            if (res != Success)
                return Error;

            if (m_ArgCount < 8)
            {
                LogError() << "invalid number of arguments for aniAlias at line " << m_Line;
                return Error;
            }

            m_Alias.m_Name = m_Args[0];
            std::transform(m_Alias.m_Name.begin(), m_Alias.m_Name.end(), m_Alias.m_Name.begin(), ::toupper);

            m_Ani.m_Layer = std::stoi(m_Args[1]);

            m_Alias.m_Next = m_Args[2];
            std::transform(m_Alias.m_Next.begin(), m_Alias.m_Next.end(), m_Alias.m_Next.begin(), ::toupper);

            m_Alias.m_Alias = m_Args[6];
            std::transform(m_Alias.m_Alias.begin(), m_Alias.m_Alias.end(), m_Alias.m_Alias.begin(), ::toupper);

            m_Alias.m_Dir = (!m_Args[7].empty() && m_Args[7][0] == 'R') ? MSB_BACKWARD : MSB_FORWARD;

            std::string& flags = m_Args[5];

            m_Alias.m_Flags = 0;
            for (size_t i = 0; i < flags.size(); i++)
            {
                switch (flags[i])
                {
                    case 'M':
                        m_Alias.m_Flags |= EModelScriptAniFlags::MSB_MOVE_MODEL;
                        break;

                    case 'R':
                        m_Alias.m_Flags |= EModelScriptAniFlags::MSB_ROTATE_MODEL;
                        break;

                    case 'E':
                        m_Alias.m_Flags |= EModelScriptAniFlags::MSB_QUEUE_ANI;
                        break;

                    case 'F':
                        m_Alias.m_Flags |= EModelScriptAniFlags::MSB_FLY;
                        break;

                    case 'I':
                        m_Alias.m_Flags |= EModelScriptAniFlags::MSB_IDLE;
                        break;

                    case '.':
                        // This is used as a placeholder
                        break;

                    default:
                        LogWarn() << "Anim: Unknown anialias-flag: " << flags[i];
                }
            }

            return parseAniEvents();
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parseAniBlend()
        {
            Result res = parseArguments();
            if (res != Success)
                return Error;

            if (m_ArgCount < 2 || m_ArgCount > 4)
            {
                LogError() << "invalid number of arguments for aniBlend at line " << m_Line;
                return Error;
            }

            m_Blend.m_Name = m_Args[0];
            std::transform(m_Blend.m_Name.begin(), m_Blend.m_Name.end(), m_Blend.m_Name.begin(), ::toupper);

            m_Blend.m_Next = m_Args[1];
            std::transform(m_Blend.m_Next.begin(), m_Blend.m_Next.end(), m_Blend.m_Next.begin(), ::toupper);

            // There doesn't seem to be any layer information on the aniBlends
            m_Blend.m_Layer = 1;

            if (m_ArgCount <= 2)
            {
                m_Blend.m_BlendIn = 0.0f;
                m_Blend.m_BlendOut = 0.0f;
            }
            else
            {
                m_Blend.m_BlendIn = std::stof(m_Args[2]);
                m_Blend.m_BlendOut = std::stof(m_Args[3]);
            }

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

        inline bool isStringNumber(const std::string& s)
        {
            return s.find_first_not_of("0123456789") == std::string::npos;
        }
        ModelScriptTextParser::Result ModelScriptTextParser::parsePfxEvent()
        {
            Result res = parseArguments();
            if (res != Success || m_Args.size() < 3 || m_Args.size() > 5)
                return Error;

            m_Pfx.emplace_back();
            unsigned int currentArgIndex = 0;
            //Base case: no node name and no attach
            m_Pfx.back().m_Frame = (uint32_t)std::stoi(m_Args[currentArgIndex]);
            ++currentArgIndex;
            //If pfx event has no handle number, no pfx end event exists for it
            if (isStringNumber(m_Args[currentArgIndex]))
            {
                m_Pfx.back().m_Num = (uint32_t)std::stoi(m_Args[currentArgIndex]);
                ++currentArgIndex;
            }
            else
            {
                m_Pfx.back().m_Num = 0;
            }
            m_Pfx.back().m_Name = m_Args[currentArgIndex];
            ++currentArgIndex;

            switch (m_Args.size())
            {
                //No attach
                case 4:
                    m_Pfx.back().m_Pos = m_Args[currentArgIndex];
                    break;
                case 5:
                    m_Pfx.back().m_Pos = m_Args[currentArgIndex];
                    if (m_Args[currentArgIndex] == "ATTACH")
                    {
                        //FIXME attach is encoded as float in other parsing function. Is this different here?
                        m_Pfx.back().m_isAttached = true;
                    }
                    else
                    {
                        LogWarn() << "Unknown string at the end of PFX Start Event " << m_Args[currentArgIndex];
                    }
                    break;
                default:
                    LogWarn() << "Parsed malformed PFX Start Event";
            }
            return Success;
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parsePfxStopEvent()
        {
            Result res = parseArguments();
            if (res != Success || m_Args.size() != 2)
                return Error;

            m_PfxStop.emplace_back();
            m_PfxStop.back().m_Frame = (uint32_t)std::stoi(m_Args[0]);
            m_PfxStop.back().m_Num = (uint32_t)std::stoi(m_Args[1]);

            return Success;
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parseSfxEvent()
        {
            Result res = parseArguments();
            if (res != Success || m_Args.size() < 2 || m_Args.size() > 4)
                return Error;

            m_Sfx.emplace_back();
            m_Sfx.back().m_Frame = (uint32_t)std::stoi(m_Args[0]);
            m_Sfx.back().m_Name = m_Args[1];

            for (size_t i = 2; i < std::min(m_Args.size(), static_cast<size_t>(4)); i++)
            {
                // Look for optional arguments (First 2 args are required)
                if (m_Args[i].find("R:") != std::string::npos)
                {
                    m_Sfx.back().m_Range = std::stof(m_Args[i].substr(m_Args[i].find("R:") + 2));  // 2: Skip R:
                }
                else if (m_Args[i] == "EMPTY_SLOT")
                {
                    m_Sfx.back().m_EmptySlot = true;
                }
                else
                {
                    LogWarn() << "MODELSCRIPT: Invalid eventSFX-Option: " << m_Args[i];
                }
            }

            return Success;
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parseSfxGrndEvent()
        {
            Result res = parseArguments();
            if (res != Success || m_Args.size() < 2 || m_Args.size() > 4)
                return Error;

            m_SfxGround.emplace_back();
            m_SfxGround.back().m_Frame = (uint32_t)std::stoi(m_Args[0]);
            m_SfxGround.back().m_Name = m_Args[1];

            for (size_t i = 2; i < std::min(m_Args.size(), static_cast<size_t>(4)); i++)
            {
                // Look for optional arguments (First 2 args are required)
                if (m_Args[i].find("R:") != std::string::npos)
                {
                    m_Sfx.back().m_Range = std::stof(m_Args[i].substr(m_Args[i].find("R:") + 2));  // 2: Skip R:
                }
                else if (m_Args[i] == "EMPTY_SLOT")
                {
                    m_Sfx.back().m_EmptySlot = true;
                }
                else
                {
                    LogWarn() << "MODELSCRIPT: Invalid eventSFX-Option: " << m_Args[i];
                }
            }

            return Success;
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parseTagEvent()
        {
            Result res = parseArguments();
            if (res != Success || m_Args.size() < 2)
                return Error;

            m_Tag.emplace_back();

            // atoi is used here since some original MDS files have invalid arguments, which relay on atoi returning 0
            // on anything that is not a number. For example, t_BSANVIL_S1_2_S0 in HUMANS.MDS has a double opening
            // brace on one of the EventTags.
            m_Tag.back().m_Frame = (uint32_t)atoi(m_Args[0].c_str());
            m_Tag.back().m_Tag = m_Args[1];

            if (m_Args.size() >= 3)
            {
                m_Tag.back().m_Argument = m_Args[2];
            }

            return Success;
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parseMMStartAniEvent()
        {
            Result res = parseArguments();
            if (res != Success)
                return Error;

            m_MMStartAni.emplace_back();

            m_MMStartAni.back().m_Frame = (uint32_t)atoi(m_Args[0].c_str());
            m_MMStartAni.back().m_Animation = m_Args[1];

            return Success;
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parseModelTag()
        {
            Result res = parseArguments();
            if (res != Success)
                return Error;

            // TODO: assign

            return Success;
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parseAniDisable()
        {
            Result res = parseArguments();
            if (res != Success)
                return Error;

            // TODO: assign

            return Success;
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parseAniComb()
        {
            Result res = parseArguments();
            if (res != Success)
                return Error;

            // TODO: assign

            return parseAniEvents();
        }

        ModelScriptTextParser::Result ModelScriptTextParser::parseCamTremorEvent()
        {
            Result res = parseArguments();
            if (res != Success)
                return Error;

            // TODO: assign

            return Success;
        }

    }  // namespace ZenLoad
} // namespace ZenLib