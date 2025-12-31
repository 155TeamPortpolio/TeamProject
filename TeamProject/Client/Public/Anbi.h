#pragma once
#include "Character.h"

NS_BEGIN(Client)

class CAnbi final : public CCharacter
{
private:
	CAnbi();
	CAnbi(const CAnbi& rhs);
	virtual ~CAnbi() DEFAULT;

public:
	CStateMachine<CAnbi>* Get_StateMachine() { return m_pStateMachine; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();

private:
    virtual void Update_Input(_float dt) override;
    void         Update_States();

private:
	CStateMachine<CAnbi>* m_pStateMachine = { nullptr };

public:
    static CAnbi* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END