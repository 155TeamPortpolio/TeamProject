#include "Engine_Defines.h"
#include "CamSequencePlayer.h"
#include "GameObject.h"

#include "CamPosPerSegmentEvaluator.h"
#include "CamRotPerSegmentEvaluator.h"
#include "CamFovPerSegmentEvaluator.h"

CCamSequencePlayer::CCamSequencePlayer(const CCamSequencePlayer& rhs)
    : CComponent(rhs)
{
    target   = {};
    playback = {};
    apply    = {};
    eval     = {};
}

HRESULT CCamSequencePlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamSequencePlayer::Initialize(COMPONENT_DESC* pArg)
{
    apply.transform = m_pOwner->Get_Component<CTransform>();
    apply.cam = m_pOwner->Get_Component<CCamera>();

    if (!eval.evaluator)
        eval.evaluator = CCamEvaluator::Create();

    eval.pos = CCamPosPerSegmentEvaluator::Create();
    eval.rot = CCamRotPerSegmentEvaluator::Create();
    eval.fov = CCamFovPerSegmentEvaluator::Create();

    eval.evaluator->SetPosEvaluator(eval.pos);
    eval.evaluator->SetRotEvaluator(eval.rot);
    eval.evaluator->SetFovEvaluator(eval.fov);

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
    if (!target.seq)
    {
        playback.playTime = max(0.f, t);
        return;
    }

    const float duration = target.seq->GetDuration();

    playback.playTime = max(0.f, t);
    if (duration > 1e-6f && playback.playTime > duration)
        playback.playTime = duration;

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

    const float duration = target.seq->GetDuration();

    if (playback.playing)
    {
        playback.playTime += dt * playback.timeScale;

        if (duration <= 1e-6f)
        {
            playback.playTime = 0.f;
            playback.playing = false;
        }
        else
        {
            if (target.seq->playbackMode == CamPlaybackMode::Loop)
            {
                playback.playTime = fmodf(playback.playTime, duration);
                if (playback.playTime < 0.f) playback.playTime += duration;
            }
            else
            {
                if (playback.playTime >= duration)
                {
                    playback.playTime = duration;
                    playback.playing = false;
                }
                else if (playback.playTime < 0.f)
                {
                    playback.playTime = 0.f;
                }
            }
        }
    }

    ApplyPose(eval.evaluator->Evaluate(playback.playTime));
}

void CCamSequencePlayer::RebuildIfNeeded()
{
    if (!eval.dirty) return;

    eval.dirty = false;

    if (!target.seq) return;

    const auto& keys = target.seq->keyframes;
    if (keys.empty()) return;

    assert(eval.evaluator);
    assert(eval.pos);
    assert(eval.rot);
    assert(eval.fov);

    eval.pos->SetSequence(target.seq);
    eval.rot->SetSequence(target.seq);
    eval.fov->SetSequence(target.seq);

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
    Safe_Release(eval.evaluator);
    Safe_Release(eval.pos);
    Safe_Release(eval.rot);
    Safe_Release(eval.fov);

    eval.pos = nullptr;
    eval.rot = nullptr;
    eval.fov = nullptr;

    __super::Free();
}