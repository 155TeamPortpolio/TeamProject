#pragma once
#include "Character.h"

NS_BEGIN(Client)

class CJaneDoe final : public CCharacter
{
private:
    CJaneDoe();
    CJaneDoe(const CJaneDoe& rhs);
    virtual ~CJaneDoe() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;

public: // 상태머신
    CStateMachine<CJaneDoe>* Get_StateMachine() { return m_pStateMachine; }

public: // 타겟팅
    _bool   Is_LookTarget() const { return m_bLookTarget; }
    void    Set_LookTarget(_bool bLook) { m_bLookTarget = bLook; }

public: // 열광
    _bool   Is_Passion() const { return m_bPassion; }
    void    Increase_Passion(_float fStream);
    void    Decrease_Passion(_float fStream);
    _bool   Can_Salchow() const { return m_bCanSalchow; }
    void    Set_Salchow(_bool bSalchow) { m_bCanSalchow = bSalchow; }

public: // 행동 이벤트
    virtual void    Reset_State()              override;
    virtual void    On_Start()                 override;
    virtual void    On_SwitchIn(SWITCH eType)  override;
    virtual void    On_SwitchOut()             override;
    virtual void    On_Ultimate()              override;
    virtual void    On_Special()               override;
    virtual void    On_Hit(DAMAGE_TYPE eType)  override;
    virtual void    OnDamage()                 override;
    virtual void    OnPerfectDodge()           override;
    virtual void    OnDefensiveAssist()        override;

public: // 이펙트
    void Play_Effect(const string& effectTag, _fvector offsetPosition, _fvector offsetQuaternion, _bool syncTransform = true);

public: // 모션블러
    void Update_MotionBlurQueue();

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

private: // 모션블러 렌더
    HRESULT Add_PassionMotionBlur();
    HRESULT Render_PassionMotionBlur(ID3D11DeviceContext* pContext, _uint index);

private:
    // 상태머신
    CStateMachine<CJaneDoe>* m_pStateMachine = { nullptr };

    // 타겟팅
    _bool   m_bLookTarget = true;

    // 열광
    _bool                   m_bPassion = false;
    _float                  m_fPassionStream = 0.f;
    _bool                   m_bCanSalchow = false;
    static constexpr _float MAX_PASSIONSTREAM = 100.f;

    // 모션블러
    deque<vector<vector<_float4x4>>>    m_BoneMatrices;
    deque<_float4x4>                    m_WorldMatrices;
    _uint                               m_iFrameCount = 0;
    static constexpr _uint              FRAMECOUNT = 7;

public:
    static CJaneDoe* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END