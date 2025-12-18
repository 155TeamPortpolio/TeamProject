#include "pch.h"
#include "CamDirector.h"
#include "CamSequencePlayer.h"
#include "SequenceCam.h"
#include "GameInstance.h"

void CCamDirector::Bind(CSequenceCam* sequenceCam)
{
    m_sequenceCam = sequenceCam;
}

_bool CCamDirector::Register(const string& key, const filesystem::path& path)
{
    assert(!key.empty());

    SeqEntry entry{};
    entry.path = path;

    string err;
    if (!CamUtil::Load(entry.path, entry.seq, &err))
        return false;

    entry.loaded = true;
    m_sequences[key] = move(entry);
    return true;
}

void CCamDirector::UnRegister(const string& key)
{
    auto it = m_sequences.find(key);
    if (it == m_sequences.end())
        return;

    if (m_playing.active && m_playing.key == key)
        StopAll(0.25f);

    m_sequences.erase(it);
}

void CCamDirector::Update(_float dt)
{
    if (!m_playing.active) return;

    auto sequencePlayer = m_sequenceCam->Get_Component<CCamSequencePlayer>();

    if (m_playing.pendingStart)
    {
        m_playing.blendInRemain -= dt;

        if (m_playing.blendInRemain <= 0.f)
        {
            if (m_playing.resetTimeOnStart)
                sequencePlayer->SetTime(0.f);

            sequencePlayer->Play();
            m_playing.pendingStart = false;
        }
        else
        {
            if (m_playing.resetTimeOnStart)
                sequencePlayer->SetTime(0.f);
        }

        return;
    }

    sequencePlayer->Update(dt);

    if (!sequencePlayer->IsPlaying())
        StopAll(2.f);
}

_uint CCamDirector::RequestSequence(const string& key, _float blendSec, _bool resetTime)
{
    if (!EnsureLoaded(key))  return 0u;

    if (m_playing.active)
        StopAll(blendSec);

    auto sequencePlayer = m_sequenceCam->Get_Component<CCamSequencePlayer>();
    auto it = m_sequences.find(key);
    SeqEntry& entry = it->second;

    sequencePlayer->SetSequence(&entry.seq);
    sequencePlayer->SetApplyEnabled(true);

    if (resetTime)
        sequencePlayer->SetTime(0.f);

    auto camComp = m_sequenceCam->Get_Component<CCamera>();
    const _uint handle = CAM->Push(camComp, blendSec);
    if (handle == 0u)
    {
        sequencePlayer->Stop(true);
        sequencePlayer->SetApplyEnabled(false);
        return 0u;
    }

    m_playing.handle = handle;
    m_playing.key = key;
    m_playing.active = true;

    if (blendSec > 1e-6f)
    {
        m_playing.pendingStart = true;
        m_playing.blendInRemain = blendSec;
        m_playing.resetTimeOnStart = resetTime;
        sequencePlayer->Pause();
        if (resetTime)
            sequencePlayer->SetTime(0.f);
    }
    else
    {
        if (resetTime)
            sequencePlayer->SetTime(0.f);
        sequencePlayer->Play();
        m_playing.pendingStart = false;
        m_playing.blendInRemain = 0.f;
        m_playing.resetTimeOnStart = resetTime;
    }

    return handle;
}

_bool CCamDirector::StopRequest(_uint handle, _float blendOutSec, _bool resetTime)
{
    if (!m_playing.active) return false;
    if (m_playing.handle != handle) return false;

    auto sequencePlayer = m_sequenceCam->Get_Component<CCamSequencePlayer>();
    sequencePlayer->Stop(resetTime);

    const _bool ok = CAM->Pop(handle, blendOutSec);

    m_playing.handle = 0u;
    m_playing.key.clear();
    m_playing.active = false;
    m_playing.pendingStart = false;
    m_playing.blendInRemain = 0.f;
    m_playing.resetTimeOnStart = true;

    return ok;
}

void CCamDirector::StopAll(_float blendOutSec)
{
    if (!m_playing.active) return;
    StopRequest(m_playing.handle, blendOutSec, true);
}

bool CCamDirector::EnsureLoaded(const string& key)
{
    auto it = m_sequences.find(key);
    if (it == m_sequences.end())
        return false;

    SeqEntry& entry = it->second;

    if (entry.loaded)
        return true;

    string err;
    if (!CamUtil::Load(entry.path, entry.seq, &err))
    {
        entry.loaded = false;
        return false;
    }

    entry.loaded = true;
    return true;
}

CCamDirector* CCamDirector::Create()
{
    auto inst = new CCamDirector();
    return inst;
}

void CCamDirector::Free()
{
    StopAll(0.f);
    Safe_Release(m_sequenceCam);
    __super::Free();
}