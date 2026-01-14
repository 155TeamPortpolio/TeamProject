#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CCharacterAttackCollider : public CGameObject
{
private:
	CCharacterAttackCollider();
	CCharacterAttackCollider(const CCharacterAttackCollider& rhs);
	virtual ~CCharacterAttackCollider() DEFAULT;

public:
	HRESULT         Initialize_Prototype() override;
	HRESULT         Initialize(INIT_DESC* pArg) override;
	void	        Awake() override;
    void            Priority_Update(_float dt) override;
    void            Update(_float dt) override;
    void            Late_Update(_float dt) override;
    virtual void    Render_GUI() override;
    virtual void    OnCollisionEnter(CGameObject* pOther) override;
    virtual void    OnCollisionStay(CGameObject* pOther) override;
    virtual void    OnCollisionExit(CGameObject* pOther) override;
    virtual void    OnTriggerEnter(CGameObject* pOther) override;
    virtual void    OnTriggerStay(CGameObject* pOther) override;
    virtual void    OnTriggerExit(CGameObject* pOther) override;

public:
    static CCharacterAttackCollider* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END