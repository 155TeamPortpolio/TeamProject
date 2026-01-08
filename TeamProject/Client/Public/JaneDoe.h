#pragma once
#include "Character.h"

NS_BEGIN(Client)

class CJaneDoe final :
    public CCharacter
{
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

private:
    CJaneDoe();
    CJaneDoe(const CJaneDoe& rhs);
    virtual ~CJaneDoe() DEFAULT;

public:
    CStateMachine<CJaneDoe>* Get_StateMachine() { return m_pStateMachine; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;

public:
    void Process_RootMotion(_float dt, const ROOTMOTION_DESC& desc);
    void Process_RootMotion(_float dt, _uint iModeMask = ENUM(ROOTMOTION_MASK::MOVE));

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();

private:
    virtual void Update_Input(_float dt) override;
    void         Update_States();
    void         Process_AttackInput(const string& strCurrentState);
    void         Process_EndState(const string& strCurrentState);

private:
    CStateMachine<CJaneDoe>* m_pStateMachine = { nullptr };

public:
    static CJaneDoe* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END  