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
    CTestStateMachine* Get_StateMachine() { return m_pStateMachine; }
    _vector3           Get_InputDir() const { return m_vInputDir; }
    _bool              Is_Jump() const { return m_bJump; }
    _float             Get_Speed() const { return m_fSpeed; }
    _float             Get_JumpPower() const { return m_fJumpPower; }

public:
    HRESULT            Initialize_Prototype() override;
    HRESULT            Initialize(INIT_DESC* pArg) override;
    HRESULT            Initialize_State();
    void               Awake() override;
    void               Priority_Update(_float dt) override;
    void               Update(_float dt) override;
    void               Late_Update(_float dt) override;

    virtual void       OnCollisionEnter() override;
    virtual void       OnCollisionStay() override;
    virtual void       OnCollisionExit() override;

    void               Render_GUI() override;

private:
    CTestStateMachine* m_pStateMachine = { nullptr };
    _vector3                    m_vInputDir = {};
    _bool                       m_bJump = { false };
    _float                      m_fSpeed = 5.f;
    _float                      m_fJumpPower = 3.f;

public:
    static CTestObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
