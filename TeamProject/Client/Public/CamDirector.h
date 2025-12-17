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
    void   Bind(CSequenceCam* sequenceCam);

public:
	void   Register(const string& key, const filesystem::path& path);
	void   UnRegister(const string& key);

public:
	_uint  RequestSequence(const string& key, _float blendSec = 0.25f, _bool resetTime = true);
	_bool  StopRequest(_uint handle, _float blendOutSec = 0.25f, _bool resetTime = true);
	void   StopAll(_float blendOutSec = 0.25f);
	void   Update(_float dt);

private:
	bool   EnsureLoaded(const string& key);

private:
    struct SeqEntry
    {
        filesystem::path path{};
        CamSequenceDesc  seq{};
        _bool            loaded = false;
    };
    struct PlayingState
    {
        _uint  handle = 0u;
        string key{};
        _bool  active = false;
    };

private:
    unordered_map<string, SeqEntry> m_sequences{};
    PlayingState                    m_playing{};
    CSequenceCam*                   m_sequenceCam{};

public:
    static CCamDirector* Create();
    void Free() override;
};

NS_END