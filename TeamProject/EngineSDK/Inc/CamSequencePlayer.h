#pragma once

#include "Component.h"
#include "ICamEvaluator.h"
#include "CamEvaluator.h"
#include "Transform.h"
#include "Camera.h"
#include "VideoPlayer.h"

NS_BEGIN(Engine)
class CCamPosPerSegmentEvaluator; class CCamRotPerSegmentEvaluator; class CCamFovPerSegmentEvaluator;

class ENGINE_DLL CCamSequencePlayer final : public CComponent
{
    struct CamPlayerPlaybackState
    {
        _bool  playing = false;
        _float playTime = 0.f;
        _float timeScale = 1.f;
    };

    struct CamPlayerApplyState
    {
        _bool         applyEnabled = true;
        CTransform* transform{};
        CCamera* cam{};
        OBJECT_HANDLE spaceRefHandle{};
    };

    struct CamPlayerEvalState
    {
        _bool          dirty = true;
        CCamEvaluator* evaluator{};

        CCamPosPerSegmentEvaluator* pos{};
        CCamRotPerSegmentEvaluator* rot{};
        CCamFovPerSegmentEvaluator* fov{};
    };

private:
    const CamSeqDesc* seq{};
    CamPlayerPlaybackState playback{};
    CamPlayerApplyState    apply{};
    CamPlayerEvalState     eval{};

private:
    CCamSequencePlayer() DEFAULT;
    CCamSequencePlayer(const CCamSequencePlayer& rhs) : CComponent(rhs) {}
    ~CCamSequencePlayer() DEFAULT;

public:
    HRESULT Initialize_Prototype() override { return S_OK; }
    HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
    void   SetSequence(const CamSeqDesc* seq);
    const  CamSeqDesc* GetSequence() const { return seq; }

    void   Invalidate() { eval.dirty = true; }

    void   Play();
    void   Pause() { playback.playing = false; }
    void   Stop(_bool resetTime = true);

    bool   IsPlaying() const { return playback.playing; }

    void   SetTime(_float t);
    _float GetTime() const { return playback.playTime; }

    void   SetTimeScale(_float scale) { playback.timeScale = scale; }
    _float GetTimeScale() const { return playback.timeScale; }

    void   SetApplyEnabled(_bool enabled);
    _bool  IsApplyEnabled() const { return apply.applyEnabled; }

    void   SetSpaceRef(OBJECT_HANDLE handle) { apply.spaceRefHandle = handle; }
    void   ClearSpaceRef() { apply.spaceRefHandle.Reset(); }

    void   Update(_float dt);

private:
    void   RebuildIfNeeded();
    void   ApplyPose(const CamPose& pose);
    _float GetPlaybackDur() const;
    void   ApplyAtSampleTime(_float sampleTime);

public:
    static CCamSequencePlayer* Create();
    virtual void Free() override { Safe_Release(eval.evaluator); __super::Free(); }
    CComponent* Clone() override { return new CCamSequencePlayer(*this); }
};

NS_END
