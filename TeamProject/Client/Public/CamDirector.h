#pragma once

#include "Base.h"
#include "CameraMgr.h"

NS_BEGIN(Engine)
class ICameraService;
NS_END

NS_BEGIN(Client)
class CSequenceCam;

enum class CamReturnType { None, OrbitCam ,FreeCam };

class CCamDirector final : public CBase
{
    DECLARE_SINGLETON(CCamDirector)
private:
    CCamDirector() {}
    virtual ~CCamDirector() = default;

public:
    void  Bind(CSequenceCam* sequenceCam);
    _bool Register(const string& key, const filesystem::path& path);
    void  UnRegister(const string& key);

public:
    void  SetSpaceReference(OBJECT_HANDLE handle) { m_spaceRefHandle = handle; }
    void  SetReturnCam(OBJECT_HANDLE handle, CamReturnType type) { m_returnCamHandle = handle; m_returnCamType = type; }

    _uint RequestSequence(const string& key, _float blendInSec = 0.25f, _bool resetTime = true, _float blendOutSec = 0.25f);
    _bool StopRequest(_uint handle, _float blendOutSec = 0.25f, _bool resetTime = true);
    void  StopAll(_float blendOutSec = 0.25f);
    void  Update(_float dt);

    virtual void  Free()   override { __super::Free(); }

private:
    struct SeqEntry
    {
        filesystem::path path{};
        CamSequenceDesc  seq{};
    };
    struct PlayingState
    {
        _uint  handle = 0u;
        string key{};
        _bool  active = false;

        _bool  pendingStart       = false;
        _float blendInRemain      = 0.f;
        _bool  resetTimeOnStart   = true;
        _float defaultBlendOutSec = 0.25f;
    };

private:
    CSequenceCam* GetSequenceCam() const;

private:
    unordered_map<string, SeqEntry> m_seqs{};
    PlayingState                    m_playing{};
    OBJECT_HANDLE                   m_seqHandle{};
    OBJECT_HANDLE                   m_spaceRefHandle{};
    OBJECT_HANDLE                   m_returnCamHandle{};
    CamReturnType                   m_returnCamType = CamReturnType::None;
};

NS_END