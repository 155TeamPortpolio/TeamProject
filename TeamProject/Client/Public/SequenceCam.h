#pragma once

#include "CamObject.h"
#include "CamSequencePlayer.h"

NS_BEGIN(Client)
// Cinematic
class CSequenceCam final : public CCamObject
{
private:
    CSequenceCam() DEFAULT;
    CSequenceCam(const CSequenceCam& rhs) : CCamObject(rhs) {}
    virtual ~CSequenceCam() DEFAULT;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;

    void    Priority_Update(_float dt)  override {}
    void    Update(_float dt)           override {}
    void    Late_Update(_float dt)      override {}
    void    Render_GUI()                override;

public:
    void    Stop(_bool resetTime = true);
            
    _bool   IsPlaying() const { return m_sequencePlayer && m_sequencePlayer->IsPlaying(); }

private:
    CCamSequencePlayer* m_sequencePlayer{};
    CamSequenceDesc     m_seqDesc{};
    filesystem::path    m_LastPath{};

public:
    static CSequenceCam* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};

NS_END