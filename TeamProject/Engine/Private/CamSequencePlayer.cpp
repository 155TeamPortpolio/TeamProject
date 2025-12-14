#include "Engine_Defines.h"
#include "CamSequencePlayer.h"
#include "GameObject.h"

#include "CamPosLinearEvaluator.h"
#include "CamPosCatmullRomEvaluator.h"
#include "CamPosCentripetalEvaluator.h"

#include "CamRotSlerpEvaluator.h"

#include "CamFovLinearEvaluator.h"
#include "CamFovSmoothEvaluator.h"

HRESULT CamSequencePlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CamSequencePlayer::Initialize(COMPONENT_DESC* pArg)
{
    apply.transform = m_pOwner->Get_Component<CTransform>();
    apply.cam = m_pOwner->Get_Component<CCamera>();

    eval.evaluator = CamEvaluator::Create();
    eval.evaluator->SetPosEvaluator(CamPosLinearEvaluator::Create());
    eval.evaluator->SetRotEvaluator(CamRotSlerpEvaluator::Create());
    eval.evaluator->SetFovEvaluator(CamFovLinearEvaluator::Create());

    target.seq         = nullptr;
    playback.playing   = false;
    playback.playTime  = 0.f;
    playback.timeScale = 1.f;

    apply.applyEnabled = true;
    eval.dirty         = true;

    return S_OK;
}

void CamSequencePlayer::SetSequence(const CamSequenceDesc* seq)
{
    target.seq        = seq;
    playback.playing  = false;
    playback.playTime = 0.f;
    eval.dirty        = true;
}

void CamSequencePlayer::Play()
{
    if (!target.seq) return;

    RebuildIfNeeded();
    playback.playing = true;
}

void CamSequencePlayer::Stop(_bool resetTime)
{
    playback.playing = false;
    if (resetTime)
        playback.playTime = 0.f;
}

void CamSequencePlayer::SetTime(_float t)
{
    playback.playTime = max(0.f, t);

    if (!target.seq) return;

    const auto& keys = target.seq->keyframes;
    if (keys.empty()) return;

    RebuildIfNeeded();

    if (apply.applyEnabled)
        ApplyPose(eval.evaluator->Evaluate(playback.playTime));
}

void CamSequencePlayer::SetApplyEnabled(_bool enabled)
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

void CamSequencePlayer::Update(_float dt)
{
    if (!target.seq || !apply.applyEnabled) return;

    const auto& keys = target.seq->keyframes;
    if (keys.empty()) return;

    RebuildIfNeeded();

    if (playback.playing)
        playback.playTime += dt * playback.timeScale;

    ApplyPose(eval.evaluator->Evaluate(playback.playTime));
}

void CamSequencePlayer::RebuildIfNeeded()
{
    if (!eval.dirty) return;

    eval.dirty = false;

    if (!target.seq) return;

    const auto& keys = target.seq->keyframes;
    if (keys.empty())
        return;

    if (!eval.evaluator)
        eval.evaluator = CamEvaluator::Create();

    switch (target.seq->posInterp)
    {
    case CamPosInterp::Linear:
        eval.evaluator->SetPosEvaluator(CamPosLinearEvaluator::Create());
        break;
    case CamPosInterp::CatmullRom:
        eval.evaluator->SetPosEvaluator(CamPosCatmullRomEvaluator::Create());
        break;
    case CamPosInterp::Centripetal:
        eval.evaluator->SetPosEvaluator(CamPosCentripetalEvaluator::Create());
        break;
    }

    switch (target.seq->rotInterp)
    {
    case CamRotInterp::Slerp:
        eval.evaluator->SetRotEvaluator(CamRotSlerpEvaluator::Create());
        break;
    case CamRotInterp::Squad:
        eval.evaluator->SetRotEvaluator(CamRotSlerpEvaluator::Create());
        break;
    }

    switch (target.seq->fovInterp)
    {
    case CamFovInterp::Linear:
        eval.evaluator->SetFovEvaluator(CamFovLinearEvaluator::Create());
        break;
    case CamFovInterp::Smooth:
        eval.evaluator->SetFovEvaluator(CamFovSmoothEvaluator::Create());
        break;
    }

    const bool ok = eval.evaluator->Build(*target.seq);
    assert(ok);
}

void CamSequencePlayer::ApplyPose(const CamPose& pose)
{
    apply.transform->Set_Pos(pose.pos);
    const Quaternion rotation = pose.rot;
    _vector3 forward = _vector3::Transform(_vector3(0.f, 0.f, 1.f), rotation);

    if (forward.LengthSquared() <= 1e-8f)
        forward = _vector3{ 0.f, 0.f, 1.f };
    else
        forward.Normalize();

    apply.transform->LookAt(pose.pos + forward);

    if (apply.cam)
        apply.cam->Set_FOV(pose.fov);
}

CamSequencePlayer* CamSequencePlayer::Create()
{
    auto inst = new CamSequencePlayer();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("CamSequencePlayer Create Failed : CamSequencePlayer");
        Safe_Release(inst);
    }
    return inst;
}

void CamSequencePlayer::Free()
{
    __super::Free();
    Safe_Release(eval.evaluator);
}