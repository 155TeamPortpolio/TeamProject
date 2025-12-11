#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CEffectNode abstract :
    public CGameObject
{
protected:
    CEffectNode();
    CEffectNode(const CEffectNode& rhs);
    virtual ~CEffectNode() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override { return S_OK; }
    virtual HRESULT Initialize(INIT_DESC* pArg) override { return S_OK; }
    virtual void Awake() override {}
    virtual void Priority_Update(_float dt) override {}
    virtual void Update(_float dt) override {}
    virtual void Late_Update(_float dt) override {}

public:
    void Render_GUI() override;

public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free();

protected:
    _float m_fDuration{};

};
NS_END