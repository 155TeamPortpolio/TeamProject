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

    void  Begin(_float targetFov = 30.f, _float blendSec = 0.5f);
    void  End(_float blendSec = 0.5f);

    void  Update(_float dt, CCamera* cam, COrbitCam* orbit, CTransform* focusTr);

    _bool IsHolding() const { return holding; }
    _bool IsBusy()    const { return holding || blending; }

private:
    _bool         holding = false;
    _bool         blending = false;
    _bool         restoring = false;

    _float        savedFov = 0.f;
    _float        holdFov = 30.f;

    OBJECT_HANDLE partnerHandle{};

    _float        time = 0.f;
    _float        dur = 0.5f;

    _float        fromFov = 0.f;
    _float        toFov = 0.f;

    Vector3       fromPivotWorld{};
    Vector3       toPivotWorld{};
    _bool         blendInit = false;

    EaseType      ease = EaseType::InOutSine;

    _float        maxPivotOffset = 0.8f;
    _float        faceYOffsetMul = 0.85f;
};

NS_END