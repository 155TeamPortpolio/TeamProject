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
    struct KeyInput
    {
        _int x = 0;
        _int z = 0;

        void  Reset() { x = 0; z = 0; }
        _bool IsZero() const { return x == 0 && z == 0; }
        _bool operator!=(const KeyInput& other) const
        {
            return x != other.x || z != other.z;
        }
    };

    struct InputState
    {
        KeyInput current;
        KeyInput previous;
        KeyInput lastValid;
        KeyInput currentMove;
        KeyInput previousMove;
        _vector3 direction = {};
        _vector3 prevDirection = {};

        _float bufferTimer = 0.f;
        void ResetBuffer()
        {
            bufferTimer = 0.f;
            lastValid.Reset();
            previousMove.Reset();
            currentMove.Reset();
        }
        _bool IsMoving() const
        {
            return direction.x != 0.f || direction.z != 0.f;
        }
    };

public:
    enum class ROOTMOTION_MASK
    {
        MOVE = 1 << 0,  // 0x01 - 루트 모션 이동 사용
        QUATERNION = 1 << 1,  // 0x02 - 루트 모션 회전 사용
    };
    struct ROOTMOTION_DESC
    {
        _uint  iModeMask = ENUM(ROOTMOTION_MASK::MOVE);
        _float fMoveWeight = 1.f;
        _float fRotateWeight = 1.f;
        _float fMoveSpeed = 10.f;
        _float fRotateSpeed = 10.f;
    };
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
    _bool  Is_Move() const { return m_input.IsMoving(); }
    _bool  Is_Move_Buffer() const { return m_input.IsMoving() || m_input.bufferTimer > 0.f; }
    _bool  Is_Attack() const { return m_bIsAttack; }
    _bool  Is_Evade() const { return m_bIsEvade; }
    _bool  Is_Input() const { return m_bIsInput; }

    void   Set_HP(_float fHp) { m_fCurrentHP = fHp; }
    void   Set_MaxHP(_float fMaxHp) { m_fMaxHP = fMaxHp; }
    void   Set_Energy(_float fEnergy) { m_fCurrentEnergy = fEnergy; }
    void   Set_Speed(_float fSpeed) { m_fMoveSpeed = fSpeed; }
    void   Set_Move(_bool bMoving) { m_bIsMove = bMoving; }
    void   Set_SpecialGauge(_float fSpecialGauge) { m_fSpecialGauge = fSpecialGauge; } //*스페셜 게이지*

    void   Process_HP(_float fHP); //*이벤트 버스를 보내는 함수 Set_HP를 ProcessHP 함수 내부에서 호출*

    _vector3    Get_InputDir() const { return m_input.direction; }
    _vector3    Get_PrevInputDir() const { return m_input.prevDirection; }

    CAnimator3D*          Get_Animator() { return m_pAnimator; }
    CCharacterController* Get_CCT() { return m_pCCT; }
    const string&         Get_Name() const { return m_strName; }

public:
    void Process_RootMotion(_float dt, const ROOTMOTION_DESC& desc);
    void Process_RootMotion(_float dt, _uint iModeMask = ENUM(ROOTMOTION_MASK::MOVE));

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

public:
    void     Rotate(_vector3 vDirection);
    _bool    Can_Evade() const;
    void     Use_Evade();
    _bool    Is_OppositeInput() const;
    void     Reset_LastValidKey()
    {
        m_input.lastValid = m_input.current;
        m_input.previousMove = m_input.current;
    }


public:
    // 입력 처리 - 파생 클래스에서 StateMachine 파라미터 설정에 사용
    virtual void    Update_Input(_float dt);

private:
    void    Update_Rotation(_float dt);
    void    Update_Evade(_float dt);

protected:
    CAnimator3D*          m_pAnimator = { nullptr };
    CCharacterController* m_pCCT = { nullptr };
    string                m_strName = "";
    // 스탯
    _float          m_fMaxHP = { 100.f };
    _float          m_fCurrentHP = { 100.f };
    _float          m_fMaxEnergy = { 100.f };
    _float          m_fCurrentEnergy = { 0.f };
    _float          m_fAttackPower = { 10.f };
    _float          m_fDefense = { 5.f };
    _float          m_fMoveSpeed = { 1.f };
    _float          m_fSpecialGauge = { 60.f };         //*스페셜 게이지*
    // 입력
    InputState              m_input;
    static constexpr _float KEY_BUFFER_TIME = 0.1f;
    static constexpr _float TURNBACK_ANGLE_THRESHOLD = 100.f;
    // 상태 플래그
    _bool           m_bIsMove = { false };
    _bool           m_bIsAttack = { false };
    _bool           m_bIsInput = { false };
    _bool           m_bIsEvade = { false };
    // 회피 시스템
    _uint                   m_iEvadeCount = { 0 };
    _float                  m_fEvadeTimer = { 0.f };
    _float                  m_fEvadeCooldown = { 0.f };
    static constexpr _float EVADE_COOLDOWN = 1.f;
    static constexpr _uint  EVADE_MAX_COUNT = 2;
    // 회전
    _quaternion     m_qCurrentRot = {};
    _quaternion     m_qTargetRot = {};
    _bool           m_bIsRotating = { false };

    // 테스트용
    _bool           m_bTest = { false };

public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};

NS_END