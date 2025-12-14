#include "Engine_Defines.h"
#include "CamSequencePlayer.h"
#include "GameObject.h"

#include "CamPosLinearEvaluator.h"
#include "CamPosCatmullRomEvaluator.h"
#include "CamPosCentripetalEvaluator.h"
#include "CamPosBSplineEvaluator.h"
#include "CamPosHermiteEvaluator.h"

#include "CamRotSlerpEvaluator.h"
#include "CamRotSquadEvaluator.h"

#include "CamFovLinearEvaluator.h"
#include "CamFovSmoothEvaluator.h"

HRESULT CCamSequencePlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamSequencePlayer::Initialize(COMPONENT_DESC* pArg)
{
    apply.transform = m_pOwner->Get_Component<CTransform>();
    apply.cam = m_pOwner->Get_Component<CCamera>();

    eval.evaluator = CCamEvaluator::Create();
    eval.evaluator->SetPosEvaluator(CCamPosLinearEvaluator::Create());
    eval.evaluator->SetRotEvaluator(CCamRotSlerpEvaluator::Create());
    eval.evaluator->SetFovEvaluator(CCamFovLinearEvaluator::Create());

    target.seq         = nullptr;
    playback.playing   = false;
    playback.playTime  = 0.f;
    playback.timeScale = 1.f;

    apply.applyEnabled = true;
    eval.dirty         = true;

    return S_OK;
}

void CCamSequencePlayer::SetSequence(const CamSequenceDesc* seq)
{
    target.seq        = seq;
    playback.playing  = false;
    playback.playTime = 0.f;
    eval.dirty        = true;
}

void CCamSequencePlayer::Play()
{
    if (!target.seq) return;

    RebuildIfNeeded();
    playback.playing = true;
}

void CCamSequencePlayer::Stop(_bool resetTime)
{
    playback.playing = false;
    if (resetTime)
        playback.playTime = 0.f;
}

void CCamSequencePlayer::SetTime(_float t)
{
    playback.playTime = max(0.f, t);

    if (!target.seq) return;

    const auto& keys = target.seq->keyframes;
    if (keys.empty()) return;

    RebuildIfNeeded();

    if (apply.applyEnabled)
        ApplyPose(eval.evaluator->Evaluate(playback.playTime));
}

void CCamSequencePlayer::SetApplyEnabled(_bool enabled)
{
    apply.applyEnabled = enabled;

    if (!apply.applyEnabled || !target.seq)
        return;

    const auto& keys = target.seq->keyframes;
    if (keys.empty())
        return;

    RebuildIfNeeded();
    ApplyPose(eval.evaluator->Evaluate(playback.playTime));
}

void CCamSequencePlayer::Update(_float dt)
{
    if (!target.seq || !apply.applyEnabled) return;

    const auto& keys = target.seq->keyframes;
    if (keys.empty()) return;

    RebuildIfNeeded();

    if (playback.playing)
        playback.playTime += dt * playback.timeScale;

    ApplyPose(eval.evaluator->Evaluate(playback.playTime));
}

void CCamSequencePlayer::RebuildIfNeeded()
{
    if (!eval.dirty) return;

    eval.dirty = false;

    if (!target.seq) return;

    const auto& keys = target.seq->keyframes;
    if (keys.empty()) return;

    if (!eval.evaluator)
        eval.evaluator = CCamEvaluator::Create();

    const size_t keyCount = keys.size();

    if (keyCount == 1)
    {
        eval.evaluator->SetPosEvaluator(CCamPosLinearEvaluator::Create());
        eval.evaluator->SetRotEvaluator(CCamRotSlerpEvaluator::Create());
        eval.evaluator->SetFovEvaluator(CCamFovLinearEvaluator::Create());

        const bool ok = eval.evaluator->Build(*target.seq);
        assert(ok);
        return;
    }

    switch (target.seq->posInterp)
    {
    case CamPosInterp::Linear:
        eval.evaluator->SetPosEvaluator(CCamPosLinearEvaluator::Create());
        break;
    case CamPosInterp::CatmullRom:
        eval.evaluator->SetPosEvaluator(CCamPosCatmullRomEvaluator::Create());
        break;
    case CamPosInterp::Centripetal:
        eval.evaluator->SetPosEvaluator(CCamPosCentripetalEvaluator::Create());
        break;
    case CamPosInterp::BSpline:
        eval.evaluator->SetPosEvaluator(CCamPosBSplineEvaluator::Create());
        break;
    case CamPosInterp::Hermite:
        eval.evaluator->SetPosEvaluator(CCamPosHermiteEvaluator::Create());
        break;
    }

    switch (target.seq->rotInterp)
    {
    case CamRotInterp::Slerp:
        eval.evaluator->SetRotEvaluator(CCamRotSlerpEvaluator::Create());
        break;
    case CamRotInterp::Squad:
        eval.evaluator->SetRotEvaluator(CCamRotSquadEvaluator::Create());
        break;
    }

    switch (target.seq->fovInterp)
    {
    case CamFovInterp::Linear:
        eval.evaluator->SetFovEvaluator(CCamFovLinearEvaluator::Create());
        break;
    case CamFovInterp::Smooth:
        eval.evaluator->SetFovEvaluator(CCamFovSmoothEvaluator::Create());
        break;
    }

    const bool ok = eval.evaluator->Build(*target.seq);
    assert(ok);
}

void CCamSequencePlayer::ApplyPose(const CamPose& pose)
{
    apply.transform->Set_Pos(pose.pos);

    const _vector4 quat{ pose.rot.x, pose.rot.y, pose.rot.z, pose.rot.w };
    apply.transform->Set_Quaternion(quat);

    if (apply.cam)
        apply.cam->Set_FOV(pose.fov);
}

CCamSequencePlayer* CCamSequencePlayer::Create()
{
    auto inst = new CCamSequencePlayer();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("CamSequencePlayer Create Failed : CamSequencePlayer");
        Safe_Release(inst);
    }
    return inst;
}

void CCamSequencePlayer::Free()
{
    __super::Free();
    Safe_Release(eval.evaluator);
}