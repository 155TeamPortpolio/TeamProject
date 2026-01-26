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

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;

public:
    virtual void    Reset_State()              override;
    virtual void    On_Start()                 override;
    virtual void    On_SwitchIn(SWITCH eType)  override;   //*스위치 인 콜*
    virtual void    On_SwitchOut()             override;   //*스위치 아웃 콜*
    virtual void    On_Ultimate()              override;
    virtual void    On_Special()               override;
    virtual void    On_Hit(DAMAGE_TYPE eType)  override;

    virtual void    OnDamage()                 override;
    virtual void    OnPerfectDodge()           override;
    virtual void    OnDefensiveAssist()        override;

public:
    _bool   Is_Passion() const { return m_bPassion; }
    void    Increase_Passion(_float fStream);
    void    Decrease_Passion(_float fStream);
    _bool   Can_Salchow() const { return m_bCanSalchow; }
    void    Set_Salchow(_bool bSalchow) { m_bCanSalchow = bSalchow; }

    void Update_MotionBlurQueue();

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Initialize_Stat();
    HRESULT Initialize_Weapon();

    /* 이펙트 생성 */
    HRESULT Initialize_Effects();

private:
    HRESULT Add_PassionMotionBlur();
    HRESULT Render_PassionMotionBlur(ID3D11DeviceContext* pContext, _uint index);

private:
    void         Update_States();
    void         Process_AttackInput(const string& strCurrentState);
    void         Process_EndState(const string& strCurrentState);

private: /* Passion */
    _bool                   m_bPassion = false;
    _float                  m_fPassionStream = 0.f;
    _bool                   m_bCanSalchow = false;
    static constexpr _float MAX_PASSIONSTREAM = 100.f;

private:
    CStateMachine<CJaneDoe>* m_pStateMachine = { nullptr };
    deque<vector<vector<_float4x4>>>        m_BoneMatrices;
    deque<_float4x4>                        m_WorldMatrices;

    _uint                   m_iFrameCount = 0;
    static constexpr _uint  FRAMECOUNT = 7;
public:
    static CJaneDoe* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END  