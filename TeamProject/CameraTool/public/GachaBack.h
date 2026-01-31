#pragma once

NS_BEGIN(CameraTool)

class CGachaBack : public CGameObject
{
private:
	CGachaBack() DEFAULT;
	CGachaBack(const CGachaBack& rhs) : CGameObject(rhs) {}
	virtual ~CGachaBack() DEFAULT;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake()                     override;
    void    Priority_Update(_float dt)  override{}
    void    Update(_float dt)           override{}
    void    Late_Update(_float dt)      override{}

public:
    static CGachaBack* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END