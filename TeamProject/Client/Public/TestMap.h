#pragma once

#include "GameObject.h"

NS_BEGIN(Client)

class CTestMap final : public CGameObject
{
private:
    CTestMap() {}
    CTestMap(const CTestMap& rhs) : CGameObject(rhs) {}
    virtual ~CTestMap() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override{}
    virtual void    Update(_float dt)           override{}
    virtual void    Late_Update(_float dt)      override{}
                                                        
    virtual void    OnCollisionEnter()          override{}
    virtual void    OnCollisionStay()           override{}
    virtual void    OnCollisionExit()           override{}

public:
    void Render_GUI() override;

public:
    static CTestMap* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};

NS_END