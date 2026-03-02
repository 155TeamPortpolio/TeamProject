#pragma once

NS_BEGIN(Engine)

enum class CamSpinCenterMode
{
	Origin, SpaceRefOrigin, Custom
};

struct CamOrbitSpinDesc
{
    _bool             enabled = false;

    CamSpinCenterMode centerMode = CamSpinCenterMode::Origin;
    _vector3          center{};                 // Custom일 때만 사용
    _vector3          axis = {0.f, 1.f, 0.f};   // 기본 Y

    _bool             keepHeight = true;        // "y 고정"
    _bool             clockwise = false;

    _int              extraTurns = 1;           // 360 = 1턴, 720 = 2턴
};

NS_END