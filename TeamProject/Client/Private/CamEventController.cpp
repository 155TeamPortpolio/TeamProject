#include "pch.h"
#include "CamEventController.h"

#include "CamSequencePlayer.h"

void CCamEventController::SetSequence(const CamSequenceDesc* seq)
{
	m_seq = seq;
	m_rules.clear();

	if (!m_seq) return;

    const auto& keys = m_seq->keyframes;

    for (size_t i = 0; i < keys.size(); ++i)
    {
        const string& tag = keys[i].eventTag;
        if (tag.empty()) continue;
        if (m_rules.count(tag)) continue;

        Rule rule{};

        if (i + 1 < keys.size())
        {
            rule.fireTime = keys[i + 1].time;
            rule.fireAtEnd = false;
        }
        else
        {
            rule.fireTime = 0.f;
            rule.fireAtEnd = true;
        }

        m_rules.emplace(tag, rule);
    }
}

void CCamEventController::SyncTime(_float time, _bool isPlaying)
{
    m_prevTime = time;
    m_prevPlaying = isPlaying;
}

void CCamEventController::Evaluate(_float time, _bool isPlaying)
{
    if (!m_seq) return;

    if (time < m_prevTime)
    {
        SyncTime(time, isPlaying);
        return;
    }

    for (auto& it : m_rules)
    {
        const string& tag = it.first;
        const Rule& rule = it.second;

        if (!rule.fireAtEnd)
        {
            if (m_prevTime < rule.fireTime&& time >= rule.fireTime)
                m_fired.insert(tag);
        }
        else
        {
            if (m_prevPlaying && !isPlaying)
                m_fired.insert(tag);
        }
    }

    SyncTime(time, isPlaying);
}