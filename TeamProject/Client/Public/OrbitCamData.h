#pragma once

NS_BEGIN(Client)

enum class OrbitPreset { Field, Battle };

struct OrbitProfile
{
    _float  minDist             = 0.6f;
    _float  maxDist             = 4.f;
                                
    _float  pitchMin            = -30.f;
    _float  pitchMax            = 60.f;
                                
    _float  rotSmoothSpeed      = 18.f;
    _float  distSmoothSpeed     = 18.f;
    _float  pivotSmoothSpeed    = 15.f;

    _float  offsetY             = 0.f;

    _bool   usePitchAutoZoom    = true;
    _float  pitchAutoZoomMax    = 1.0f;
    _float  pitchAutoZoomStartN = 0.75f;
    _float  pitchAutoZoomSmooth = 18.f;

    _float  startDistance       = 2.3f;
    _float  startPitchDeg       = -15.f;
    _float  startHeightOffset   = 0.4f;
};
struct OrbitCollisionProfile
{
    _float radius      = 0.12f;
    _float skin        = 0.06f;
    _float smoothSpeed = 30.f;
};

struct OrbitCollisionState
{
    _float targetDist = 9999.f;
    _float curDist = 9999.f;
};
namespace OrbitPresets
{
    inline OrbitProfile Field()
    {
        OrbitProfile profile{};
        profile.minDist           = 0.6f;
        profile.maxDist           = 2.2f;
        profile.pitchMin          = -20.f;
        profile.pitchMax          = 55.f;
        profile.rotSmoothSpeed    = 22.f;
        profile.distSmoothSpeed   = 22.f;
        profile.pivotSmoothSpeed  = 18.f;
        profile.offsetY           = 0.2f;
        profile.startDistance     = 2.3f;
        profile.startPitchDeg     = -15.f;
        profile.startHeightOffset = 0.4f;
        return profile;
    }
    inline OrbitProfile Battle()
    {
        OrbitProfile profile{};
        profile.minDist           = 1.2f;
        profile.maxDist           = 3.5f;
        profile.pitchMin          = -35.f;
        profile.pitchMax          = 70.f;
        profile.rotSmoothSpeed    = 16.f;
        profile.distSmoothSpeed   = 16.f;
        profile.pivotSmoothSpeed  = 14.f;
        profile.offsetY           = 0.2f;
        profile.startDistance     = 3.0f;
        profile.startPitchDeg     = -18.f;
        profile.startHeightOffset = 0.6f;
        return profile;
    }
    inline OrbitProfile Get(OrbitPreset preset)
    {
        switch (preset)
        {
        case OrbitPreset::Field:  return Field();
        case OrbitPreset::Battle: return Battle();
        }
    }
}

NS_END