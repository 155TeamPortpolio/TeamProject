#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CSacrificeHand :
    public CEnemy
{
public:
    enum class PATTERN { PHASE1, OVER_DRIVE_START, OVER_DRIVE_ATTACK01, OVER_DRIVE_ATTACK02, OVER_DRIVE_ATTACK03, END };

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
    void OverDrive_Start();
    void OverDrive_Attack1();
    void OverDrive_Attack2();
    void OverDrive_Attack3();
    void SetVisable(_bool isActive);

    void Idle();

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();

private:
    CStateMachine<CSacrificeHand>* m_pStateMachine = { nullptr };
    SACRIFICE_HAND_BLACK_BOARD m_AttackBlackBoard{};
};
NS_END
