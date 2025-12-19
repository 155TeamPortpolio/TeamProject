#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CTestObject :
    public CGameObject
{
private:
    CTestObject();
    CTestObject(const CTestObject& rhs);
    virtual ~CTestObject() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

    virtual void OnCollisionEnter() override;
    virtual void OnCollisionStay() override;
    virtual void OnCollisionExit() override;

public:
    void Render_GUI() override;

public:
    static CTestObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
