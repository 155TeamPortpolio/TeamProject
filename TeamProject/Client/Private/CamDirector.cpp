#include "pch.h"
#include "CamDirector.h"
#include "CamSequencePlayer.h"
#include "SequenceCam.h"
#include "CharacterController.h"
#include "GameInstance.h"
#include "OrbitCam.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCamDirector)

CGameObject* CCamDirector::GetCamObj(CamType type) const
{
    return ObjectManager()->Request_Object(m_camHandles[ENUM(type)]);
}

CGameObject* CCamDirector::GetSeqObj() const
{
    return GetCamObj(CamType::Sequence);
}

CGameObject* CCamDirector::GetOrbitObj() const
{
    return GetCamObj(CamType::Orbit);
}

void CCamDirector::SetCam(CamType type, OBJECT_HANDLE handle)
{
    m_camHandles[ENUM(type)] = handle;
}

void CCamDirector::SetSpaceRef(OBJECT_HANDLE handle)
{
    m_spaceRefHandle = handle;
}

void CCamDirector::SetReturnCam(CamType type)
{
    m_returnCamType = type;
}

void CCamDirector::ClearReturnCam()
{
    m_returnCamType = CamType::None;
}

void CCamDirector::ClearCam(CamType type)
{
    m_camHandles[ENUM(type)].Reset();
}

OBJECT_HANDLE CCamDirector::GetCamHandle(CamType type) const
{
    return m_camHandles[ENUM(type)];
}

COrbitCam* CCamDirector::GetOrbitCam() const
{
    return static_cast<COrbitCam*>(GetOrbitObj());
}

CSequenceCam* CCamDirector::GetSeqCam() const
{
    return static_cast<CSequenceCam*>(GetSeqObj());
}

_bool CCamDirector::Register(const string& key, const filesystem::path& path)
{
    CamSequenceRequestDesc req{};
    return Register(key, path, req);
}

_bool CCamDirector::Register(const string& key, const filesystem::path& path, const CamSequenceRequestDesc& defaultReq)
{
    CamDirectorSeqEntry entry{};
    entry.path = path;
    CamUtil::Load(entry.path, entry.seqDesc);
    entry.defaultReq = defaultReq;
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

    auto seqPlayer = GetSeqObj()->Get_Component<CCamSequencePlayer>();

    if (m_playing.pendingStart)
    {
        m_playing.blendInRemain -= dt;

        if (m_playing.blendInRemain <= 0.f)
        {
            seqPlayer->Play();
            m_playing.pendingStart = false;
        }
        return;
    }

    seqPlayer->Update(dt);

    if (!seqPlayer->IsPlaying()) StopAll(m_playing.defaultBlendOutSec);
}

_uint CCamDirector::RequestSequence(const string& key)
{
    auto& entry = m_seqs.at(key);
    return RequestSequence(key, entry.defaultReq);
}

_uint CCamDirector::RequestSequence(const string& key, _float blendInSec, _bool resetTime, _float blendOutSec)
{
    CamSequenceRequestDesc req{};
    req.blendInSec = blendInSec;
    req.blendOutSec = blendOutSec;
    req.resetTime = resetTime;
    req.returnMode = CamReturnMode::SnapToEnd;
    req.returnCamType = m_returnCamType;
    return RequestSequence(key, req);
}

_uint CCamDirector::RequestSequence(const string& key, const CamSequenceRequestDesc& req)
{
    if (m_playing.active) StopAll(req.blendOutSec);

    auto& entry = m_seqs.at(key);

    CamType resolvedReturnCamType = req.returnCamType;
    if (resolvedReturnCamType == CamType::None) resolvedReturnCamType = m_returnCamType;

    if (req.returnMode == CamReturnMode::RestorePrev && resolvedReturnCamType == CamType::Orbit)
        GetOrbitCam()->CaptureSnapshot(m_playing.prevOrbit);

    auto seqObj = GetSeqObj();
    auto seqPlayer = seqObj->Get_Component<CCamSequencePlayer>();
    auto seqCam = seqObj->Get_Component<CCamera>();

    seqPlayer->SetSequence(&entry.seqDesc);

    if (entry.seqDesc.space == CamSpace::Local) seqPlayer->SetSpaceReference(m_spaceRefHandle);
    else seqPlayer->ClearSpaceReference();

    seqPlayer->SetApplyEnabled(true);

    if (req.resetTime) seqPlayer->SetTime(0.f);

    const _uint handle = CameraManager()->Push(seqCam, req.blendInSec);

    m_playing.handle = handle;
    m_playing.key = key;
    m_playing.active = true;
    m_playing.defaultBlendOutSec = req.blendOutSec;
    m_playing.pendingStart = (req.blendInSec > 0.f);
    m_playing.blendInRemain = req.blendInSec;

    m_playing.returnMode = req.returnMode;
    m_playing.returnCamType = resolvedReturnCamType;

    if (m_playing.returnCamType != CamType::None) m_playing.returnCamHandle = GetCamHandle(m_playing.returnCamType);
    else m_playing.returnCamHandle.Reset();

    if (m_playing.pendingStart) seqPlayer->Pause();
    else seqPlayer->Play();

    return handle;
}

_bool CCamDirector::StopRequest(_uint handle, _float blendOutSec, _bool resetTime)
{
    const Matrix outWorld = *CameraManager()->Get_InversedViewMatrix();

    Vector3 outPos = outWorld.Translation();
    Quaternion outRot = Quaternion::CreateFromRotationMatrix(outWorld);
    outRot.Normalize();

    auto seqObj = GetSeqObj();
    auto seqPlayer = seqObj->Get_Component<CCamSequencePlayer>();

    seqPlayer->SetApplyEnabled(false);
    seqPlayer->Stop(false);

    if (resetTime) seqPlayer->SetTime(0.f);

    if (m_playing.returnCamType != CamType::None && m_playing.returnMode != CamReturnMode::None)
    {
        auto returnObj = ObjectManager()->Request_Object(m_playing.returnCamHandle);

        if (m_playing.returnCamType == CamType::Orbit)
        {
            auto orbit = static_cast<COrbitCam*>(returnObj);

            if (m_playing.returnMode == CamReturnMode::SnapToEnd) orbit->SnapFromCamPose(outPos, outRot);
            else if (m_playing.returnMode == CamReturnMode::RestorePrev) orbit->RestoreSnapshot(m_playing.prevOrbit);
        }
    }

    const _bool ok = CameraManager()->Pop(handle, blendOutSec);
    m_playing = {};
    return ok;
}

void CCamDirector::StopAll(_float blendOutSec)
{
    if (!m_playing.active) return;
    StopRequest(m_playing.handle, blendOutSec, true);
}