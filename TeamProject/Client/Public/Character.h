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
public:
    typedef struct tagAttackColliderInitDesc {
        string          tagName = "";
        string          tagBone = "";
        CAnimator3D* pOwnerAnimator = { nullptr };
        COLLIDER_TYPE   eColliderType = COLLIDER_TYPE::SPHERE;
        _float3         vSize = { 1.f, 1.f, 1.f };
        _float3         vCenter = { 0.f, 0.f, 0.f };
        _float3         vRotation = { 0.f, 0.f, 0.f };
    }ATTACK_COLLIDER_DESC;

    struct InputInfo
    {
        _vector3 direction = { 0, 0, 0 };
        _vector3 prevDirection = {};
        _float   bufferTimer = 0.f;
        _int  prevMoveX = 0;
        _int  prevMoveZ = 0;
        _int  curMoveX = 0;
        _int  curMoveZ = 0;
        _bool resetMove = false;
    };

    enum class ROOTMOTION_MASK
    {
        MOVE = 1 << 0,
        QUATERNION = 1 << 1,
    };

    struct ROOTMOTION_DESC
    {
        _uint  iModeMask = ENUM(ROOTMOTION_MASK::MOVE);
        _float fMoveWeight = 1.f;
        _float fRotateWeight = 1.f;
        _float fMoveSpeed = 10.f;
        _float fRotateSpeed = 10.f;
    };

    struct EnergyDesc
    {
        _float fCurrentEnergy = { 0.f };
        _float fPrevEnergy = { 0.f };
        _float fEnergyWeight = { 10.f };
        _float fSpecialEnergy = { 60.f };
    };

    enum class SWITCH { NORMAL, ATTACK, PARRYAID, END };

protected:
    CCharacter() {}
    CCharacter(const CCharacter& rhs);
    virtual ~CCharacter() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;

    virtual void    OnTriggerEnter(CGameObject* pOther) override;
    virtual void    OnTriggerStay(CGameObject* pOher) override;
    virtual void    OnTriggerExit(CGameObject* pOther) override;

public: // 컴포넌트 접근
    CAnimator3D* Get_Animator() { return m_pAnimator; }
    CCharacterController* Get_CCT() { return m_pCCT; }

public: // 기본 정보
    const string& Get_Name() const { return m_strAnimName; }
    const CHARACTER Get_CharacterName() const { return m_eCharacterName; }
    void   Set_Speed(_float fSpeed) { m_fMoveSpeed = fSpeed; }
    _float Get_Speed() const { return m_fMoveSpeed; }

public: // HP
    _float  Get_HP() const { return m_fCurrentHP; }
    _float  Get_MaxHP() const { return m_fMaxHP; }
    void    Set_MaxHP(_float fMaxHp) { m_fMaxHP = fMaxHp; }
    _float  Get_AttackPower() const { return m_fAttackPower; }

public: // 에너지
    const EnergyDesc& Get_EnergyDesc() const { return m_tEnergy; }
    void    Set_EnergyDesc(EnergyDesc desc) { m_tEnergy = desc; }
    void    Set_CurrentEnergy(_float fEnergy) { m_tEnergy.fCurrentEnergy = fEnergy; }
    void    Set_EnergyWeight(_float fWeight) { m_tEnergy.fEnergyWeight = fWeight; }
    void    Set_SpecialEnergy(_float fEnergy) { m_tEnergy.fSpecialEnergy = fEnergy; }
    _float  Get_MaxEnergy() { return MAX_ENERGY; }

public: // 궁극기
    _float  Get_CurrentDecibel() const { return m_fCurrentDecibel; }
    _float  Get_PrevDecibel() const { return m_fPrevDecibel; }
    _float  Get_MaxDecibel() const { return MAX_DECIBEL; }
    void    Set_Decibel(_float fDecibel) { m_fCurrentDecibel = fDecibel; }

public: // 이동
    _vector3    Get_InputDir() const { return m_inputInfo.direction; }
    _vector3    Get_PrevInputDir() const { return m_inputInfo.prevDirection; }
    _bool       Get_InputReset() const { return m_inputInfo.resetMove; }
    void        Reset_InputBuffer() { m_inputInfo.bufferTimer = 0.f; }
    void        Set_ResetMove(_bool bReset) { m_inputInfo.resetMove = bReset; }
    _bool       Is_Move() const { return m_inputInfo.direction.LengthSquared() > 0.01f; }
    _bool       Is_Move_Buffer() const { return m_inputInfo.direction.LengthSquared() > 0.01f || m_inputInfo.bufferTimer > 0.f; }
    void        Reset_InputInfo() { m_inputInfo = {}; }
    _bool       Can_Move() const { return m_bCanMove; }
    void        Lock_Move() { m_bCanMove = false; }
    void        Unlock_Move() { m_bCanMove = true; }
    _bool       Can_Rotate() const { return m_bCanRotate; }
    void        Lock_Rotate() { m_bCanRotate = false; }
    void        Unlock_Rotate() { m_bCanRotate = true; }

public: // 상태
    _bool       Is_Attack() const { return m_bIsAttack; }
    _bool       Is_Evade() const { return m_bIsEvade; }
    _bool       Is_Input() const { return m_bIsAttack || Is_Move() || m_bIsEvade; }
    _float      Get_EvadeTimer() const { return m_fEvadeTimer; }
    _float      Get_EvadeCooldown() const { return m_fEvadeCooldown; }
    _uint       Get_EvadeCount() const { return m_iEvadeCount; }
    void        Set_EvadeMax(_uint iMax) { m_iEvadeMax = iMax; }
    void        Reset_Interact() { m_bCanInteract = false; }
    void        Reserve_ComboAttack() { m_bReserveCombo = true; }
    void        Reset_ReserveCombo() { m_bReserveCombo = false; }
    _bool       Is_ReserveCombo() const { return m_bReserveCombo; }

public: // 캐릭터 전환
    SWITCH      Get_Switch() const { return m_eSwitchType; }
    void        Set_Switch(SWITCH eType) { m_eSwitchType = eType; }
    _bool       Is_MainCharacter() const { return m_bIsMain; }
    void        Set_MainCharacter(_bool bMain) { m_bIsMain = bMain; }
    _bool       Can_SwitchIn() const;
    void        Active_Character();
    void        DeActive_Character();
    void        Update_DissolveProgress(_float dt);

public: // 타겟팅
    OBJECT_HANDLE   Get_TargetHandle() { return m_TargetHandle; }
    void            Set_TargetHandle(OBJECT_HANDLE targetHandle) { m_TargetHandle = targetHandle; }

public: // 패링
    OBJECT_HANDLE   Get_ParryHandle() { return m_ParryHandle; }
    void            Set_ParryHandle(OBJECT_HANDLE parryHandle) { m_ParryHandle = parryHandle; }
    _bool           Can_Parry();
    OBJECT_HANDLE   Calculate_Parry();
    _vector3        Get_ParryPos() { return m_vParryPos; }
    _vector3        Get_ParryLook() { return m_vParryLook; }

public: // 무적
    _bool Is_Invincible() const { return m_iInvincibleCount > 0 || m_fInvincibleTimer > 0.f; }
    void Push_Invincible() { ++m_iInvincibleCount; }
    void Pop_Invincible() { if (m_iInvincibleCount > 0) --m_iInvincibleCount; }
    void Set_InvincibleTimer(_float fDuration) { m_fInvincibleTimer = fDuration; }

public: // 행동 이벤트
    virtual void    Reset_State() {}
    virtual void    On_Start() {}
    virtual void    On_Move(const InputInfo& inputInfo);
    virtual void    On_Attack();
    virtual void    On_Evade();
    virtual void    On_SwitchIn(SWITCH eType) {}
    virtual void    On_SwitchOut();
    virtual void    On_Ultimate();
    virtual void    On_Special() {}
    virtual void    On_Hit(DAMAGE_TYPE eType) {}
    virtual void    On_Interact();
    virtual void    OnDamage() {}
    virtual void    OnPerfectDodge() {}
    virtual void    OnDefensiveAssist() {}

public: // 루트모션
    void Process_RootMotion(_float dt, const ROOTMOTION_DESC& desc);
    void Process_RootMotion(_float dt, _uint iModeMask = ENUM(ROOTMOTION_MASK::MOVE));

public: // 회전
    void     Rotate(_vector3 vDirection);
    void     Stop_Rotation();
    void     Look_Target();
    _bool    Is_OppositeInput() const;

public: // 회피
    _bool    Can_Evade() const;
    void     Use_Evade();
    void     Buffer_Evade() { m_bEvadeBuffer = true; }
    _bool    Use_EvadeBuffer();
    _bool    Can_Ultimate();

public: // 공격 & 패링 콜라이더
    HRESULT  Attach_AttackCollider(ATTACK_COLLIDER_DESC* pDesc);
    HRESULT  Attach_ParryCollider();
    void     Active_ParryCollider(_bool bActive);
    void     Active_AttackCollider(const string& strName, _bool bActive);
    void     Begin_AttackCollider(const string& strName, const HitDesc& hitdesc);
    void     End_AttackCollider(const string& strName);
    void     End_AllAttackColliders();
    _bool    Is_Active_AttackCollider(const string& strName);

public: // 피격
    void     Take_Damage(DAMAGE_TYPE eType, _float fDamage);
    _vector3 Get_HitTargetPos() const { return m_vHitPos; }

private:
    void    Update_Rotation(_float dt);
    void    Update_Evade(_float dt);
    void    Update_Energy(_float dt);
    void    Update_Decibel(_float dt);
    void    Update_Invincible(_float dt);
    class CCharacterAttackCollider* Find_AttackCollider(const string& strName);

protected:
    // 컴포넌트
    CAnimator3D* m_pAnimator = { nullptr };
    CCharacterController* m_pCCT = { nullptr };
    // 기본 정보
    string      m_strAnimName = "";
    string      m_strName = "";
    CHARACTER   m_eCharacterName = { CHARACTER::JaneDoe };
    _float      m_fMoveSpeed = { 1.f };
    // HP
    _float      m_fMaxHP = { 100.f };
    _float      m_fCurrentHP = { 100.f };
    _float      m_fAttackPower = { 10.f };
    _float      m_fDefense = { 5.f };
    // 이동/회전
    InputInfo   m_inputInfo;
    _bool       m_bCanMove = { true };
    _bool       m_bCanRotate = { true };
    _quaternion m_qCurrentRot = {};
    _quaternion m_qTargetRot = {};
    _bool       m_bIsRotating = { false };
    // 상태
    _bool       m_bIsMain = { false };
    _bool       m_bIsAttack = { false };
    _bool       m_bIsEvade = { false };
    _bool       m_bCanInteract = { false };
    _bool       m_bReserveCombo = { false };
    // 회피
    _bool       m_bEvadeBuffer = { false };
    _uint       m_iEvadeMax = 2;
    _uint       m_iEvadeCount = { 0 };
    _float      m_fEvadeTimer = { 0.f };
    _float      m_fEvadeCooldown = { 0.f };
    // 에너지
    EnergyDesc  m_tEnergy = {};
    // 궁극기
    _float      m_fCurrentDecibel = {};
    _float      m_fPrevDecibel = {};
    // 무적
    _int        m_iInvincibleCount = { 0 };
    _float      m_fInvincibleTimer = { 0.f };
    // 타겟팅
    OBJECT_HANDLE   m_TargetHandle;
    _bool           m_bLockOn = { false };
    // 패링
    _int            m_iParryColliderIndex = {};
    SWITCH          m_eSwitchType = SWITCH::END;
    _float          m_fParryOffset = { 1.8f };
    _vector3        m_vParryPos = {};
    _vector3        m_vParryLook = {};
    OBJECT_HANDLE   m_ParryHandle;
    // 피격
    _vector3        m_vHitPos = {};
    // 공격 콜라이더
    unordered_map<string, _int> m_AttackColliderIndex;
    // 이펙트
    _float3     m_vRimLightColor = _float3(0.f, 0.f, 0.f);
    _float      m_fRimLightPower = { 0.f };
    _float      m_fDissolveProgress = { 0.f };
    _float      m_fDissolveTiling = { 10.f };
    // 테스트
    _bool       m_bTest = { false };

    static constexpr _float TURNBACK_ANGLE_THRESHOLD = 100.f;
    static constexpr _float EVADE_COOLDOWN = 1.f;
    static constexpr _float MAX_ENERGY = { 120.f };
    static constexpr _float MAX_DECIBEL = { 3000.f };

public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};

NS_END