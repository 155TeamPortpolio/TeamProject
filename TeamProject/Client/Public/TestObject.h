#pragma once
#include "GameObject.h"

#include "StateMachine.h"

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
    HRESULT Initialize_State();
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

    virtual void OnCollisionEnter() override;
    virtual void OnCollisionStay() override;
    virtual void OnCollisionExit() override;

    void Render_GUI() override;

public:
    CStateMachine<CTestObject>* Get_StateMachine() { return m_pStateMachine; }


private:
    CStateMachine<CTestObject>* m_pStateMachine = { nullptr };
    _float                      m_fSpeed = 3.f;

public:
    static CTestObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
