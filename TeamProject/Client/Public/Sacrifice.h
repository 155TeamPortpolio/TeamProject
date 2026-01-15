#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CSacrifice final :
    public CEnemy
{
public:
    enum class PHASE { PHASE1, PHASE2, END };
    enum class PARTS { ICE, WEAPON_SWORD, WEAPON_AXE, WEAPON_WHIP, END };
    enum class DISSOLVE_STATE { DISAPPEAR, APPEAR, NONE, END };
private:
    CSacrifice();
    CSacrifice(const CSacrifice& rhg);
    virtual ~CSacrifice() DEFAULT;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI() override;

public:
    static CSacrifice* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

public:
    /* For State Machine */
    void RotateToTarget(_float dt, _float rotateSpeed);
    void MoveByRootMotion(_float dt, _float moveScale = 1.f);

    void ActiveSword();
    void DeactiveSword();
    void ActiveAxe();
    void DeactiveAxe();
    void ActiveWhip();
    void DeactiveWhip();

    void SetPhase(PHASE phase) { m_eCurrPhase = phase; }
    _bool IsOverDrive()const { return m_IsOverDrive; }
    _bool IsOverDriveCharged()const { return m_IsOverDriveCharged; }

    void SetOverDrive(_bool overdrive) { m_IsOverDrive = overdrive; }
    void SetOverDriveCharged(_bool charged) { m_IsOverDriveCharged = charged; }

    void Idle();
    void Evade();
    void ChangePhase();

    ATTACK_BLACK_BOARD& GetBlackBoard() { return m_AttackBlackBoard; }
    PHASE GetCurrPhase()const { return m_eCurrPhase; }

    /* Hand */
    void Phase1Attack();
    void Phase2Attack();
    void OverDrive_Start();
    void OverDrive_Attack1();
    void OverDrive_Attack2();
    void OverDrive_Attack3();

    /* Laser */
    void ActiveLaser(_uint mode);
    void DeactiveLaser();

private:
    void Create_Children();
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    void Update_States(_float dt);

    /* Dissolve */
    void Set_DissolveState(DISSOLVE_STATE state);
    void Update_Dissolve(_float dt);

private:
    CStateMachine<CSacrifice>* m_pStateMachine{};
    vector<_uint> m_PartMeshIndices;
    ATTACK_BLACK_BOARD m_AttackBlackBoard{};
    _bool m_RequestIdle = false;

    _float m_fIdleElasedTime{};
    _float m_fIdleDuration = 3.05f;
    _float m_fPhase1ElapseTime{};
    _float m_fPhase1Duration = 10.f;

    PHASE m_eCurrPhase = PHASE::PHASE1;
    _bool m_IsOverDrive = false; /* Only Use Phase2 */
    _bool m_IsOverDriveCharged = false;

    _uint m_iHandID{};

    /* Material Params */
    _float3 m_vRimLightColor{};
    _float m_fRimLightPower{};
    _float m_fDissolveProgress{};
    _float m_fDissolveTilling{};

    _float m_fDissolveDuration{};
    _float m_fDissolveElapsedTime{};
    DISSOLVE_STATE m_eDissolveState = DISSOLVE_STATE::NONE;
};
NS_END
