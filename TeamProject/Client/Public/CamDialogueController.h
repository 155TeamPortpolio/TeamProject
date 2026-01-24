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

    void  Begin(_float targetFov = 30.f, _float blendSec = 0.5f, _float assumedPartnerFrontDist = 1.f);
    void  End(_float blendSec = 0.5f);

    void  Update(_float dt, CCamera* cam, COrbitCam* orbit, CTransform* focusTr);

    _bool IsHolding() const { return m_state.holding; }
    _bool IsBusy()    const { return m_state.holding || m_state.blending; }

private:
    void  BeginBlend(_float fromFov, _float toFov, _float fromPivotDist, _float toPivotDist, _float dur);

private:
    struct State
    {
        _bool  holding = false;
        _bool  blending = false;
        _bool  restoring = false;

        _float savedFov = 0.f;

        _float holdFov = 30.f;

        _float assumedPartnerFrontDist = 1.f;
        _float holdPivotDist = 0.5f;

        _float time = 0.f;
        _float dur = 0.5f;

        _float fromFov = 0.f;
        _float toFov = 0.f;

        _float fromPivotDist = -1.f;
        _float toPivotDist = 0.f;

        _float lastPivotDist = 0.f;

        Vector3 fromPivotWorld{};
        Vector3 toPivotWorld{};
        _bool   blendInit = false;

        EaseType ease = EaseType::OutSine;
    };

    State m_state{};
};

NS_END