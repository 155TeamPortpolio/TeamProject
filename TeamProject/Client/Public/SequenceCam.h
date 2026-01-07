#pragma once

#include "CamObject.h"
#include "CamSequencePlayer.h"

NS_BEGIN(Client)

class CSequenceCam final : public CCamObject
{
private:
    CSequenceCam() = default;
    CSequenceCam(const CSequenceCam& rhs) : CCamObject(rhs) {}
    virtual ~CSequenceCam() = default;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt)  override {}
    virtual void    Update(_float dt)           override {}
    virtual void    Late_Update(_float dt)      override {}
    virtual void    Render_GUI()                override { __super::Render_GUI(); }

public:
    void    Stop(_bool resetTime = true) { m_seqPlayer->Stop(resetTime); }
    _bool   IsPlaying() const            { return m_seqPlayer->IsPlaying(); }

private:
    CCamSequencePlayer* m_seqPlayer{};
    CamSequenceDesc     m_seqDesc{};

public:
    static  CSequenceCam* Create();
    virtual CGameObject*  Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};

NS_END