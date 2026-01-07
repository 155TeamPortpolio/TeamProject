#include "Engine_Defines.h"
#include "Helper_Func.h"
#include "CamSequencePlayer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "ObjectMgr.h"

#include "CamPosPerSegmentEvaluator.h"
#include "CamRotPerSegmentEvaluator.h"
#include "CamFovPerSegmentEvaluator.h"

namespace
{
#define OBJ CGameInstance::GetInstance()->Get_ObjectMgr()
}

namespace
{
    float WrapLoopTime(float t, float dur)
    {
        float x = fmodf(t, dur);
        if (x < 0.f) x += dur;
        return x;
    }
    float WrapPingPongSampleTime(float t, float dur)
    {
        const float period = dur * 2.f;
        float x = fmodf(t, period);
        if (x < 0.f) x += period;
        if (x <= dur) return x;
        return period - x;
    }
    float CalcSampleTime(CamPlaybackMode mode, float playTime, float dur)
    {
        if (dur <= 1e-6f) return 0.f;
        switch (mode)
        {
        case CamPlaybackMode::Once:     return clamp(playTime, 0.f, dur);
        case CamPlaybackMode::Loop:     return WrapLoopTime(playTime, dur);
        case CamPlaybackMode::PingPong: return WrapPingPongSampleTime(playTime, dur);
        }
        return clamp(playTime, 0.f, dur);
    }
}

HRESULT CCamSequencePlayer::Initialize(COMPONENT_DESC* pArg)
{
    apply.transform = m_pOwner->Get_Component<CTransform>();
    apply.cam       = m_pOwner->Get_Component<CCamera>();

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
    playback.playTime = t;

    if (!target.seq || !apply.applyEnabled || target.seq->keyframes.empty()) return;               

    RebuildIfNeeded();

    const float sampleTime = CalcSampleTime(target.seq->playbackMode, playback.playTime, eval.evaluator->GetDuration());
    const float easedTime = eval.evaluator->RemapTimeBySegmentEasing(sampleTime);
    ApplyPose(eval.evaluator->Evaluate(easedTime));
}

void CCamSequencePlayer::SetApplyEnabled(_bool enabled)
{
    apply.applyEnabled = enabled;

    if (!apply.applyEnabled || !target.seq || target.seq->keyframes.empty()) return;

    RebuildIfNeeded();

    const float sampleTime = CalcSampleTime(target.seq->playbackMode, playback.playTime, eval.evaluator->GetDuration());
    const float easedTime  = eval.evaluator->RemapTimeBySegmentEasing(sampleTime);
    ApplyPose(eval.evaluator->Evaluate(easedTime));
}

void CCamSequencePlayer::Update(_float dt)
{
    if (!target.seq || !apply.applyEnabled || target.seq->keyframes.empty()) return;

    RebuildIfNeeded();

    const float dur = eval.evaluator->GetDuration();

    if (playback.playing)
    {
        playback.playTime += dt * playback.timeScale;

        if (dur <= 1e-6f)
        {
            playback.playTime = 0.f;
            playback.playing = false;
        }
        else if (target.seq->playbackMode == CamPlaybackMode::Once)
        {
            if (playback.playTime >= dur)
            {
                playback.playTime = dur;
                playback.playing = false;
            }
            else if (playback.playTime < 0.f)
                playback.playTime = 0.f;
        }
    }

    const float sampleTime = CalcSampleTime(target.seq->playbackMode, playback.playTime, dur);
    const float easedTime  = eval.evaluator->RemapTimeBySegmentEasing(sampleTime);
    ApplyPose(eval.evaluator->Evaluate(easedTime));
}

void CCamSequencePlayer::RebuildIfNeeded()
{
    if (!eval.dirty) return;
    eval.dirty = false;
    if (!target.seq) return;

    const auto& keys = target.seq->keyframes;
    if (keys.empty()) return;

    eval.pos->SetSequence(target.seq);
    eval.rot->SetSequence(target.seq);
    eval.fov->SetSequence(target.seq);

    eval.evaluator->Build(*target.seq);
}

void CCamSequencePlayer::ApplyPose(const CamPose& pose)
{
    if (target.seq && target.seq->space == CamSpace::Local)
    {
        auto refObj = OBJ->Request_Object(apply.spaceRefHandle);
        auto refTf  = refObj->Get_Component<CTransform>();

        Matrix refWorld = Matrix(refTf->Get_WorldMatrix());

        Vector3 refS{};
        Vector3 refT{};
        Quaternion refR = Quaternion::Identity;
        refWorld.Decompose(refS, refR, refT);
        refR.Normalize();

        Matrix refRT = Matrix::CreateFromQuaternion(refR) * Matrix::CreateTranslation(refT);

        const Matrix localM = Matrix::CreateFromQuaternion(pose.rot) * Matrix::CreateTranslation(pose.pos);
        Matrix worldM = localM * refRT;

        Vector3 s{}, t{};
        Quaternion r = Quaternion::Identity;
        worldM.Decompose(s, r, t);
        r.Normalize();

        apply.transform->Set_Pos(_vector3(t.x, t.y, t.z));
        apply.transform->Set_Quaternion(_vector4(r.x, r.y, r.z, r.w));
    }
    else
    {
        apply.transform->Set_Pos(pose.pos);
        apply.transform->Set_Quaternion(_vector4(pose.rot.x, pose.rot.y, pose.rot.z, pose.rot.w));
    }

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