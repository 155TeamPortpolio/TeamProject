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

void CCamSequencePlayer::SyncSpaceRefAnimatorTime(_float sampleTime)
{
    if (!target.seq) return;

    const CamBoneAttachDesc& bone = target.seq->boneAttach;
    if (!bone.enabled) return;
    if (!apply.spaceRefHandle.isValid()) return;

    auto refObj = ObjectManager()->Request_Object(apply.spaceRefHandle);
    if (!refObj) return;

    auto anim = refObj->Get_Component<CAnimator3D>();
    if (!anim) return;

    anim->Set_TimeSec(sampleTime);
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
    SyncSpaceRefAnimatorTime(sampleTime);

    const float easedTime = eval.evaluator->RemapTimeBySegmentEasing(sampleTime);
    ApplyPose(eval.evaluator->Evaluate(easedTime));
}

void CCamSequencePlayer::SetApplyEnabled(_bool enabled)
{
    apply.applyEnabled = enabled;

    if (!apply.applyEnabled || !target.seq || target.seq->keyframes.empty()) return;

    RebuildIfNeeded();

    const float sampleTime = CalcSampleTime(target.seq->playbackMode, playback.playTime, eval.evaluator->GetDuration());
    SyncSpaceRefAnimatorTime(sampleTime);

    const float easedTime = eval.evaluator->RemapTimeBySegmentEasing(sampleTime);
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
    SyncSpaceRefAnimatorTime(sampleTime);

    const float easedTime = eval.evaluator->RemapTimeBySegmentEasing(sampleTime);
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
    const CamSequenceDesc* seq = target.seq;
    const CamBoneAttachDesc* boneDesc = seq ? &seq->boneAttach : nullptr;

    Vector3 worldPos(pose.pos.x, pose.pos.y, pose.pos.z);
    Quaternion worldRot = pose.rot;
    worldRot.Normalize();

    Matrix spaceRefRT = Matrix::Identity;
    Matrix boneRT = Matrix::Identity;
    bool hasSpaceRefRT = false;
    bool hasBoneRT = false;

    const bool needSpaceRef = (seq && (seq->space == CamSpace::Local));
    const bool needBone = (boneDesc && boneDesc->enabled && !boneDesc->boneName.empty());

    if (needSpaceRef || needBone)
    {
        auto refObj = ObjectManager()->Request_Object(apply.spaceRefHandle);
        auto refTf = refObj->Get_Component<CTransform>();

        Matrix refWorld = Matrix(refTf->Get_WorldMatrix());

        Vector3 refS{};
        Vector3 refT{};
        Quaternion refR = Quaternion::Identity;
        refWorld.Decompose(refS, refR, refT);
        refR.Normalize();

        spaceRefRT = Matrix::CreateFromQuaternion(refR) * Matrix::CreateTranslation(refT);
        hasSpaceRefRT = true;

        if (needBone)
        {
            auto anim = refObj->Get_Component<CAnimator3D>();

            Matrix boneWorld = Matrix(anim->Get_BoneMatrix(CAnimator3D::BoneSpace::WORLD, boneDesc->boneName));

            Vector3 bs{};
            Vector3 bt{};
            Quaternion br = Quaternion::Identity;
            boneWorld.Decompose(bs, br, bt);
            br.Normalize();

            boneRT = Matrix::CreateFromQuaternion(br) * Matrix::CreateTranslation(bt);
            hasBoneRT = true;
        }
    }

    if (seq && seq->space == CamSpace::Local)
    {
        Matrix parentRT = spaceRefRT;

        if (boneDesc && boneDesc->enabled && boneDesc->mode == CamBoneMode::Parent && hasBoneRT)
            parentRT = boneRT;

        const Matrix localM = Matrix::CreateFromQuaternion(worldRot) * Matrix::CreateTranslation(worldPos);
        Matrix worldM = localM * parentRT;

        Vector3 s{}, t{};
        Quaternion r = Quaternion::Identity;
        worldM.Decompose(s, r, t);
        r.Normalize();

        worldPos = t;
        worldRot = r;
    }

    Quaternion finalRot = worldRot;

    if (boneDesc && boneDesc->enabled && boneDesc->mode == CamBoneMode::LookAt)
    {
        Vector3 lookTarget = worldPos;

        if (hasBoneRT) lookTarget = boneRT.Translation();
        else if (hasSpaceRefRT) lookTarget = spaceRefRT.Translation();

        Vector3 dir = worldPos - lookTarget;

        if (dir.LengthSquared() > 1e-8f)
        {
            dir.Normalize();

            Vector3 up(0.f, 1.f, 0.f);
            if (fabsf(dir.Dot(up)) > 0.98f) up = Vector3(0.f, 0.f, 1.f);

            Matrix lookM = Matrix::CreateWorld(Vector3::Zero, dir, up);
            Quaternion lookRot = Quaternion::CreateFromRotationMatrix(lookM);
            lookRot.Normalize();

            Quaternion rollQ = Quaternion::CreateFromAxisAngle(dir, pose.roll);

            finalRot = rollQ * lookRot;
            finalRot.Normalize();
        }
    }

    apply.transform->Set_Pos(_vector3(worldPos.x, worldPos.y, worldPos.z));
    apply.transform->Set_Quaternion(_vector4(finalRot.x, finalRot.y, finalRot.z, finalRot.w));

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