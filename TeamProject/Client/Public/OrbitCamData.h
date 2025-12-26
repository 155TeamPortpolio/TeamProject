#pragma once

NS_BEGIN(Client)

enum class OrbitPreset { Field, Battle };

struct OrbitProfile
{
    _float  minDist             = 0.6f;
    _float  maxDist             = 2.5f;
                                
    _float  pitchMin            = -30.f;
    _float  pitchMax            = 60.f;
                                
    _float  rotSmoothSpeed      = 18.f;
    _float  distSmoothSpeed     = 18.f;
    _float  pivotSmoothSpeed    = 15.f;

    _float  offsetY             = 0.2f;

    _bool   usePitchAutoZoom    = true;
    _float  pitchAutoZoomMax    = 1.0f;
    _float  pitchAutoZoomStartN = 0.75f;
    _float  pitchAutoZoomSmooth = 18.f;

    _float  startDistance       = 2.3f;
    _float  startPitchDeg       = -15.f;
    _float  startHeightOffset   = 0.4f;
};

namespace OrbitPresets
{
    inline OrbitProfile Field()
    {
        OrbitProfile p{};
        p.minDist           = 0.6f;
        p.maxDist           = 2.2f;
        p.pitchMin          = -20.f;
        p.pitchMax          = 55.f;
        p.rotSmoothSpeed    = 22.f;
        p.distSmoothSpeed   = 22.f;
        p.pivotSmoothSpeed  = 18.f;
        p.offsetY           = 0.2f;
        p.startDistance     = 2.3f;
        p.startPitchDeg     = -15.f;
        p.startHeightOffset = 0.4f;
        return p;
    }

    inline OrbitProfile Battle()
    {
        OrbitProfile p{};
        p.minDist          = 1.2f;
        p.maxDist          = 3.5f;
        p.pitchMin         = -35.f;
        p.pitchMax         = 70.f;
        p.rotSmoothSpeed   = 16.f;
        p.distSmoothSpeed  = 16.f;
        p.pivotSmoothSpeed = 14.f;
        p.offsetY          = 0.2f;
        p.startDistance     = 3.0f;
        p.startPitchDeg     = -18.f;
        p.startHeightOffset = 0.6f;
        return p;
    }

    inline OrbitProfile Get(OrbitPreset preset)
    {
        if (preset == OrbitPreset::Field) return Field();
        return Battle();
    }
}

NS_END