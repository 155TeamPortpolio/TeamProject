#pragma once

NS_BEGIN(Engine)
struct CamSequenceDesc;
NS_END

NS_BEGIN(Client)

class CCamEventController
{
public:
	void  BeginFrame() { m_fired.clear(); }

	void  SetSequence(const CamSequenceDesc* seq);
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
	const CamSequenceDesc*      m_seq{};
	unordered_map<string, Rule> m_rules{};
	unordered_set<string>       m_fired{};

	_float m_prevTime    = 0.f;
	_bool  m_prevPlaying = false;
};

NS_END