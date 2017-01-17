#pragma once

#include <cstdint>
#include <string>

enum
{
    ANIEVENT_MAXSTRING              = 4
};

// TODO: these should be general flags?
enum EModelScriptAniFlags
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
enum EModelScriptAniDir
{
    MSB_FORWARD,
    MSB_BACKWARD
};

struct zCModelScriptAni
{
    /// Add + ".MAN" for the animation data
    std::string                     m_Name;
    uint32_t                        m_Layer = 0;
    std::string                     m_Next;
    float                           m_BlendIn = 0;
    float                           m_BlendOut = 0;
    uint32_t                        m_Flags = 0;
    std::string                     m_Asc;
    EModelScriptAniDir              m_Dir = MSB_FORWARD;
    uint32_t                        m_FirstFrame = 0;
    uint32_t                        m_LastFrame = 0;
    float                           m_MaxFps = 0.0f;
    float                           m_Speed = 0.0f;
    float                           m_ColVolScale = 1.0f;
};

struct zCModelScriptAniAlias
{
    /// Add + ".MAN" for the animation data
    std::string                     m_Name;
    uint32_t                        m_Layer = 0;
    std::string                     m_Next;
    float                           m_BlendIn = 0;
    float                           m_BlendOut = 0;
    uint32_t                        m_Flags = 0;
    std::string                     m_Alias;
    EModelScriptAniDir              m_Dir = MSB_FORWARD;
};

struct zCModelScriptAniBlend
{
    std::string                     m_Name;
    uint32_t                        m_Layer = 0;
    std::string                     m_Next;
    float                           m_BlendIn = 0;
    float                           m_BlendOut = 0;
};

struct zCModelScriptAniSync
{
    std::string                     m_Name;
    uint32_t                        m_Layer = 0;
    std::string                     m_Next;
};

struct zCModelScriptAniCombine
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

struct zCModelScriptAniDisable
{
    std::string                     m_Name;
};

struct zCModelScriptTag
{
    std::string                     m_Tag;
    std::string                     m_Values[ANIEVENT_MAXSTRING];
};

struct zCModelScriptEventSfx
{
    uint32_t                        m_Frame = 0;
    std::string                     m_Name;
    std::string                     m_Value1;
    std::string                     m_Value2;
};
