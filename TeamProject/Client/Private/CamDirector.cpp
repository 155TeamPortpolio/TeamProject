#include "pch.h"
#include "CamDirector.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Helper_Func.h"
#include "FieldSystem.h"
#include "UIDirector.h"
#include "DisplayGate.h"
#include "UI_Gangta.h"
// Camera
#include "SequenceCam.h"
#include "OrbitCam.h"
#include "FreeCam.h"
#include "CamDebugInput.h"
#include "CamObject.h"
// Component
#include "CharacterController.h"
#include "CamSequencePlayer.h"
// Player
#include "Player.h"
#include "BattlePlayer.h"
#include "BattleSystem.h"
#include "Animator3D.h"
#include "Jaeger.h"

namespace
{
    _bool StartsWith(const string& s, const char* prefix)
    {
        const size_t n = strlen(prefix);
        if (s.size() < n) return false;
        return memcmp(s.data(), prefix, n) == 0;
    }
    _bool IsParrySeqKey(const string& key)
    {
        return (StartsWith(key, "Parry/")) ? true : false;
    }
}

IMPLEMENT_SINGLETON(CCamDirector)

CGameObject* CCamDirector::GetCamObj(CamType type) const
{
    return ObjectManager()->Request_Object(m_camHandles[ENUM(type)]);
}

OBJECT_HANDLE CCamDirector::GetCurTarget() const
{
    return BattleSystem()->GetBattlePlayer()->GetTargetHandle();
}

OBJECT_HANDLE CCamDirector::GetCurParry() const
{
    return BattleSystem()->GetBattlePlayer()->GetParryHandle();
}

_bool CCamDirector::Register(const string& key, const fs::path& path)
{
    CamSeqReqDesc req{};
    return Register(key, path, req);
}

_bool CCamDirector::Register(const string& key, const fs::path& path, const CamSeqReqDesc& defaultReq)
{
    CamSeqEntry entry{};
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

void CCamDirector::EnterBoss()
{
    CameraManager()->SetFov(10.f, 2.f, EaseType::InOutCubic);
    CameraManager()->SetZFar(1500.f, 2.f, EaseType::InOutCubic);
}

void CCamDirector::ExitBoss()
{
    CameraManager()->SetFov(-10.f, 0.f, EaseType::InOutCubic);
    CameraManager()->SetZFar(500.f, 2.f, EaseType::InOutCubic);
}

void CCamDirector::AutoTarget()
{
    auto handle = GetCurHandle();

    const string& instanceName = handle.Get()->Get_InstanceName();

    if (handle.isValid())
        SetTarget(handle);
}

void CCamDirector::AutoField(CamStartDir dir)
{
    AutoTarget();
    switch (dir)
    {
    case CamStartDir::Front: RequestSequence("Field/Front"); break;
    case CamStartDir::Back:  RequestSequence("Field/Back");  break;
    }
}

void CCamDirector::AutoBattle(CamStartDir dir)
{
    AutoTarget();
    switch (dir)
    {
    case CamStartDir::Front: RequestSequence("Battle/Front"); break;
    case CamStartDir::Back:  RequestSequence("Battle/Back");  break;
    }
}

Vector3 CCamDirector::GetBipPos(OBJECT_HANDLE h, _float offsetY)
{
    auto obj = ObjectManager()->Request_Object(h);
    auto anim = obj->Get_Component<CAnimator3D>();
    _float4x4 m{};
    anim->Get_BipWorld(&m);
    return Vector3(m._41, m._42, m._43) + Vector3(0.f, offsetY, 0.f);
}

void CCamDirector::Update(_float dt)
{
    m_events.BeginFrame();

    UpdatePlayer();
    UpdateLevel();

    if (IsFreeCamActive() && m_playing.active)
        AbortSequence_NoCam(true);

    if (m_playing.active)
    {
        auto seqPlayer = GetSeqPlayer();

        if (seqPlayer->GetSequence()->space == CamSpace::Local && !m_spaceRefHandle.isValid())
        {
            AbortSequenceToOrbit(true);
            CamDebugInput::UpdateInput(dt);
            return;
        }

        if (m_playing.pendingStart)
        {
            seqPlayer->Update(0.f);

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

    if (IsValid())
        m_dialogue.Update(dt);

    m_parry.Update(dt);
    m_wipeOut.Update(dt);
    m_switch.Update(dt);
    m_portal.Update(dt);

    if (m_dialogueUnlockPending && !m_dialogue.IsBusy())
    {
        GetOrbitCam()->Unlock_Input();
        m_dialogueUnlockPending = false;
    }

    CamDebugInput::UpdateInput(dt);
}

void CCamDirector::StartBattleIntro(CamSeqType type)
{
    AutoTarget();
    RequestSequence(type);

    if (type == CamSeqType::ZeroIntro)
        BattleSystem()->GetBattlePlayer()->QuestStart();
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
}
 
void CCamDirector::StartDialog()
{
    GetOrbitCam()->Lock_Input();
    GetOrbitCam()->DialogueMode_Begin();

    m_dialogue.Begin(35.f, 0.5f);
    m_dialogueUnlockPending = false;
}

void CCamDirector::EndDialog()
{
    m_dialogue.End(0.5f);
    m_dialogueUnlockPending = true;
    GetOrbitCam()->Unlock_Input();
}

void CCamDirector::StartParry()
{
    if (m_playing.active)
    {
        if (IsParrySeqKey(m_playing.key) && BattleSystem()->GetBattlePlayer()->Is_ChainParry() && m_parry.IsChainReentryOpen())
            StopAll(0.f);
        else
            return;
    }

    m_parry.Begin();
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

_uint CCamDirector::RequestSequence(CamSeqType type)
{
    return RequestSequence(ResolveSeqKey(type));
}

_bool CCamDirector::IsPlaying(const string& key) const
{
    if (!m_playing.active)      return false;
    if (m_playing.key != key)   return false;
    if (m_playing.pendingStart) return true;

    return GetSeqPlayer()->IsPlaying();
}

_bool CCamDirector::IsPlaying(CamSeqType type) const
{
    return IsPlaying(ResolveSeqKey(type));
}

_bool CCamDirector::IsFinished(CamEventType type) const
{
    return m_events.IsFired(Helper::EnumToString(type));
}

_uint CCamDirector::RequestSequence(const string& key, const CamSeqReqDesc& req)
{
    if (IsFreeCamActive()) return 0u;

    auto it = m_seqs.find(key);
    if (it == m_seqs.end()) return 0u;

    auto& entry = it->second;

    if (entry.seqDesc.space == CamSpace::Local && !m_spaceRefHandle.isValid())
        return 0u;

    CamType resolvedReturnCamType = req.returnCamType;
    if (resolvedReturnCamType == CamType::None) resolvedReturnCamType = m_returnCamType;

    auto seqPlayer = GetSeqPlayer();
    auto seqCam = GetSeqCamComp();

    if (!m_playing.active)
    {
        if (req.returnMode == CamReturnMode::RestorePrev && resolvedReturnCamType == CamType::Orbit)
            GetOrbitCam()->CaptureSnapshot(m_playing.prevOrbit);

        seqPlayer->SetSequence(&entry.seqDesc);

        if (entry.seqDesc.space == CamSpace::Local) seqPlayer->SetSpaceRef(m_spaceRefHandle);
        else seqPlayer->ClearSpaceRef();

        seqPlayer->SetApplyEnabled(true);

        if (req.resetTime) seqPlayer->SetTime(0.f);

        CameraManager()->Set_BlendEase(req.blendInEase);
        const _uint handle = CameraManager()->Push(seqCam, req.blendInSec);

        m_playing.handle = handle;
        m_playing.key = key;
        m_playing.active = true;
        m_playing.defaultBlendOutSec = req.blendOutSec;
        m_playing.defaultBlendOutEase = req.blendOutEase;
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
    {
        seqPlayer->SetApplyEnabled(false);
        seqPlayer->Stop(false);

        seqPlayer->SetSequence(&entry.seqDesc);

        if (entry.seqDesc.space == CamSpace::Local) seqPlayer->SetSpaceRef(m_spaceRefHandle);
        else seqPlayer->ClearSpaceRef();

        seqPlayer->SetApplyEnabled(true);

        if (req.resetTime) seqPlayer->SetTime(0.f);

        m_playing.key = key;
        m_playing.defaultBlendOutSec = req.blendOutSec;
        m_playing.defaultBlendOutEase = req.blendOutEase;

        if (req.returnMode != CamReturnMode::None)
        {
            m_playing.returnMode = req.returnMode;
            m_playing.returnCamType = resolvedReturnCamType;

            if (m_playing.returnCamType != CamType::None) m_playing.returnCamHandle = GetCamHandle(m_playing.returnCamType);
            else m_playing.returnCamHandle.Reset();
        }

        seqPlayer->Play();

        m_events.SetSequence(&entry.seqDesc);
        m_events.SyncTime(seqPlayer->GetTime(), seqPlayer->IsPlaying());

        return m_playing.handle;
    }
}

void CCamDirector::AbortSequence_NoCam(_bool resetTime)
{
    m_lastEndedValid = m_playing.active;
    m_lastEndedKey = m_playing.key;

    auto seqPlayer = GetSeqPlayer();
    seqPlayer->SetApplyEnabled(false);
    seqPlayer->Stop(false);

    if (resetTime) seqPlayer->SetTime(0.f);

    m_playing = {};
}

_bool CCamDirector::StopRequest(_uint handle, _float blendOutSec, _bool resetTime)
{
    if (!m_playing.active) return false;
    if (handle != m_playing.handle) return false;

    if (IsFreeCamActive())
    {
        AbortSequence_NoCam(resetTime);
        return true;
    }

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

    CameraManager()->Set_BlendEase(m_playing.defaultBlendOutEase);
    const _bool ok = CameraManager()->Pop(handle, blendOutSec);

    m_playing = {};
    return ok;
}

void CCamDirector::StopAll(_float blendOutSec)
{
    if (!m_playing.active) return;
    StopRequest(m_playing.handle, blendOutSec, true);
}

void CCamDirector::UpdateLevel()
{
    const string curLevelKey = LevelManager()->Get_NowLevelKey();

    if (!m_levelInit)
    {
        m_levelInit = true;
        m_lastLevelKey = curLevelKey;
        return;
    }

    if (curLevelKey == m_lastLevelKey) return;

    m_lastLevelKey = curLevelKey;
}