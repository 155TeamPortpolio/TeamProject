#pragma once

NS_BEGIN(Engine)

enum class CamOrbitArcAngleMode 
{
    Shortest, Longest, Force180,
};
enum class CamOrbitArcRadiusMode 
{
    FixedStartRadius, FixedEndRadius, BlendRadius,
};
struct CamOrbitArcDesc
{
    _bool                 enabled{ false };

    _vector3              center{ 0.f, 0.f, 0.f };
    _vector3              axis{ 0.f, 1.f, 0.f };

    CamOrbitArcAngleMode  angleMode{ CamOrbitArcAngleMode::Shortest };
    _bool                 clockwise{ false };

    CamOrbitArcRadiusMode radiusMode{ CamOrbitArcRadiusMode::BlendRadius };

    void NormalizeAxis()
    {
        if (axis.LengthSquared() <= 1e-10f)
            axis = _vector3(0.f, 1.f, 0.f);
        axis.Normalize();
    }
};

NS_END