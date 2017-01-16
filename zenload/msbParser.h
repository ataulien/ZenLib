#pragma once

#include <string>

#include <zenload/zCModelAni.h>

namespace ZenLoad
{

// TODO: these should be general flags?
enum EMsbAniFlags
{
    /// Animation moves model in world space
    MSB_MOVE_MODEL                  = 0x00000001,
    /// Animation rotates model in world space
    MSB_ROTATE_MODEL                = 0x00000002,
    /// Animation is queued after the current any on layer instead of started immediately
    MSB_QUEUE_ANI                   = 0x00000004,
    /// Don't stick to ground
    MSB_FLY                         = 0x00000008,
    /// Idle animation
    MSB_IDLE                        = 0x00000010,
};

// TODO: should general enum?
enum EMsbAniDir
{
    MSB_FORWARD,
    MSB_BACKWARD
};

struct zCMsbAni
{
    /// Add + ".MAN" for the animation data
    std::string                     m_Name;
    uint32_t                        m_Layer = 0;
    std::string                     m_Next;
    float                           m_BlendIn = 0;
    float                           m_BlendOut = 0;
    uint32_t                        m_Flags = 0;
    std::string                     m_Asc;
    EMsbAniDir                      m_Dir = MSB_FORWARD;
    uint32_t                        m_FirstFrame = 0;
    uint32_t                        m_LastFrame = 0;
    float                           m_MaxFps = 0.0f;
    float                           m_Speed = 0.0f;
    float                           m_ColVolScale = 1.0f;
};

struct zCMsbAniAlias
{
    /// Add + ".MAN" for the animation data
    std::string                     m_Name;
    uint32_t                        m_Layer = 0;
    std::string                     m_Next;
    float                           m_BlendIn = 0;
    float                           m_BlendOut = 0;
    uint32_t                        m_Flags = 0;
    std::string                     m_Alias;
    EMsbAniDir                      m_Dir = MSB_FORWARD;
};

struct zCMsbAniBlend
{
    std::string                     m_Name;
    uint32_t                        m_Layer = 0;
    std::string                     m_Next;
    float                           m_BlendIn = 0;
    float                           m_BlendOut = 0;
};

struct zCMsbAniSync
{
    std::string                     m_Name;
    uint32_t                        m_Layer = 0;
    std::string                     m_Next;
};

struct zCMsbAniCombine
{
    std::string                     m_Name;
    uint32_t                        m_Layer = 0;
    std::string                     m_Next;
    float                           m_BlendIn = 0;
    float                           m_BlendOut = 0;
    uint32_t                        m_Flags = 0;
    std::string                     m_Asc;
    uint32_t                        m_LastFrame = 0;
};

struct zCMsbAniDisable
{
    std::string                     m_Name;
};

struct zCMsbTag
{
    std::string                     m_Tag;
    std::string                     m_Values[ANIEVENT_MAXSTRING];
};

struct zCMsbEventSfx
{
    uint32_t                        m_Frame = 0;
    std::string                     m_Name;
    std::string                     m_Value1;
    std::string                     m_Value2;
};

class ZenParser;

/** Streaming parser for .MAN files.
 */
class MsbParser
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

    MsbParser(ZenParser &zen);

    /** Returns the parsed animation alias.
     *
     * Call this if parse() returns CHUNK_ANI.
     *
     * @return The ani read during the last call to parse().
     */
    const zCMsbAni                  &ani() const { return m_Ani; }

    /** Returns the parsed animation alias.
     *
     * Call this if parse() returns CHUNK_ANI_ALIAS.
     *
     * @return The alias read during the last call to parse().
     */
    const zCMsbAniAlias             &alias() const { return m_Alias; }

    /** Returns the sfx event.
     *
     * Call this if parse() returns CHUNK_EVENT_SFX or CHUNK_EVENT_SFX_GRND.
     *
     * @return The header read during the last call to parse().
     */
    const zCMsbEventSfx             &sfx() const { return m_Sfx; }

    EChunkType                      parse();

private:

    ZenParser                       &m_Zen;

    zCMsbAni                        m_Ani;
    zCMsbAniAlias                   m_Alias;
    zCMsbEventSfx                   m_Sfx;

    void                            readAni();
    void                            readAlias();
    void                            readSfx();
};

} // namespace ZenLoad
