#pragma once

#include <string>

#include <zenload/zCModelAni.h>
#include <zenload/zCModelScript.h>

namespace ZenLoad
{

class ZenParser;

/** Streaming parser for .MAN files.
 */
class ModelScriptBinParser
{
public:

    typedef std::vector<zCModelAniSample> Samples;
    typedef std::vector<uint32_t> NodeIndex;

    enum EChunkType
    {       
        CHUNK_ERROR,
        CHUNK_EOF,

        CHUNK_MODEL_SCRIPT          = 0xF000,
        CHUNK_SOURCE                = 0xF100,
        CHUNK_MODEL                 = 0xF200,
        CHUNK_MESH_AND_TREE         = 0xF300,
        CHUNK_REGISTER_MESH         = 0xF400,
        CHUNK_ANI_ENUM              = 0xF500,
        CHUNK_ANI_MAX_FPS           = 0xF510,
        CHUNK_ANI                   = 0xF520,
        CHUNK_ANI_ALIAS             = 0xF530,
        CHUNK_ANI_BLEND             = 0xF540,
        CHUNK_ANI_SYNC              = 0xF550,
        CHUNK_ANI_BATCH             = 0xF560,
        CHUNK_ANI_COMB              = 0xF570,
        CHUNK_ANI_DISABLE           = 0xF580,
        CHUNK_MODE_LTAG             = 0xF590,
        CHUNK_ANI_EVENTS            = 0xF5A0,
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

    ModelScriptBinParser(ZenParser &zen);

    /** Returns the parsed animation alias.
     *
     * Call this if parse() returns CHUNK_ANI.
     *
     * @return The ani read during the last call to parse().
     */
    const zCModelScriptAni                  &ani() const { return m_Ani; }

    /** Returns the parsed animation alias.
     *
     * Call this if parse() returns CHUNK_ANI_ALIAS.
     *
     * @return The alias read during the last call to parse().
     */
    const zCModelScriptAniAlias             &alias() const { return m_Alias; }

    /** Returns the sfx event.
     *
     * Call this if parse() returns CHUNK_EVENT_SFX or CHUNK_EVENT_SFX_GRND.
     *
     * @return The header read during the last call to parse().
     */
    const zCModelScriptEventSfx             &sfx() const { return m_Sfx; }

    EChunkType                      parse();

private:

    ZenParser                       &m_Zen;

    zCModelScriptAni                        m_Ani;
    zCModelScriptAniAlias                   m_Alias;
    zCModelScriptEventSfx                   m_Sfx;

    void                            readAni();
    void                            readAlias();
    void                            readSfx();
};

} // namespace ZenLoad
