#include "Engine_Defines.h"
#include "Helper_Func.h"
#include "CamSequencePlayer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "ObjectMgr.h"
#include "Animator3D.h"

#include "CamPosPerSegmentEvaluator.h"
#include "CamRotPerSegmentEvaluator.h"
#include "CamFovPerSegmentEvaluator.h"

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

    seq = nullptr;

    playback.playing = false;
    playback.playTime = 0.f;
    playback.timeScale = 1.f;

    apply.applyEnabled = true;
    eval.dirty = true;

    return S_OK;
}

void CCamSequencePlayer::SetSequence(const CamSeqDesc* inSeq)
{
    seq = inSeq;
    playback.playing = false;
    playback.playTime = 0.f;
    eval.dirty = true;
}

void CCamSequencePlayer::Play()
{
    if (!seq) return;

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

    if (!seq || !apply.applyEnabled) return;

    const _bool hasKeys = !seq->keyframes.empty();
    if (hasKeys) RebuildIfNeeded();

    const float dur = GetPlaybackDur();
    const float sampleTime = clamp(playback.playTime, 0.f, dur);

    if (playback.playTime != sampleTime)
        playback.playTime = sampleTime;

    ApplyAtSampleTime(sampleTime);
}

void CCamSequencePlayer::SetApplyEnabled(_bool enabled)
{
    apply.applyEnabled = enabled;

    if (!apply.applyEnabled)
        return;

    if (!seq)
        return;

    SetTime(playback.playTime);
}

void CCamSequencePlayer::Update(_float dt)
{
    if (!seq || !apply.applyEnabled)
        return;

    const _bool hasKeys = !seq->keyframes.empty();
    if (hasKeys)
        RebuildIfNeeded();

    const float dur = GetPlaybackDur();

    if (playback.playing)
    {
        playback.playTime += dt * playback.timeScale;

        if (dur <= 1e-6f)
        {
            playback.playTime = 0.f;
            playback.playing = false;
        }
        else
        {
            if (playback.playTime >= dur)
            {
                playback.playTime = dur;
                playback.playing = false;
            }
            else if (playback.playTime < 0.f)
                playback.playing = 0.f;
        }
    }

    const float sampleTime = clamp(playback.playTime, 0.f, dur);
    ApplyAtSampleTime(sampleTime);
}

void CCamSequencePlayer::RebuildIfNeeded()
{
    if (!eval.dirty) return;
    eval.dirty = false;
    if (!seq) return;

    const auto& keys = seq->keyframes;
    if (keys.empty()) return;

    eval.pos->SetSequence(seq);
    eval.rot->SetSequence(seq);
    eval.fov->SetSequence(seq);

    eval.evaluator->Build(*seq);
}

void CCamSequencePlayer::ApplyPose(const CamPose& pose)
{
    const CamSeqDesc* curSeq = seq;

    const _bool hasKeys = (curSeq && !curSeq->keyframes.empty());

    Matrix curWorld = Matrix(apply.transform->Get_WorldMatrix());

    Vector3 curS{};
    Vector3 curT{};
    Quaternion curR = Quaternion::Identity;
    curWorld.Decompose(curS, curR, curT);
    curR.Normalize();

    Vector3 offsetPos(pose.pos.x, pose.pos.y, pose.pos.z);

    Quaternion keyRot = pose.rot;
    keyRot.Normalize();

    Quaternion finalRot = hasKeys ? keyRot : curR;

    Quaternion spaceRefR = Quaternion::Identity;
    Vector3 spaceRefT = Vector3::Zero;
    _bool hasSpaceRef = false;

    const _bool needSpaceRef = (curSeq && curSeq->space == CamSpace::Local);

    if (needSpaceRef && apply.spaceRefHandle.isValid())
    {
        auto refObj = ObjectManager()->Request_Object(apply.spaceRefHandle);
        auto refTf = refObj->Get_Component<CTransform>();

        Matrix refWorld = Matrix(refTf->Get_WorldMatrix());

        Vector3 rs{};
        Vector3 rt{};
        Quaternion rr = Quaternion::Identity;
        refWorld.Decompose(rs, rr, rt);
        rr.Normalize();

        spaceRefR = rr;
        spaceRefT = rt;
        hasSpaceRef = true;
    }

    Vector3 basePos = curT;
    if (needSpaceRef && hasSpaceRef) basePos = spaceRefT;

    Vector3 offsetWorld = offsetPos;
    if (needSpaceRef && hasSpaceRef)
    {
        Matrix rM = Matrix::CreateFromQuaternion(spaceRefR);
        offsetWorld = Vector3::Transform(offsetPos, rM);
    }

    Vector3 finalPos = basePos + offsetWorld;

    if (needSpaceRef && hasKeys && hasSpaceRef)
    {
        finalRot = finalRot * spaceRefR;
        finalRot.Normalize();
    }

    apply.transform->Set_Pos(_vector3(finalPos.x, finalPos.y, finalPos.z));
    apply.transform->Set_Quaternion(_vector4(finalRot.x, finalRot.y, finalRot.z, finalRot.w));

    apply.cam->Set_FOV(pose.fov);
}

_float CCamSequencePlayer::GetPlaybackDur() const
{
    if (!seq) return 0.f;

    if (!seq->keyframes.empty())
        return eval.evaluator ? eval.evaluator->GetDuration() : seq->GetDuration();

    return seq->refAnimDurSec;
}

void CCamSequencePlayer::ApplyAtSampleTime(_float sampleTime)
{
    CamPose pose{};

    if (!seq->keyframes.empty() && eval.evaluator)
    {
        const float easedTime = eval.evaluator->RemapTimeBySegmentEasing(sampleTime);
        pose = eval.evaluator->Evaluate(easedTime);
    }
    else
    {
        pose.pos = {0.f, 0.f, 0.f};
        pose.rot = Quaternion::Identity;
        pose.fov = apply.cam->Get_FOV();
        pose.roll = 0.f;
    }

    ApplyPose(pose);
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
