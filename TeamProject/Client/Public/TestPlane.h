#pragma once

#include "GameObject.h"

NS_BEGIN(Client)

class CTestPlane final : public CGameObject
{
private:
    CTestPlane() {}
    CTestPlane(const CTestPlane& rhs) : CGameObject(rhs) {}
    virtual ~CTestPlane() DEFAULT;

public:
    HRESULT      Initialize_Prototype()      override;
    HRESULT      Initialize(INIT_DESC* pArg) override;
    void         Awake()                     override{}
    void         Priority_Update(_float dt)  override{}
    void         Update(_float dt)           override{}
    void         Late_Update(_float dt)      override{}

    virtual void OnCollisionEnter()          override{}
    virtual void OnCollisionStay()           override{}
    virtual void OnCollisionExit()           override{}
                                             
public:                                      
    void         Render_GUI()                override { __super::Render_GUI(); }

public:
    static CTestPlane* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() { __super::Free(); }
};

NS_END