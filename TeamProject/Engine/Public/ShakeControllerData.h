#pragma once

NS_BEGIN(Engine)

enum class CamShakeType
{
    TapSoft,
    HitLight,
    HitNormal,
    HitHeavy,
    HitCrit,

    ExplosionSmall,
    ExplosionBig,

    LandingLight,
    LandingHeavy,

    Dash,

    End
};

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
    _uint  seed{};
    _float elapsed{};

    _float attackSec{};
    _float sustainSec{};
    _float decaySec{};

    _float kickDur{};
    _float kickRotRad{};
    _float kickPos{};

    _float noiseRotRad{};
    _float noisePos{};
    _float noiseFreq{};

    _float yawSign{};
    _float rollSign{};
    _float sideSign{};

    _float p0{};
    _float p1{};
    _float p2{};
};

NS_END