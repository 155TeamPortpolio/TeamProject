#pragma once

#include "CameraMgr.h"
#include "CamDirectorData.h"

NS_BEGIN(Client)

class CCamDirector final : public CBase
{
    DECLARE_SINGLETON(CCamDirector)
private:
    CCamDirector();
    virtual ~CCamDirector() = default;

public:
    void          SetCam(CamType type, OBJECT_HANDLE handle) { m_camHandles[ENUM(type)] = handle; }
    void          SetSpaceRef(OBJECT_HANDLE handle)          { m_spaceRefHandle         = handle; }
    void          SetReturnCam(CamType type)                 { m_returnCamType          = type;   }
    void          SetTarget(OBJECT_HANDLE targetHandle);
    void          SetCurTarget();

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
    CPlayer*      GetPlayer()                const;

public:
    _bool         Register(const string& key, const filesystem::path& path);
    _bool         Register(const string& key, const filesystem::path& path, const CamSequenceRequestDesc& defaultReq);
    void          UnRegister(const string& key);
            
public:           
    _uint         RequestSequence(const string& key);
    _uint         RequestSequence(const string& key, const CamSequenceRequestDesc& req);
    _uint         RequestSequence(const string& key, _float blendInSec, _bool resetTime, _float blendOutSec);
                  
    _bool         StopRequest(_uint handle, _float blendOutSec = 0.25f, _bool resetTime = true);
    void          StopAll(_float blendOutSec = 0.25f);
    void          Update(_float dt);

private:
    void          UpdatePlayer();
    void          UpdateInput();

private:
    ICameraService&         camMgr;
    IObjectService&         objMgr;

    CamDirectorSeqMap       m_seqs{};
    CamDirectorPlayingState m_playing{};
    CamDirectorCamHandles   m_camHandles{};
    OBJECT_HANDLE           m_spaceRefHandle{};
    CamType                 m_returnCamType = CamType::None;

    OBJECT_HANDLE           m_focusHandle{};
    _int                    m_focusType = -1;
};

inline CCamDirector& CamDirector() { return *CCamDirector::GetInstance(); }

NS_END