#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComponent abstract:
    public CBase
{
protected:
    CComponent();
    CComponent(const CComponent& rhs);

    virtual ~CComponent() override DEFAULT;
public:
    virtual HRESULT Initialize_Prototype() PURE;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg = nullptr)PURE;
    virtual void Render_GUI();

public:
    void Set_Owner(class CGameObject* owner) { m_pOwner = owner; }
    class CGameObject* Get_Owner() { return m_pOwner; }

public:
    virtual void Set_CompActive(_bool bActive) { m_bActive = bActive; }
    _bool Get_CompActive() { return m_bActive; }

protected:
    class CGameObject* m_pOwner = {nullptr};
    _bool m_bActive = { true };

public:
    virtual void Releas_Component() {};
    virtual CComponent* Clone() PURE;
    virtual void Free();
};

NS_END