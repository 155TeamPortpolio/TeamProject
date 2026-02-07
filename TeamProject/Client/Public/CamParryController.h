#pragma once

NS_BEGIN(Client)

class CCamParryController
{
public:
    void Reset();

    void Begin(OBJECT_HANDLE parryTarget, _float fovHold = 30.f, _float blendInSec = 0.08f, _float holdSec = 0.12f);
    void End(_float blendOutSec = 0.65f);

    void Update(_float dt);

    _bool IsHolding() const { return hold; }
    _bool IsBusy() const { return hold || blend || restore; }

private:
    _bool hold = false;
    _bool blend = false;
    _bool restore = false;

    OBJECT_HANDLE target{};

    _float fovSaved = 0.f;
    _float fovHold = 30.f;

    Vector3 extSaved{};
    Vector3 extFrom{};
    Vector3 extTo{};
    _float  maxPivotOff = 0.85f;

    _float blendTime = 0.f;
    _float blendDur = 0.08f;

    _float holdTime = 0.f;
    _float holdDur = 0.12f;

    _float fovFrom = 0.f;
    _float fovTo = 0.f;

    EaseType easeIn = EaseType::OutCubic;
    EaseType easeOut = EaseType::InOutSine;

    _bool orbitParryFlagsApplied = false;
};


NS_END