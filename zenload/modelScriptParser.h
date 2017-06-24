#pragma once

#include <string>

#include <zenload/zCModelAni.h>
#include <zenload/zCModelScript.h>

namespace ZenLoad
{

class ZenParser;

class ModelScriptParser
{
public:

    enum EChunkType
    {
        CHUNK_ERROR,
        CHUNK_EOF,

        CHUNK_MODEL_SCRIPT          = 0xF000,
        CHUNK_MODEL_SCRIPT_END      = 0xFFFF,
        CHUNK_SOURCE                = 0xF100,
        CHUNK_MODEL                 = 0xF200,
        CHUNK_MODEL_END             = 0xF2FF,
        CHUNK_MESH_AND_TREE         = 0xF300,
        CHUNK_REGISTER_MESH         = 0xF400,
        CHUNK_ANI_ENUM              = 0xF500,
        CHUNK_ANI_ENUM_END          = 0xF5FF,
        CHUNK_ANI_MAX_FPS           = 0xF510,
        CHUNK_ANI                   = 0xF520,
        CHUNK_ANI_ALIAS             = 0xF530,
        CHUNK_ANI_BLEND             = 0xF540,
        CHUNK_ANI_SYNC              = 0xF550,
        CHUNK_ANI_BATCH             = 0xF560,
        CHUNK_ANI_COMB              = 0xF570,
        CHUNK_ANI_DISABLE           = 0xF580,
        CHUNK_MODEL_TAG             = 0xF590,
        CHUNK_ANI_EVENTS            = 0xF5A0,
        CHUNK_ANI_EVENTS_END        = 0xF5AF,
        CHUNK_EVENT_SFX             = 0xF5A1,
        CHUNK_EVENT_SFX_GRND        = 0xF5A2,
        CHUNK_EVENT_TAG             = 0xF5A3,
        CHUNK_EVENT_PFX             = 0xF5A4,
        CHUNK_EVENT_PFX_STOP        = 0xF5A5,
        CHUNK_EVENT_PFX_GRND        = 0xF5A6,
        CHUNK_EVENT_SET_MESH        = 0xF5A7,
        CHUNK_EVENT_SWAP_MESH       = 0xF5A8,
        CHUNK_EVENT_MMSTARTANI      = 0xF5A9,
        CHUNK_EVENT_CAMTREMOR       = 0xF5AA
    };

    typedef std::vector<zCModelAniSample> Samples;
    typedef std::vector<uint32_t>   NodeIndex;

    ModelScriptParser(ZenParser &zen);
    virtual ~ModelScriptParser();

    /** Returns the parsed animation alias.
     *
     * Call this if parse() returns CHUNK_ANI.
     *
     * @return The ani read during the last call to parse().
     */
    const zCModelScriptAni          &ani() const { return m_Ani; }

    /** Returns the parsed animation alias.
     *
     * Call this if parse() returns CHUNK_ANI_ALIAS.
     *
     * @return The alias read during the last call to parse().
     */
    const zCModelScriptAniAlias     &alias() const { return m_Alias; }

    /** Returns the sfx event.
     *
     * Call this if parse() returns CHUNK_EVENT_SFX or CHUNK_EVENT_SFX_GRND.
     * Also call it after an ani-chunk was parsed, in case this is a textfole
     *
     * @return The event read during the last call to parse().
     */
    std::vector<zCModelScriptEventSfx>     &sfx() { return m_Sfx; }
    std::vector<zCModelScriptEventSfx>     &sfxGround() { return m_SfxGround; }
    std::vector<zCModelScriptEventTag>          &tag() { return m_Tag; }

    /** Reads the next chunk.
     *
     * @return The chunk type or CHUNK_ERROR / CHUNK_EOF.
     *
     */
    virtual EChunkType              parse()=0;

protected:

    ZenParser                       &m_Zen;

    zCModelScriptAni                m_Ani;
    zCModelScriptAniAlias           m_Alias;
    std::vector<zCModelScriptEventSfx>           m_Sfx;
    std::vector<zCModelScriptEventSfx>           m_SfxGround;
    std::vector<zCModelScriptEventTag>                m_Tag;
};

/** Streaming parser for .MSB files.
 */
class ModelScriptBinParser : public ModelScriptParser
{
public:

    ModelScriptBinParser(ZenParser &zen);

    EChunkType                      parse() override;

private:

    void                            readAni();
    void                            readAlias();
    void                            readSfx();
};

/** Streaming parser for .MDS files.
 */
class ModelScriptTextParser : public ModelScriptParser
{
public:

    ModelScriptTextParser(ZenParser &zen);

    void                            setStrict(bool strict) { m_Strict = strict; }

    EChunkType                      parse() override;

private:

    enum Result
    {
        Error,
        End,
        Success
    };

    enum TokenType
    {
        TokenText,
        TokenOpenParen,
        TokenCloseParen,
    };

    enum Context
    {
        ContextFile,
        ContextModel,
        ContextAniEnum
    };

    struct Token
    {
        TokenType                   type = TokenText;
        unsigned                    line = 0;
        std::string                 text;
    };

    Token                           m_Token;
    Token                           m_NextToken;
    unsigned                        m_Line = 1;

    std::vector<std::string>        m_Args;
    unsigned                        m_ArgCount = 0;

    std::vector<Context>            m_Context;

    bool                            m_Strict = false;

    bool                            isEof() const;

    Result                          token();

    Result                          skipSpace();

    Result                          expectChar(char ch);

    bool                            nextIs(char ch) const;

    Result                          parseObjectStart();

    Result                          parseObjectEnd();

    Result                          parseArguments();

    EChunkType                      parseFileChunk();

    bool                            parseModelStart();

    EChunkType                      parseModelChunk();

    bool                            parseAniEnumStart();

    Result                          parseModelTag();

    Result                          parseAniDisable();

    Result                          parseAniComb();

    EChunkType                      parseAniEnumChunk();

    Result                          parseAni();

    Result                          parseAniAlias();

    Result                          parseAniBlend();

    Result                          parseAniEvents();

    Result                          parseTagEvent();

    Result                          parseMMStartAniEvent();

    Result                          parseCamTremorEvent();

    Result                          parseSfxEvent();

    Result                          parseSfxGrndEvent();

    Result                          parseSwapMeshEvent();

    Result                          parsePfxEvent();

    Result                          parsePfxStopEvent();
};

} // namespace ZenLoad
