#pragma once

NS_BEGIN(Engine)

enum class CamZoomType
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

NS_END