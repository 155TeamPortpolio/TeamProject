#pragma once
#include "Character.h"

NS_BEGIN(Client)

class CCorin final : public CCharacter
{
private:
	CCorin();
	CCorin(const CCorin& rhs);
	virtual ~CCorin() DEFAULT;

public:
    CStateMachine<CCorin>* Get_StateMachine() { return m_pStateMachine; }

public:
    virtual HRESULT     Initialize_Prototype() override;
    virtual HRESULT     Initialize(INIT_DESC* pArg) override;
    virtual void        Awake() override;
    virtual void        Priority_Update(_float dt) override;
    virtual void        Update(_float dt) override;
    virtual void        Late_Update(_float dt) override;
    virtual void        Render_GUI() override;

public:
    virtual void        On_SwitchIn(SWITCH eType)  override;   //*스위치 인 콜*
    virtual void        On_SwitchOut()             override;   //*스위치 아웃 콜*
    virtual void        On_Ultimate()              override;
    virtual void        On_Special()               override;
private:
    HRESULT     Initialize_StateMachine();
    HRESULT     Initialize_States();
    HRESULT     Initialize_Transitions();
    HRESULT     Initialize_Weapon();

private:
    void        Update_States();
    void        Process_AttackInput(const string& strCurrentState);
    void        Process_EndState(const string& strCurrentState);

private:
    CStateMachine<CCorin>* m_pStateMachine = { nullptr };

public:
    static CCorin* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END