#pragma once

NS_BEGIN(Engine)
struct CamSeqDesc;
NS_END

NS_BEGIN(Client)

class CCamEventController
{
public:
	void  BeginFrame() { m_fired.clear(); }

	void  SetSequence(const CamSeqDesc* seq);
	void  SyncTime(_float time, _bool isPlaying);
	void  Evaluate(_float time, _bool isPlaying);

	_bool IsFired(const string& eventTag) const { return m_fired.count(eventTag) > 0; }

private:
	struct Rule
	{
		_float fireTime  = 0.f;
		_bool  fireAtEnd = false;
	};

private:
	const CamSeqDesc*      m_seq{};
	unordered_map<string, Rule> m_rules{};
	unordered_set<string>       m_fired{};

	_float m_prevTime    = 0.f;
	_bool  m_prevPlaying = false;
};

NS_END