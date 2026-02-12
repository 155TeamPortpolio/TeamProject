#pragma once
#include "Enemy.h"
#include "Defiler_Control.h"

NS_BEGIN(Client)
class CMiasmaGrandierJaeger :
    public CEnemy
{
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
    DefilerDissolve& Get_Dissolve() { return m_Dissolve; }
    CStateMachine<CMiasmaGrandierJaeger>* Get_MainStateMachine() { return m_pStateMachine; }
    void LockOn(_bool lock) { m_LockedOn = lock; }
private:
    void Update_Dissolve(_float dt);
    void Route_AnimEvent(CAnimator3D* animator);
    _float3 Get_FirePos();
private:
    void Summon_Bullet();
private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Initialize_Effects();

private:
    _bool m_LockedOn = { false };
    DefilerDissolve m_Dissolve;
    CStateMachine<CMiasmaGrandierJaeger>* m_pStateMachine = { nullptr };
    _uint m_HitCount = {};
public:
    static CMiasmaGrandierJaeger* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END