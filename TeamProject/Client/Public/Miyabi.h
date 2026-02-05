#pragma once
#include "Character.h"

NS_BEGIN(Client)

class CMiyabi final : public CCharacter
{
private:
    CMiyabi();
    CMiyabi(const CMiyabi& rhs);
    virtual ~CMiyabi() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;
    //void          Render_OutLine(ID3D11DeviceContext* pContext, _uint idx);
    
public: // 상태머신
    CStateMachine<CMiyabi>* Get_StateMachine() { return m_pStateMachine; }
    // 회피관련 미야비 특수처리
    virtual _bool Can_Evade() override;

public: // 타겟팅
    _bool   Is_LookTarget() const { return m_bLookTarget; }
    void    Set_LookTarget(_bool bLook) { m_bLookTarget = bLook; }

public: // 서리
    _bool   Has_Frost() const { return m_iFrost > 0; }
    _bool   Can_Charge() const { return m_iFrost >= 2; }
    void    Increase_Frost(_uint iFrost);
    void    Decrease_Frost(_uint iFrost);

public: // 행동 이벤트
    virtual void    Reset_State()              override;
    virtual void    On_Start()                 override;
    virtual void    On_SwitchIn(SWITCH eType)  override;
    virtual void    On_ChainParry()            override;
    virtual void    On_SwitchOut()             override;
    virtual void    On_Ultimate()              override;
    virtual void    On_Special()               override;
    virtual void    On_Hit(DAMAGE_TYPE eType)  override;
    virtual void    OnDamage()                 override;
    virtual void    OnPerfectDodge()           override;
    virtual void    OnDefensiveAssist()        override;

private: // 초기화
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Initialize_Stat();
    HRESULT Initialize_Weapon();
    HRESULT Initialize_Effects();

private: // 상태 처리
    void    Update_States();
    void    Process_AttackInput(const string& strCurrentState);
    void    Process_EndState(const string& strCurrentState);
    
//private:
//    HRESULT Add_OutLineRender();

private:
    // 상태머신
    CStateMachine<CMiyabi>* m_pStateMachine = { nullptr };

    // 타겟팅
    _bool   m_bLookTarget = true;

    // 서리
    _uint   m_iFrost = { 0 };

    _uint   MAX_FROST = { 6 };

public:
    static CMiyabi* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END