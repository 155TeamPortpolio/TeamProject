#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CAnimator3D;
class CCharacterController;
NS_END

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CFieldCharacter abstract:
    public CGameObject
{
public:
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
    struct InputInfo
    {
        _vector3 direction = {};
        _vector3 prevDirection = {};
        _float   bufferTimer = 0.f;

        _int  prevMoveX = 0;
        _int  prevMoveZ = 0;
        _int  curMoveX = 0;
        _int  curMoveZ = 0;
        _bool resetMove = false;
    };
protected:
    CFieldCharacter() {}
    CFieldCharacter(const CFieldCharacter& rhs);
    virtual ~CFieldCharacter() DEFAULT;

public:
    CAnimator3D* Get_Animator() { return m_pAnimator; }
    CCharacterController* Get_CCT() { return m_pCCT; }
    const string& Get_AnimName() const { return m_strAnimName; }

public:
    _vector3    Get_InputDir() const { return m_inputInfo.direction; }
    _vector3    Get_PrevInputDir() const { return m_inputInfo.prevDirection; }
    _bool       Get_InputReset() const { return m_inputInfo.resetMove; }
    void        Reset_InputBuffer() { m_inputInfo.bufferTimer = 0.f; }
    void        Set_ResetMove(_bool bReset) { m_inputInfo.resetMove = bReset; }
    _bool       Is_Move() const { return m_inputInfo.direction.LengthSquared() > 0.01f; }
    _bool       Is_Move_Buffer() const { return m_inputInfo.direction.LengthSquared() > 0.01f || m_inputInfo.bufferTimer > 0.f; }
    _bool       Is_OppositeInput() const;
    _bool       Can_Move() const { return m_bCanMove; }
    void        Lock_Move() { m_bCanMove = false; }
    void        Unlock_Move() { m_bCanMove = true; }
    void        Rotate(_vector3 vDirection);

public:
    void    Active_Character();
    void    DeActive_Character();

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

private:
    void            Update_Rotation(_float dt);
    void            Stop_Rotation();

protected:
    CAnimator3D*                m_pAnimator = { nullptr };
    CCharacterController*       m_pCCT = { nullptr };
    string                      m_strAnimName = "";   
    string                      m_strName = "";       

    _quaternion                 m_qCurrentRot = {};
    _quaternion                 m_qTargetRot = {};
    _bool                       m_bIsRotating = { false };
    _bool                       m_bCanMove = { true };
    InputInfo                   m_inputInfo;
    static constexpr _float     TURNBACK_ANGLE_THRESHOLD = 100.f;
public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};
NS_END