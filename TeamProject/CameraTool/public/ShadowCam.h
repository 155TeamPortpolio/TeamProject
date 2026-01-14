#pragma once

NS_BEGIN(CameraTool)

class CShadowCam final : public CCamObj
{
private:
    CShadowCam() {}
    CShadowCam(const CShadowCam& rhs) :CCamObj(rhs) {}
    virtual ~CShadowCam() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Update(_float dt)           override;

public:
    static  CShadowCam* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END