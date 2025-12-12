#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CEffectNode abstract :
    public CGameObject
{
protected:
    CEffectNode();
    CEffectNode(const CEffectNode& rhs);
    virtual ~CEffectNode() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void Awake() override {}
    virtual void Priority_Update(_float dt) override {}
    virtual void Update(_float dt) override;
    virtual void Late_Update(_float dt) override {}

public:
    void Render_GUI() override;

public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free();

protected:
    _bool m_IsLoop = false;
    _float m_fDuration{};
    _float m_fElpasedTime{};

};
NS_END