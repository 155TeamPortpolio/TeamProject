#include "pch.h"
#include "CamDirector.h"
#include "GameInstance.h"
#include "GameObject.h"
// Camera
#include "SequenceCam.h"
#include "OrbitCam.h"
#include "FreeCam.h"
// Component
#include "CharacterController.h"
#include "CamSequencePlayer.h"
// Player
#include "Player.h"
#include "BattlePlayer.h"
#include "BattleSystem.h"

IMPLEMENT_SINGLETON(CCamDirector)

CCamDirector::CCamDirector() : camMgr(*CameraManager()), objMgr(*ObjectManager()) {}

CGameObject* CCamDirector::GetCamObj(CamType type) const
{
    return objMgr.Request_Object(m_camHandles[ENUM(type)]);
}

_bool CCamDirector::Register(const string& key, const fs::path& path)
{
    CamSequenceRequestDesc req{};
    return Register(key, path, req);
}

_bool CCamDirector::Register(const string& key, const fs::path& path, const CamSequenceRequestDesc& defaultReq)
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

void CCamDirector::SetTarget(OBJECT_HANDLE targetHandle)
{
    if (!targetHandle.isValid()) return;
    GetOrbitCam()->SetTarget(targetHandle);
    SetSpaceRef(targetHandle);
}

void CCamDirector::AutoTarget()
{
    auto handle = GetPlayer()->Get_CurCharacterHandle();
    if (handle.isValid())
        SetTarget(handle);
}

void CCamDirector::Update(_float dt)
{
    UpdateInput();
    UpdatePlayer();

    if (!m_playing.active) return;

    auto seqPlayer = GetSeqPlayer();

    if (seqPlayer->GetSequence()->space == CamSpace::Local && !m_spaceRefHandle.isValid())
    {
        AbortSequenceToOrbit(true);
        return;
    }

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

    if (!seqPlayer->IsPlaying())
        StopAll(m_playing.defaultBlendOutSec);
}

CPlayer* CCamDirector::GetPlayer() const
{
    return static_cast<CPlayer*>(objMgr.Find_Global(ENUM(GLOBAL_ID::Player)));
}

void CCamDirector::UpdateInput()
{
    if (InputDevice()->Key_Tap(VK_F1))
        camMgr.Set_MainCam(GetFreeCamComp(), 0.5f);

    if (InputDevice()->Key_Tap(VK_F2))
        camMgr.Set_MainCam(GetOrbitCamComp(), 0.5f);

    if (InputDevice()->Key_Tap(VK_F3))
        RequestSequence("Intro/Jane_Intro");
}

void CCamDirector::AbortSequenceToOrbit(_bool resetTime)
{
    auto seqPlayer = GetSeqPlayer();

    seqPlayer->SetApplyEnabled(false);
    seqPlayer->Stop(false);

    if (resetTime) seqPlayer->SetTime(0.f);

    camMgr.Clear(0.f);
    camMgr.Set_MainCam(GetOrbitCamComp(), 0.f);

    m_playing = {};
}

void CCamDirector::UpdatePlayer()
{
    auto player = GetPlayer();

    const _int type = ENUM(player->Get_PlayerType());
    OBJECT_HANDLE focus = player->Get_CurCharacterHandle();

    if (type == ENUM(CPlayer::PLAYER::END) || !focus.isValid())
    {
        if (m_focusHandle.isValid()) GetOrbitCam()->ClearTarget();

        m_focusHandle.Reset();
        m_focusType = type;
        return;
    }

    if (type != m_focusType) m_focusType = type;

    if (!m_focusHandle.isValid() || focus != m_focusHandle)
    {
        m_focusHandle = focus;
        SetTarget(m_focusHandle);
        SetSpaceRef(m_focusHandle);
    }
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

_bool CCamDirector::IsPlaying(const string& key) const
{
    if (!m_playing.active)      return false;
    if (m_playing.key != key)   return false;
    if (m_playing.pendingStart) return true;

    return GetSeqObj()->Get_Component<CCamSequencePlayer>()->IsPlaying();
}

_uint CCamDirector::RequestSequence(const string& key, const CamSequenceRequestDesc& req)
{
    if (m_playing.active) StopAll(req.blendOutSec);

    auto& entry = m_seqs.at(key);

    if (entry.seqDesc.space == CamSpace::Local && !m_spaceRefHandle.isValid())
        return 0u;

    CamType resolvedReturnCamType = req.returnCamType;
    if (resolvedReturnCamType == CamType::None) resolvedReturnCamType = m_returnCamType;

    if (req.returnMode == CamReturnMode::RestorePrev && resolvedReturnCamType == CamType::Orbit)
        GetOrbitCam()->CaptureSnapshot(m_playing.prevOrbit);

    auto seqPlayer = GetSeqPlayer();
    auto seqCam    = GetSeqCamComp();

    seqPlayer->SetSequence(&entry.seqDesc);

    if (entry.seqDesc.space == CamSpace::Local) seqPlayer->SetSpaceReference(m_spaceRefHandle);
    else seqPlayer->ClearSpaceReference();

    seqPlayer->SetApplyEnabled(true);

    if (req.resetTime) seqPlayer->SetTime(0.f);

    const _uint handle = camMgr.Push(seqCam, req.blendInSec);

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
    const Matrix outWorld = *camMgr.Get_InversedViewMatrix();

    Vector3 outPos = outWorld.Translation();
    Quaternion outRot = Quaternion::CreateFromRotationMatrix(outWorld);
    outRot.Normalize();

    auto seqPlayer = GetSeqPlayer();

    seqPlayer->SetApplyEnabled(false);
    seqPlayer->Stop(false);

    if (resetTime) seqPlayer->SetTime(0.f);

    if (m_playing.returnCamType != CamType::None && m_playing.returnMode != CamReturnMode::None)
    {
        auto returnObj = ObjectManager()->Request_Object(m_playing.returnCamHandle);

        if (m_playing.returnCamType == CamType::Orbit)
        {
            auto orbit = static_cast<COrbitCam*>(returnObj);

            if (m_playing.returnMode == CamReturnMode::SnapToEnd)
                orbit->SnapFromCamPose(outPos, outRot);
            else if (m_playing.returnMode == CamReturnMode::RestorePrev)
                orbit->RestoreSnapshot(m_playing.prevOrbit);
        }
    }

    const _bool ok = camMgr.Pop(handle, blendOutSec);
    m_playing = {};
    return ok;
}

void CCamDirector::StopAll(_float blendOutSec)
{
    if (!m_playing.active) return;
    StopRequest(m_playing.handle, blendOutSec, true);
}