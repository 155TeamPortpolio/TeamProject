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
public:
    struct InputInfo
    {
        _vector3 direction = {};
        _vector3 prevDirection = {};
        _float   bufferTimer = 0.f;
        // Turnback ������ �߰�
        _int  prevMoveX = 0;
        _int  prevMoveZ = 0;
        _int  curMoveX = 0;
        _int  curMoveZ = 0;
        _bool resetMove = false;
    };
    enum class ROOTMOTION_MASK
    {
        MOVE = 1 << 0,  // 0x01 - ��Ʈ ��� �̵� ���
        QUATERNION = 1 << 1,  // 0x02 - ��Ʈ ��� ȸ�� ���
    };
    struct ROOTMOTION_DESC
    {
        _uint  iModeMask = ENUM(ROOTMOTION_MASK::MOVE);
        _float fMoveWeight = 1.f;
        _float fRotateWeight = 1.f;
        _float fMoveSpeed = 10.f;
        _float fRotateSpeed = 10.f;
    };

    struct GaugeDesc
    {
        _float      fCurrentGauge = { 0.f };
        _float      fPrevGauge = { 0.f };
        _float      fGaugeWeight = { 1.f };
        _float      fSpecialGauge = { 60.f };
        void        Set_CurrentGauge(_float fGauge) { fCurrentGauge = fGauge; }
        void        Set_GaugeWeight(_float fWeight) { fGaugeWeight = fWeight; }
        void        Set_SpecialGauge(_float fGauge) { fSpecialGauge = fGauge; }
    };

public:
    enum class SWITCH { NORMAL, ATTACK, EXATTACK, PARRYAID, END };

protected:
    CCharacter() {}
    CCharacter(const CCharacter& rhs);
    virtual ~CCharacter() DEFAULT;

public:
    // ������ �ý���
    const GaugeDesc& Get_GaugeDesc() const { return m_tGauge; }
    void    Set_GaugeDesc(GaugeDesc desc) { m_tGauge = desc; }
    _float  Get_MaxGauge() { return MAX_SPECIALGAUGE; }

    // ����
    _float  Get_HP() const { return m_fCurrentHP; }
    _float  Get_MaxHP() const { return m_fMaxHP; }
    void    Set_HP(_float fHp) { m_fCurrentHP = fHp; }
    void    Set_MaxHP(_float fMaxHp) { m_fMaxHP = fMaxHp; }

    _float  Get_Decibel() const { return m_fDecibel; }
    _float  Get_MaxDecibel() const { return MAX_DECIBEL; }
    void    Set_Decibel(_float fDecibel) { m_fDecibel = fDecibel; }

    _float Get_Speed() const { return m_fMoveSpeed; }

    // ����
    _bool  Is_Move() const { return m_inputInfo.direction.LengthSquared() > 0.01f; }
    _bool  Is_Move_Buffer() const { return m_inputInfo.direction.LengthSquared() > 0.01f || m_inputInfo.bufferTimer > 0.f; }
    _bool  Is_Attack() const { return m_bIsAttack; }
    _bool  Is_Evade() const { return m_bIsEvade; }
    _bool  Is_Input() const { return m_bIsAttack || Is_Move() || m_bIsEvade; }
    _float Get_EvadeTimer() const { return m_fEvadeTimer; }
    _float Get_EvadeCooldown() const { return m_fEvadeCooldown; }
    _uint  Get_EvadeCount() const { return m_iEvadeCount; }

    void   Set_Speed(_float fSpeed) { m_fMoveSpeed = fSpeed; }
    void   Set_EvadeMax(_uint iMax) { m_iEvadeMax = iMax; }

    void   Process_HP(_float fHP, UI_STATUS_OWNER ower = UI_STATUS_OWNER::ROLE1); //*�̺�Ʈ ������ ������ �Լ� Set_HP�� ProcessHP �Լ� ���ο��� ȣ��*

    _vector3    Get_InputDir() const { return m_inputInfo.direction; }
    _vector3    Get_PrevInputDir() const { return m_inputInfo.prevDirection; }
    _bool       Get_InputReset() const { return m_inputInfo.resetMove; }
    void        Reset_InputBuffer() { m_inputInfo.bufferTimer = 0.f; }
    void        Set_ResetMove(_bool bReset) { m_inputInfo.resetMove = bReset; }
    
    CAnimator3D*          Get_Animator() { return m_pAnimator; }
    CCharacterController* Get_CCT() { return m_pCCT; }
    const string&         Get_Name() const { return m_strAnimName; }

    SWITCH      Get_Switch() const { return m_eSwitchType; } //*statemachine���� ������ switchtype*
    void        Set_Switch(SWITCH eType) { m_eSwitchType = eType; }

    void        Update_DissolveProgress(_float dt); /*dissolve*/
    void        Reset_DissolveProgress();

    OBJECT_HANDLE       Get_TargetHandle() { return m_TargetHandle; };
    void                Set_TargetHandle(OBJECT_HANDLE targetHandle) { m_TargetHandle = targetHandle; };

public:
    void Process_RootMotion(_float dt, const ROOTMOTION_DESC& desc);
    void Process_RootMotion(_float dt, _uint iModeMask = ENUM(ROOTMOTION_MASK::MOVE));

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

    virtual void    OnCollisionExit(CGameObject* pOther) override;
    virtual void    OnTriggerEnter(CGameObject* pOther) override;
    virtual void    OnTriggerStay(CGameObject* pOher) override;
    virtual void    OnTriggerExit(CGameObject* pOther) override;

public:
    virtual void    On_Move(const InputInfo& inputInfo);
    virtual void    On_Attack();
    virtual void    On_Evade();
    virtual void    On_SwitchIn(SWITCH eType)   PURE;   //*����ġ �� ��*
    virtual void    On_SwitchOut()              PURE;   //*����ġ �ƿ� ��*
    virtual void    On_Ultimate() { m_fDecibel = 0.f; };

public:
    void     Rotate(_vector3 vDirection);

    _bool    Is_OppositeInput() const;
    _bool    Can_Parry() const;

    _bool    Can_Evade() const;
    void     Use_Evade();
    void     Buffer_Evade() { m_bEvadeBuffer = true; }
    _bool    Use_EvadeBuffer();

    _bool    Can_Ultimate();

private:
    void    Update_Rotation(_float dt);
    void    Update_Evade(_float dt);
    void    Update_Gauge(_float dt);
    void    Update_Decibel(_float dt);

protected:
    CAnimator3D*          m_pAnimator = { nullptr };
    CCharacterController* m_pCCT = { nullptr };
    string                m_strAnimName = "";   //*�ִϸ��̼� �����̸�*
    string                m_strName = "";       //*ĳ���� �̸�*

    // ����
    GaugeDesc   m_tGauge = {};
    static  constexpr _float    MAX_SPECIALGAUGE = { 120.f };
    _float          m_fMaxHP = { 100.f };
    _float          m_fCurrentHP = { 100.f };
    _float          m_fAttackPower = { 10.f };
    _float          m_fDefense = { 5.f };
    _float          m_fMoveSpeed = { 1.f };
    _uint           m_iCurrentLevel = { 1 };            //*ĳ���� ����*
    _float          m_fDecibel = {};
    static constexpr _float MAX_DECIBEL = { 3000 };
    // �Է�
    InputInfo       m_inputInfo;
    _bool           m_bIsAttack = { false };
    _bool           m_bIsEvade = { false };
    // ȸ�� �ý���
    _bool                   m_bEvadeBuffer = { false };
    _uint                   m_iEvadeMax = 2;
    _uint                   m_iEvadeCount = { 0 };
    _float                  m_fEvadeTimer = { 0.f };
    _float                  m_fEvadeCooldown = { 0.f };
    static constexpr _float EVADE_COOLDOWN = 1.f;
    //*����ġ �ý���*
    SWITCH                  m_eSwitchType = SWITCH::END;
    // ȸ��
    _quaternion     m_qCurrentRot = {};
    _quaternion     m_qTargetRot = {};
    _bool           m_bIsRotating = { false };
    // �и�
    unordered_set<CGameObject*>  m_ParryableTargets;
    // �׽�Ʈ��
    _bool           m_bTest = { false };
    //���̴�
    _float3     m_vRimLightColor = _float3(0.f, 0.f, 0.f);
    _float      m_fRimLightPower = { 0.f };
    _float      m_fDissolveProgress = { 0.f };
    _float      m_fDissolveTiling = { 10.f };
    // ����
    OBJECT_HANDLE                 m_TargetHandle;
    static constexpr _float TURNBACK_ANGLE_THRESHOLD = 100.f;

public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};

NS_END