#pragma once

#include "CameraMgr.h"
#include "CamDirectorData.h"

NS_BEGIN(Client)

class CCamDirector final : public CBase
{
    DECLARE_SINGLETON(CCamDirector)
private:
    CCamDirector() {}
    virtual ~CCamDirector() = default;

public:
    void SetCam(CamType type, OBJECT_HANDLE handle);
    void SetSpaceRef(OBJECT_HANDLE handle);
    void SetReturnCam(CamType type);
    void ClearReturnCam();
    void ClearCam(CamType type);

    OBJECT_HANDLE GetCamHandle(CamType type) const;
    COrbitCam*    GetOrbitCam() const;
    CSequenceCam* GetSeqCam()   const;

public:
    _bool Register(const string& key, const filesystem::path& path);
    _bool Register(const string& key, const filesystem::path& path, const CamSequenceRequestDesc& defaultReq);
    void  UnRegister(const string& key);

public:
    _uint RequestSequence(const string& key);
    _uint RequestSequence(const string& key, const CamSequenceRequestDesc& req);
    _uint RequestSequence(const string& key, _float blendInSec, _bool resetTime, _float blendOutSec);

    _bool StopRequest(_uint handle, _float blendOutSec = 0.25f, _bool resetTime = true);
    void  StopAll(_float blendOutSec = 0.25f);
    void  Update(_float dt);

private:
    CGameObject* GetCamObj(CamType type)  const;
    CGameObject* GetSeqObj()              const;
    CGameObject* GetOrbitObj()            const;

private:
    CamDirectorSeqMap       m_seqs{};
    CamDirectorPlayingState m_playing{};
    CamDirectorCamHandles   m_camHandles{};
    OBJECT_HANDLE           m_spaceRefHandle{};
    CamType                 m_returnCamType = CamType::None;
};

NS_END