#pragma once

NS_BEGIN(Engine)
// ---------------------- Shake ---------------------------
struct CamShakePreset
{
    _float kickRotDeg{};
    _float kickPos{};
    _float kickDur{};

    _float noiseRotDeg{};
    _float noisePos{};
    _float noiseFreq{};

    _float dur{};
    _float fadeOutSec{};
};

struct CamShakeInstance
{
    _uint    seed{};
    _float   elapsed{};
    uint8_t  axisMask{0x7};

    _float   attackSec{};
    _float   sustainSec{};
    _float   decaySec{};

    EaseType attackEase = EaseType::None;
    EaseType decayEase = EaseType::None;

    _float   kickDur{};
    _float   kickRotRad{};
    _float   kickPos{};

    _float   noiseRotRad{};
    _float   noisePos{};
    _float   noiseFreq{};

    _float   yawSign{};
    _float   rollSign{};
    _float   sideSign{};

    _float   p0{}, p1{}, p2{};
};

// ----------------- Zoom ------------------

struct CamZoomPreset
{
    _float amountDeg{};
    _float attackSec{};
    _float releaseSec{};
};

struct CamZoomInstance
{
    _float amountDeg{};
    _float attackSec{};
    _float releaseSec{};
    _float elapsed{};
};

// -------------------------

enum class CamShakeAxis : uint8_t
{
    None  = 0,
    Pitch = 0x1,
    Yaw   = 0x2,
    Roll  = 0x4,
    All   = 0x7,
};

constexpr CamShakeAxis operator|(CamShakeAxis a, CamShakeAxis b)
{
    return (CamShakeAxis)((uint8_t)a | (uint8_t)b);
}

constexpr CamShakeAxis operator&(CamShakeAxis a, CamShakeAxis b)
{
    return (CamShakeAxis)((uint8_t)a & (uint8_t)b);
}

constexpr CamShakeAxis& operator|=(CamShakeAxis& a, CamShakeAxis b)
{
    a = a | b;
    return a;
}

constexpr bool HasAxis(CamShakeAxis mask, CamShakeAxis axis)
{
    return (((uint8_t)mask & (uint8_t)axis) != 0);
}

constexpr uint8_t ToAxisMask(CamShakeAxis axes)
{
    return (uint8_t)axes;
}

NS_END