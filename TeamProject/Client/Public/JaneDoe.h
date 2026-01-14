#pragma once
#include "Character.h"

NS_BEGIN(Client)

class CJaneDoe final :
    public CCharacter
{
public:
    enum class COMBATSTATE { NORMAL, PASSION, END };

private:
    CJaneDoe();
    CJaneDoe(const CJaneDoe& rhs);
    virtual ~CJaneDoe() DEFAULT;

public:
    CStateMachine<CJaneDoe>* Get_StateMachine() { return m_pStateMachine; }

    void  Set_Passion(_float fPassionGauge) { m_fPassionGauge = fPassionGauge; }
    _bool Is_Passion() const { return m_eCombatState == COMBATSTATE::PASSION; }

public:
    void Process_Passion(_float fPassionGauge);
    void Process_PassionSkill(_bool bAvailable);

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;

public:
    virtual void    On_SwitchIn(SWITCH eType)  override;   //*스위치 인 콜*
    virtual void    On_SwitchOut()             override;   //*스위치 아웃 콜*
    virtual void    On_Ultimate()              override;
    virtual void    On_Special()               override;

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Initialize_Stat();

private:
    void         Update_States();
    void         Process_AttackInput(const string& strCurrentState);
    void         Process_EndState(const string& strCurrentState);

private: /* Passion */
    COMBATSTATE             m_eCombatState = COMBATSTATE::NORMAL;
    _bool                   m_bPassionSkillAvailable = false;
    _float                  m_fPassionGauge = 0.f;
    static constexpr _float MAX_PASSIONGAUGE = 100.f;

private:
    CStateMachine<CJaneDoe>* m_pStateMachine = { nullptr };

public:
    static CJaneDoe* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END  