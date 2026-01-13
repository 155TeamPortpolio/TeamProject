#pragma once

#include "CameraMgr.h"

NS_BEGIN(Engine)
class CCamSequencePlayer;
NS_END

NS_BEGIN(Client)
class CSequenceCam;

enum class CamType { None, Free, Orbit, Sequence, End };

class CCamDirector final : public CBase
{
    DECLARE_SINGLETON(CCamDirector)
private:
    CCamDirector() {}
    virtual ~CCamDirector() = default;

public:
    void SetCam(CamType type, OBJECT_HANDLE handle) { m_camHandles[ENUM(type)] = handle; }
    void ClearCam(CamType type)                     { m_camHandles[ENUM(type)].Reset(); }

public:
    void SetSpaceRef(OBJECT_HANDLE handle) { m_spaceRefHandle       = handle; }
    void SetReturnCam(CamType type)        { m_defaultReturnCamType = type; }
    void ClearReturnCam()                  { m_defaultReturnCamType = CamType::None; }

    OBJECT_HANDLE GetCamHandle(CamType type) const { return m_camHandles[ENUM(type)]; }

public:
    _bool Register(const string& key, const filesystem::path& path);
    void  UnRegister(const string& key);

public:
    _uint RequestSequence(const string& key, _float blendInSec = 0.25f, _bool resetTime = true, _float blendOutSec = 0.25f);
    _bool StopRequest(_uint handle, _float blendOutSec = 0.25f, _bool resetTime = true);
    void  StopAll(_float blendOutSec = 0.25f);
    void  Update(_float dt);

private:
    CGameObject*        GetCamObj(CamType type)  const;
    CGameObject*        GetSeqObj()              const;

private:
    struct SeqEntry
    {
        filesystem::path path{};
        CamSequenceDesc  seqDesc{};
    };
    struct PlayingState
    {
        _uint         handle = 0u;
        string        key{};
        _bool         active = false;
                      
        _bool         pendingStart  = false;
        _float        blendInRemain = 0.f;
                      
        _float        defaultBlendOutSec = 0.25f;

        CamType       returnCamType = CamType::None;
        OBJECT_HANDLE returnCamHandle{};
    };

private:
    unordered_map<string, SeqEntry>          m_seqs{};
    PlayingState                             m_playing{};
    array<OBJECT_HANDLE, ENUM(CamType::End)> m_camHandles{};

    OBJECT_HANDLE m_spaceRefHandle{};
    CamType       m_defaultReturnCamType = CamType::None;
};

NS_END