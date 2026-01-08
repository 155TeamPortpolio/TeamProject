#include "pch.h"
#include "CamDirector.h"
#include "CamSequencePlayer.h"
#include "SequenceCam.h"
#include "CharacterController.h"
#include "GameInstance.h"
#include "OrbitCam.h"

IMPLEMENT_SINGLETON(CCamDirector)

CSequenceCam* CCamDirector::GetSequenceCam() const
{
    return static_cast<CSequenceCam*>(ObjectManger()->Request_Object(m_seqHandle));
}

void CCamDirector::Bind(CSequenceCam* sequenceCam)
{
    m_seqHandle = sequenceCam->Get_Handle();
}

_bool CCamDirector::Register(const string& key, const filesystem::path& path)
{
    SeqEntry entry{};
    entry.path = path;
    CamUtil::Load(entry.path, entry.seq);
    m_seqs[key] = move(entry);
    return true;
}

void CCamDirector::UnRegister(const string& key)
{
    if (m_playing.active && m_playing.key == key)
        StopAll(m_playing.defaultBlendOutSec);

    m_seqs.erase(key);
}

void CCamDirector::Update(_float dt)
{
    if (!m_playing.active) return;

    auto sequenceCam = GetSequenceCam();
    auto sequencePlayer = sequenceCam->Get_Component<CCamSequencePlayer>();

    if (m_playing.pendingStart)
    {
        m_playing.blendInRemain -= dt;

        if (m_playing.resetTimeOnStart)
            sequencePlayer->SetTime(0.f);

        if (m_playing.blendInRemain <= 0.f)
        {
            sequencePlayer->Play();
            m_playing.pendingStart = false;
        }
        return;
    }

    sequencePlayer->Update(dt);

    if (!sequencePlayer->IsPlaying())
        StopAll(m_playing.defaultBlendOutSec);
}

_uint CCamDirector::RequestSequence(const string& key, _float blendInSec, _bool resetTime, _float blendOutSec)
{
    if (m_playing.active)
        StopAll(blendOutSec);

    auto& entry       = m_seqs.at(key);
    auto  sequenceCam = GetSequenceCam();
    auto  seqPlayer   = sequenceCam->Get_Component<CCamSequencePlayer>();

    seqPlayer->SetSequence(&entry.seq);

    if (entry.seq.space == CamSpace::Local)
        seqPlayer->SetSpaceReference(m_spaceRefHandle);
    else
        seqPlayer->ClearSpaceReference();

    seqPlayer->SetApplyEnabled(true);

    if (resetTime)
        seqPlayer->SetTime(0.f);

    auto camComp = sequenceCam->Get_Component<CCamera>();

    if (entry.seq.space == CamSpace::Local)
    {
        auto refObj = ObjectManger()->Request_Object(m_spaceRefHandle);
        auto cc     = refObj->Get_Component<CCharacterController>();
        camComp->Set_ViewOffset({0.f, -cc->Get_HalfSize() * 0.25f, 0.f});
    }
    else
        camComp->Clear_ViewOffset();

    const _uint handle = CameraManager()->Push(camComp, blendInSec);

    m_playing.handle             = handle;
    m_playing.key                = key;
    m_playing.active             = true;
    m_playing.defaultBlendOutSec = blendOutSec;
    m_playing.pendingStart       = (blendInSec > 0.f);
    m_playing.blendInRemain      = blendInSec;
    m_playing.resetTimeOnStart   = resetTime;

    if (m_playing.pendingStart)
        seqPlayer->Pause();
    else
        seqPlayer->Play();

    return handle;
}

_bool CCamDirector::StopRequest(_uint handle, _float blendOutSec, _bool resetTime)
{
    const Matrix outWorld = *CameraManager()->Get_InversedViewMatrix();

    Vector3    outPos = outWorld.Translation();
    Quaternion outRot = Quaternion::CreateFromRotationMatrix(outWorld);
    outRot.Normalize();

    auto sequenceCam    = GetSequenceCam();
    auto sequencePlayer = sequenceCam->Get_Component<CCamSequencePlayer>();
    sequencePlayer->SetApplyEnabled(false);
    sequencePlayer->Stop(false);

    if (resetTime) sequencePlayer->SetTime(0.f);

    if (m_returnCamType != CamReturnType::None)
    {
        auto returnObj  = ObjectManger()->Request_Object(m_returnCamHandle);
        auto viewOffset = sequenceCam->Get_Component<CCamera>()->Get_ViewOffset();

        if (m_returnCamType == CamReturnType::OrbitCam)
        {
            returnObj->Get_Component<CCamera>()->Set_ViewOffset(viewOffset);

            const Vector3 ownerPos = outPos - viewOffset;
            static_cast<COrbitCam*>(returnObj)->SnapFromCamPose(ownerPos, outRot);
        }
    }

    const _bool ok = CameraManager()->Pop(handle, blendOutSec);
    m_playing = {};
    return ok;
}

void CCamDirector::StopAll(_float blendOutSec)
{
    StopRequest(m_playing.handle, blendOutSec, true);
}