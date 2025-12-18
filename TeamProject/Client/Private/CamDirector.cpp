#include "pch.h"
#include "CamDirector.h"
#include "CamSequencePlayer.h"
#include "SequenceCam.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCamDirector)

CSequenceCam* CCamDirector::RequireSequenceCam() const
{
    return static_cast<CSequenceCam*>(OBJ->Request_Object(m_sequenceHandle));
}

void CCamDirector::Bind(CSequenceCam* sequenceCam)
{
    m_sequenceHandle = sequenceCam->Get_Handle();
}

_bool CCamDirector::Register(const string& key, const filesystem::path& path)
{
    SeqEntry entry{};
    entry.path = path;
    CamUtil::Load(entry.path, entry.seq);
    m_sequences[key] = move(entry);
    return true;
}

void CCamDirector::UnRegister(const string& key)
{
    if (m_playing.active && m_playing.key == key)
        StopAll(m_playing.defaultBlendOutSec);

    m_sequences.erase(key);
}

void CCamDirector::Update(_float dt)
{
    if (!m_playing.active) return;

    auto sequenceCam = RequireSequenceCam();
    auto sequencePlayer = sequenceCam->Get_Component<CCamSequencePlayer>();

    if (m_playing.pendingStart)
    {
        m_playing.blendInRemain -= dt;

        if (m_playing.resetTimeOnStart)
            sequencePlayer->SetTime(0.f);

        if (m_playing.blendInRemain <= 0.f)
        {
            sequencePlayer->Play();
            m_playing.pendingStart = false;
        }
        return;
    }

    sequencePlayer->Update(dt);

    if (!sequencePlayer->IsPlaying())
        StopAll(m_playing.defaultBlendOutSec);
}

_uint CCamDirector::RequestSequence(const string& key, _float blendInSec, _bool resetTime, _float blendOutSec)
{
    if (m_playing.active)
        StopAll(blendOutSec);

    auto& entry = m_sequences.at(key);

    auto sequenceCam = RequireSequenceCam();
    auto sequencePlayer = sequenceCam->Get_Component<CCamSequencePlayer>();

    sequencePlayer->SetSequence(&entry.seq);
    sequencePlayer->SetApplyEnabled(true);

    if (resetTime)
        sequencePlayer->SetTime(0.f);

    auto camComp = sequenceCam->Get_Component<CCamera>();
    const _uint handle = CAM->Push(camComp, blendInSec);

    m_playing.handle = handle;
    m_playing.key = key;
    m_playing.active = true;
    m_playing.defaultBlendOutSec = blendOutSec;

    m_playing.pendingStart = (blendInSec > 0.f);
    m_playing.blendInRemain = blendInSec;
    m_playing.resetTimeOnStart = resetTime;

    if (m_playing.pendingStart)
        sequencePlayer->Pause();
    else
        sequencePlayer->Play();

    return handle;
}

_bool CCamDirector::StopRequest(_uint handle, _float blendOutSec, _bool resetTime)
{
    if (m_playing.handle != handle)
        return false;

    auto sequenceCam = RequireSequenceCam();
    auto sequencePlayer = sequenceCam->Get_Component<CCamSequencePlayer>();

    sequencePlayer->Stop(resetTime);
    sequencePlayer->SetApplyEnabled(false);

    const _bool ok = CAM->Pop(handle, blendOutSec);

    ClearPlayingState();
    return ok;
}

void CCamDirector::StopAll(_float blendOutSec)
{
    StopRequest(m_playing.handle, blendOutSec, true);
}

void CCamDirector::Free()
{
    __super::Free();
}