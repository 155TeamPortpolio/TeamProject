#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CSacrificeHand :
    public CEnemy
{
public:
    enum class DISSOLVE_STATE { APPEAR, DISAPPEAR, NONE, END };
    enum class PATTERN { PHASE1, PHASE2, OVER_DRIVE_START, OVER_DRIVE_ATTACK01, OVER_DRIVE_ATTACK02, OVER_DRIVE_ATTACK03, END };

    typedef struct tagSacrificeHandBlackBoard : public ATTACK_BLACK_BOARD
    {
        PATTERN eCurrPattern = PATTERN::END;
    }SACRIFICE_HAND_BLACK_BOARD;

private:
    CSacrificeHand();
    CSacrificeHand(const CSacrificeHand& rhg);
    virtual ~CSacrificeHand() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

public:
    static CSacrificeHand* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

public:
    SACRIFICE_HAND_BLACK_BOARD& GetBlackBoard() { return m_AttackBlackBoard; }
    void Phase1Attack();
    void Phase2Attack();
    void OverDrive_Start();
    void OverDrive_Attack1();
    void OverDrive_Attack2();
    void OverDrive_Attack3();
    void SetVisable(_bool isActive);

    void Idle();

    /* Bubble */
    void Active_Bubble();
    void Deactive_Bubble();

    /* Dissolve */
    void Set_DissolveState(DISSOLVE_STATE state, _float duration);
    DISSOLVE_STATE Get_DissolveState()const { return m_eDissolveState; }
    void Update_Dissolve(_float dt);

    /* RimLight */
    void Active_SwordRimLight() { m_fSwordRimLightPower = 2.f; }
    void Deactive_SwordRimLight() { m_fSwordRimLightPower = 0.f; }

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Create_Colliders();

private:
    CStateMachine<CSacrificeHand>* m_pStateMachine = { nullptr };
    SACRIFICE_HAND_BLACK_BOARD m_AttackBlackBoard{};

    /* Material Params */
    _bool m_IsOnDissolve = false;
    _float m_fSwordRimLightPower{};
    DISSOLVE_STATE m_eDissolveState = DISSOLVE_STATE::NONE;
};
NS_END
