#pragma once

NS_BEGIN(Client)

class CCamParryController
{
public:
    void  Reset();

    void  Begin(_float fovHold = 30.f, _float blendInSec = 0.08f, _float holdSec = 0.12f);
    void  End(_float blendOutSec = 0.65f);

    void  Update(_float dt);

    _bool IsHolding() const { return hold; }
    _bool IsBusy() const { return hold || blendIn || blendOut; }

private:
    _bool  hold = false;
    _bool  blendIn = false;
    _bool  blendOut = false;

    _float fovSaved = 0.f;
    _float fovHold = 30.f;

    Vector3 extSaved{};
    Vector3 extFrom{};
    Vector3 extTo{};

    _float blendTime = 0.f;
    _float blendInDur = 0.08f;
    _float blendOutDur = 0.65f;

    _float holdTime = 0.f;
    _float holdDur = 0.12f;

    EaseType easeIn = EaseType::OutCubic;
    EaseType easeOut = EaseType::InOutSine;
};


NS_END