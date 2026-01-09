#pragma once
#include "Character.h"

NS_BEGIN(Client)

class CJaneDoe final :
    public CCharacter
{
private:
    CJaneDoe();
    CJaneDoe(const CJaneDoe& rhs);
    virtual ~CJaneDoe() DEFAULT;

public:
    CStateMachine<CJaneDoe>* Get_StateMachine() { return m_pStateMachine; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Initialize_Stat();

private:
    virtual void Update_Input(_float dt) override;
    void         Update_States();
    void         Process_AttackInput(const string& strCurrentState);
    void         Process_EndState(const string& strCurrentState);

private:
    CStateMachine<CJaneDoe>* m_pStateMachine = { nullptr };

public:
    static CJaneDoe* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END  