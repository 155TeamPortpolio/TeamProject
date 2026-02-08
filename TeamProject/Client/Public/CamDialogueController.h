#pragma once

NS_BEGIN(Engine)
class CCamera; class CTransform;
NS_END

NS_BEGIN(Client)
class COrbitCam;

class CCamDialogueController
{
public:
    void  Reset();

    void  Begin(_float fovHold = 30.f, _float blendSec = 0.5f);
    void  End(_float blendSec = 0.5f);

    void  Update(_float dt, CCamera* cam, COrbitCam* orbit, CTransform* focusTr);

    _bool IsHolding() const { return hold; }
    _bool IsBusy() const { return hold || blend; }

private:
    _bool  hold = false;
    _bool  blend = false;
    _bool  restore = false;

    _float fovSaved = 0.f;
    _float fovHold = 30.f;

    OBJECT_HANDLE partner{};

    _float blendTime = 0.f;
    _float blendDur = 0.5f;

    _float fovFrom = 0.f;
    _float fovTo = 0.f;

    Vector3 pivotFrom{};
    Vector3 pivotTo{};
    _bool   hasBlendInit = false;

    EaseType ease = EaseType::InOutQuad;

    _float maxPivotOff = 0.8f;
    _float faceYOffsetMul = 0.85f;

    Vector3 sideDir{};
    _bool   sideInit   = false;
    _int    sideSign   = 0;
    _bool   yawInit    = false;
    _float  yawSm      = 0.f;
    _float  wSm        = 0.f;
    _float  yawTau     = 0.12f;
    _float  wTau       = 0.10f;
    _float  yawBiasDeg = 15.f;
};

NS_END
