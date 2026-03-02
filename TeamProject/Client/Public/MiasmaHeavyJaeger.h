#pragma once
#include "Enemy.h"
#include "Defiler_Control.h"

NS_BEGIN(Client)
class CMiasmaHeavyJaeger :
    public CEnemy
{
private:
    CMiasmaHeavyJaeger();
    CMiasmaHeavyJaeger(const CMiasmaHeavyJaeger& rhg);
    virtual ~CMiasmaHeavyJaeger() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI()override;

public:
    void MoveByAnim(_float dt, _float moveScale);
    virtual void TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName = CHARACTER::END);
    void RotateToTarget(_float dt, _float rotateSpeed);
    void Dissolve(_bool appear);

public:
    virtual void    OnTriggerEnter(CGameObject* pOther) override;

public:
    DefilerDissolve& Get_Dissolve() { return m_Dissolve; }
    CStateMachine<CMiasmaHeavyJaeger>* Get_MainStateMachine() { return m_pStateMachine; }
    void LockOn(_bool lock) { m_LockedOn = lock; }
    void Parried() override;
    void SpawnChild();

private:
    void Update_Dissolve(_float dt);
    void Route_AnimEvent(CAnimator3D* animator);

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Initialize_Effects();

private:
    _bool m_LockedOn = { false };
    _bool m_bParried = { false };
    _vector3 m_vCurrentDir = {};
    _vector3 m_vVelocity = {};
    _float m_ElapseTime = 0;
    DefilerDissolve m_Dissolve;
    CStateMachine<CMiasmaHeavyJaeger>* m_pStateMachine = { nullptr };
  
public:
    static CMiasmaHeavyJaeger* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END