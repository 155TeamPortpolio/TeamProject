#pragma once

#include "CameraMgr.h"
#include "CamDirectorData.h"
#include "CamEventController.h"
#include "CamDialogueController.h"

NS_BEGIN(Client)

class CCamDirector final : public CBase
{
    DECLARE_SINGLETON(CCamDirector)
private:
    CCamDirector() {}
    virtual ~CCamDirector() DEFAULT;

public:
    void          SetCam(CamType type, OBJECT_HANDLE handle) { m_camHandles[ENUM(type)] = handle; }
    void          SetSpaceRef(OBJECT_HANDLE handle)          { m_spaceRefHandle         = handle; }
    void          SetReturnCam(CamType type)                 { m_returnCamType          = type;   }
    void          SetTarget(OBJECT_HANDLE targetHandle);
    void          AutoTarget();
    void          AutoField();

    OBJECT_HANDLE GetCamHandle(CamType type) const { return m_camHandles[ENUM(type)];                }
    COrbitCam*    GetOrbitCam()              const { return static_cast<COrbitCam*>(GetOrbitObj());  }
    CSequenceCam* GetSeqCam()                const { return static_cast<CSequenceCam*>(GetSeqObj()); }
    CFreeCam*     GetFreeCam()               const { return static_cast<CFreeCam*>(GetFreeObj());    }

    CGameObject*  GetCamObj(CamType type)    const;
    CGameObject*  GetSeqObj()                const { return GetCamObj(CamType::Sequence); }
    CGameObject*  GetOrbitObj()              const { return GetCamObj(CamType::Orbit);    }
    CGameObject*  GetFreeObj()               const { return GetCamObj(CamType::Free);     }

    CCamera*      GetFreeCamComp()           const { return GetFreeCam()->Get_Component<CCamera>();  }
    CCamera*      GetSeqCamComp()            const { return GetSeqCam()->Get_Component<CCamera>();   }
    CCamera*      GetOrbitCamComp()          const { return GetOrbitCam()->Get_Component<CCamera>(); }
    CPlayer*      GetPlayer()                const { return static_cast<CPlayer*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player))); }
    CCharacter*   GetCharacter()             const { return static_cast<CCharacter*>(GetCurHandle().Get()); }
    CHARACTER     GetCharacterName()         const { return GetCharacter()->Get_CharacterName(); }
    string        GetCharacterStr()          const { return Helper::EnumToString(GetCharacter()->Get_CharacterName()); }
    OBJECT_HANDLE GetCurHandle()             const { return GetPlayer()->Get_CurCharacterHandle(); }
    
    CCamSequencePlayer* GetSeqPlayer()       const { return GetSeqObj()->Get_Component<CCamSequencePlayer>(); }

public:
    _bool         Register(const string& key, const fs::path& path);
    _bool         Register(const string& key, const fs::path& path, const CamSequenceRequestDesc& defaultReq);
    void          UnRegister(const string& key);         
    _uint         RequestSequence(const string& key);
    _uint         RequestSequence(CamSeqType type);
    
    _bool         IsPlaying(const string& key)       const;
    _bool         IsPlaying(CamSeqType type)         const;
    _bool         IsFinished(CamEventType eventType) const;

    _bool         StopRequest(_uint handle, _float blendOutSec = 0.25f, _bool resetTime = true);
    void          StopAll(_float blendOutSec = 0.25f);
    void          Update(_float dt);
    void          StartBattleIntro(CamSeqType type);
    void          StartDialog();
    void          EndDialog();

private:
    string        ResolveSeqKey(CamSeqType type) const;
    void          UpdatePlayer();
    void          UpdateInput(_float dt);
    void          AbortSequenceToOrbit(_bool resetTime);
    void          SyncSeqInputLock();
      
    _bool         IsValid() const { return GetPlayer()->Get_CurCharacterHandle().isValid(); }
    _uint         RequestSequence(const string& key, const CamSequenceRequestDesc& req);
    _uint         RequestSequence(const string& key, _float blendInSec, _bool resetTime, _float blendOutSec);
    _uint         RequestSequence(CamSeqType type, const CamSequenceRequestDesc& req);

private:
    CamDirectorSeqMap       m_seqs{};
    CamDirectorPlayingState m_playing{};
    CamDirectorCamHandles   m_camHandles{};
    CCamEventController     m_events{};
    CCamDialogueController  m_dialogue{};

    OBJECT_HANDLE           m_spaceRefHandle{};
    CamType                 m_returnCamType = CamType::None;

    OBJECT_HANDLE           m_focusHandle{};
    _int                    m_focusType = -1;

    _bool                   m_lastEndedValid = false;
    string                  m_lastEndedKey{};
    _bool                   m_seqInputLocked = false;
}; 

inline auto* CamDirector() { return CCamDirector::GetInstance(); }

NS_END