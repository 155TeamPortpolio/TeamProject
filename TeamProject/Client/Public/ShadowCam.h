#pragma once
#include "CamObject.h"

NS_BEGIN(Client)

class CShadowCam final:
    public CCamObject
{
private:
    CShadowCam();
    CShadowCam(const CShadowCam& rhs);
    virtual ~CShadowCam() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt)  override; 
    virtual void    Update(_float dt)           override; 
    virtual void    Late_Update(_float dt)      override; 

public:
    static CShadowCam* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END