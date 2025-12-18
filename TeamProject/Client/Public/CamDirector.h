#pragma once

#include "Base.h"
#include "CameraMgr.h"

NS_BEGIN(Engine)
class ICameraService;
NS_END

NS_BEGIN(Client)
class CSequenceCam;

class CCamDirector final : public CBase
{
private:
    CCamDirector() {}
    virtual ~CCamDirector() DEFAULT;

public:
    void  Bind(CSequenceCam* sequenceCam);
    _bool Register(const string& key, const filesystem::path& path);
    void  UnRegister(const string& key);

public:
    _uint RequestSequence(const string& key, _float blendInSec = 0.25f, _bool resetTime = true, _float blendOutSec = 0.25f);
    _bool StopRequest(_uint handle, _float blendOutSec = 0.25f, _bool resetTime = true);
    void  StopAll(_float blendOutSec = 0.25f);
    void  Update(_float dt);

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

        _bool  pendingStart = false;
        _float blendInRemain = 0.f;
        _bool  resetTimeOnStart = true;
        _float defaultBlendOutSec = 0.25f;
    };

private:
    CSequenceCam* RequireSequenceCam() const;
    void          ClearPlayingState();

private:
    unordered_map<string, SeqEntry> m_sequences{};
    PlayingState                    m_playing{};
    OBJECT_HANDLE                   m_sequenceHandle{};

public:
    static CCamDirector* Create() { return new CCamDirector(); }
    void Free() override;
};

NS_END