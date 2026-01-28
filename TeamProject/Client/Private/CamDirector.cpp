#include "pch.h"
#include "CamDirector.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Helper_Func.h"
#include "UIDirector.h"
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
#include "Character.h"

namespace
{
    size_t FindEventKeyIdx(const vector<CamKeyFrame>& keys, const string& eventTag)
    {
        for (size_t i = 0; i < keys.size(); ++i)
            if (keys[i].eventTag == eventTag) return i;

        return (size_t)-1;
    }
}

IMPLEMENT_SINGLETON(CCamDirector)

CGameObject* CCamDirector::GetCamObj(CamType type) const
{
    return ObjectManager()->Request_Object(m_camHandles[ENUM(type)]);
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
    auto handle = GetCurHandle();
    if (handle.isValid())
        SetTarget(handle);
}

void CCamDirector::AutoField()
{
    AutoTarget();
    RequestSequence("Field/Front");
}

void CCamDirector::Update(_float dt)
{
    m_events.BeginFrame();

    UpdatePlayer();

    if (m_playing.active)
    {
        auto seqPlayer = GetSeqPlayer();

        if (seqPlayer->GetSequence()->space == CamSpace::Local && !m_spaceRefHandle.isValid())
        {
            AbortSequenceToOrbit(true);
            SyncSeqInputLock();
            UpdateInput(dt);
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

            m_events.SyncTime(seqPlayer->GetTime(), seqPlayer->IsPlaying());
        }
        else
        {
            seqPlayer->Update(dt);

            m_events.Evaluate(seqPlayer->GetTime(), seqPlayer->IsPlaying());

            if (!seqPlayer->IsPlaying())
                StopAll(m_playing.defaultBlendOutSec);
        }
    }

    SyncSeqInputLock();

    if (IsValid())
        m_dialogue.Update(dt, GetOrbitCamComp(), GetOrbitCam(), GetCharacter()->Get_Component<CTransform>());

    if (!m_playing.active)
        UpdateInput(dt);
}

void CCamDirector::StartBattleIntro(CamSeqType type)
{
    AutoTarget();
    RequestSequence(type);
    //UIDirector()->Hide_HUD(CUIDirector::HUD::BATTLE);
    //UIDirector()->Show_SceneFrame();
}

string CCamDirector::ResolveSeqKey(CamSeqType type) const
{
    const string typeToken = Helper::EnumToString(type);
    const string charToken = GetCharacterStr();

    string key;
    key += typeToken;
    key += "/";
    key += charToken;
    return key;
}

void CCamDirector::UpdateInput(_float dt)
{
    if (InputDevice()->Key_Tap(VK_F1))
        CameraManager()->Set_MainCam(GetFreeCamComp(), 0.5f);

    if (InputDevice()->Key_Tap(VK_F2))
        CameraManager()->Set_MainCam(GetOrbitCamComp(), 0.5f);
     
    const _int damage = Helper::Get_Random_Int(1000, 10000);

    if (InputDevice()->Key_Down(VK_F3))
              RequestSequence("Field/Front");
        //GetCharacter()->Take_Damage(DAMAGE_TYPE::NORMAL, damage);
}

void CCamDirector::AbortSequenceToOrbit(_bool resetTime)
{
    m_lastEndedValid = false;
    m_lastEndedKey.clear();

    auto seqPlayer = GetSeqPlayer();

    seqPlayer->SetApplyEnabled(false);
    seqPlayer->Stop(false);

    if (resetTime) seqPlayer->SetTime(0.f);

    CameraManager()->Clear(0.f);
    CameraManager()->Set_MainCam(GetOrbitCamComp(), 0.f);

    m_playing = {};
    SyncSeqInputLock();
}

void CCamDirector::SyncSeqInputLock()
{
    const _bool wantLock = m_playing.active;

    if (wantLock && !m_seqInputLocked)
    {
        GetPlayer()->Lock_Input();
        m_seqInputLocked = true;
        return;
    }

    if (!wantLock && m_seqInputLocked)
    {
        GetPlayer()->Unlock_Input();
        m_seqInputLocked = false;
    }
}

void CCamDirector::StartDialog()
{
    m_dialogue.Begin(35.f, 0.5f);
}

void CCamDirector::EndDialog()
{
    m_dialogue.End(0.5f);
}

void CCamDirector::UpdatePlayer()
{
    const _int type = ENUM(GetPlayer()->Get_PlayerType());
    OBJECT_HANDLE focus = GetCurHandle();

    if (type == ENUM(CPlayer::PLAYER::END) || !focus.isValid())
    {
        if (m_focusHandle.isValid())
            GetOrbitCam()->ClearTarget();

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

_uint CCamDirector::RequestSequence(CamSeqType type)
{
    return RequestSequence(ResolveSeqKey(type));
}

_uint CCamDirector::RequestSequence(CamSeqType type, const CamSequenceRequestDesc& req)
{
    return RequestSequence(ResolveSeqKey(type), req);
}

_bool CCamDirector::IsPlaying(const string& key) const
{
    if (!m_playing.active)      return false;
    if (m_playing.key != key)   return false;
    if (m_playing.pendingStart) return true;

    return GetSeqObj()->Get_Component<CCamSequencePlayer>()->IsPlaying();
}

_bool CCamDirector::IsPlaying(CamSeqType type) const
{
    return IsPlaying(ResolveSeqKey(type));
}

_bool CCamDirector::IsFinished(CamEventType type) const
{
    return m_events.IsFired(Helper::EnumToString(type));
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
    auto seqCam = GetSeqCamComp();

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

    m_events.SetSequence(&entry.seqDesc);
    m_events.SyncTime(seqPlayer->GetTime(), seqPlayer->IsPlaying());

    return handle;
}

_bool CCamDirector::StopRequest(_uint handle, _float blendOutSec, _bool resetTime)
{
    m_lastEndedValid = m_playing.active;
    m_lastEndedKey = m_playing.key;

    const Matrix outWorld = *CameraManager()->Get_InversedViewMatrix();

    Vector3 outPos = outWorld.Translation();
    Quaternion outRot = Quaternion::CreateFromRotationMatrix(outWorld);
    outRot.Normalize();

    auto seqPlayer = GetSeqPlayer();

    seqPlayer->SetApplyEnabled(false);
    seqPlayer->Stop(false);

    if (m_playing.returnCamType != CamType::None && m_playing.returnMode != CamReturnMode::None)
    {
        auto returnObj = ObjectManager()->Request_Object(m_playing.returnCamHandle);

        if (m_playing.returnCamType == CamType::Orbit)
        {
            auto orbit = static_cast<COrbitCam*>(returnObj);

            if (m_playing.returnMode == CamReturnMode::SnapToEnd)
            {
                orbit->SnapFromCamPose(outPos, outRot);
                orbit->FreezeFor(blendOutSec);
            }
            else if (m_playing.returnMode == CamReturnMode::RestorePrev)
            {
                orbit->RestoreSnapshot(m_playing.prevOrbit);
                orbit->FreezeFor(blendOutSec);
            }
        }
    }

    const _bool ok = CameraManager()->Pop(handle, blendOutSec);

    m_playing = {};
    SyncSeqInputLock();

    return ok;
}

void CCamDirector::StopAll(_float blendOutSec)
{
    if (!m_playing.active) return;
    StopRequest(m_playing.handle, blendOutSec, true);
}