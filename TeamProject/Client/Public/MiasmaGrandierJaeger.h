#pragma once
#include "Enemy.h"
NS_BEGIN(Client)
class CMiasmaGrandierJaeger :
    public CEnemy
{
    struct MiasmaJaegerDisolveState {
        enum DISSOLVE_STATE { DISAPPEAR, APPEAR, NONE, END };
        DISSOLVE_STATE eDissolveState = NONE;
        _float fDissolveDuration = 2.f;
        _float fDissolveElapsedTime = 0.f;
        _float fDissolveProgress = 0.f;

        void Appear(_float duration) { Set_DissolveState(DISSOLVE_STATE::APPEAR, duration); }
        void DisAppear(_float duration) { Set_DissolveState(DISSOLVE_STATE::DISAPPEAR, duration); }
        void Set_DissolveState(DISSOLVE_STATE state, _float duration)
        {
            eDissolveState = state;
            fDissolveDuration = duration;
            fDissolveElapsedTime = 0.f;
            fDissolveProgress = 0.f;
        }
        _bool isComplete() {
            return fDissolveElapsedTime >= fDissolveDuration;
        }
    };

private:
    CMiasmaGrandierJaeger();
    CMiasmaGrandierJaeger(const CMiasmaGrandierJaeger& rhg);
    virtual ~CMiasmaGrandierJaeger() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI()override;

public:
    virtual void TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName = CHARACTER::END);
    void RotateToTarget(_float dt, _float rotateSpeed);
public:
    virtual void    OnTriggerEnter(CGameObject* pOther) override;
    virtual void OnPooledAcquire(INIT_DESC* pArg = nullptr) override;		// 풀에서 꺼낼 때
    virtual void OnPooledRelease()override;

public:
    MiasmaJaegerDisolveState& Get_Dissolve() { return m_Dissolve; }
    CStateMachine<CMiasmaGrandierJaeger>* Get_MainStateMachine() { return m_pStateMachine; }
    void LockOn(_bool lock) { m_LockedOn = lock; }
private:
    void Update_Dissolve(_float dt);
    void Route_AnimEvent(CAnimator3D* animator);
    _float3 Get_FirePos();
private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Initialize_Effects();

private:
    _bool m_LockedOn = { false };
    MiasmaJaegerDisolveState m_Dissolve;
    CStateMachine<CMiasmaGrandierJaeger>* m_pStateMachine = { nullptr };
    _uint m_HitCount = {};
public:
    static CMiasmaGrandierJaeger* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END