// Character.h
#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CAnimator3D;
class CCharacterController;
NS_END

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CCharacter abstract : public CGameObject
{
protected:
    CCharacter() {}
    CCharacter(const CCharacter& rhs);
    virtual ~CCharacter() DEFAULT;

public:
    // 상태 접근
    _float Get_HP() const { return m_fCurrentHP; }
    _float Get_MaxHP() const { return m_fMaxHP; }
    _float Get_Energy() const { return m_fCurrentEnergy; }
    _float Get_Speed() const { return m_fMoveSpeed; }
    _bool  Is_Move() const { return m_bIsMove; }
    _bool  Is_Attack() const { return m_bIsAttack; }
    
    void   Set_HP(_float fHp) { m_fCurrentHP = fHp; }
    void   Set_MaxHP(_float fMaxHp) { m_fMaxHP = fMaxHp; }
    void   Set_Energy(_float fEnergy) { m_fCurrentEnergy = fEnergy; }
    void   Set_Speed(_float fSpeed) { m_fMoveSpeed = fSpeed; }
    void   Set_Move(_bool bMoving) { m_bIsMove = bMoving; }

    _vector3              Get_InputDir() const { return m_vInputDir; }
    CAnimator3D*          Get_Animator() { return m_pAnimator; }
    CCharacterController* Get_CCT() { return m_pCCT; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

public:
    void Rotate(_vector3 vDirection);

protected:
    // 입력 처리 - 파생 클래스에서 StateMachine 파라미터 설정에 사용
    virtual void Update_Input(_float dt);

protected:
    CAnimator3D*          m_pAnimator = { nullptr };
    CCharacterController* m_pCCT = { nullptr };

protected:
    // 스탯
    _float   m_fMaxHP = { 100.f };
    _float   m_fCurrentHP = { 100.f };
    _float   m_fMaxEnergy = { 100.f };
    _float   m_fCurrentEnergy = { 0.f };
    _float   m_fAttackPower = { 10.f };
    _float   m_fDefense = { 5.f };
    _float   m_fMoveSpeed = { 1.f };
    _vector3 m_vInputDir = {};
    // 상태 플래그
    _bool  m_bIsMove = { false };
    _bool  m_bIsAttack = { false };

public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};

NS_END