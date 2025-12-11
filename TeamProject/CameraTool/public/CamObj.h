#pragma once

NS_BEGIN(CameraTool)

class CamObj : public CGameObject
{
private:
    CamObj() = default;
    CamObj(const CamObj& rhs) : CGameObject(rhs) {}
	virtual ~CamObj() = default;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Priority_Update(_float dt)  override;
    void    Update(_float dt)           override;
    void    Late_Update(_float dt)      override;

private:
    static CamObj* Create();
    virtual void Free() override;
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END