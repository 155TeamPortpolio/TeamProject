#pragma once

#include "GameObject.h"
#include "CameraMgr.h"

NS_BEGIN(Client)
class CCamObject abstract : public CGameObject
{
protected:
	CCamObject() : CGameObject() {} 
	CCamObject(const CCamObject& rhs) : CGameObject(rhs) {}
	virtual ~CCamObject() = default;

public:
	virtual HRESULT Initialize_Prototype()      override;
	virtual HRESULT Initialize(INIT_DESC* pArg) override;
	virtual void    Priority_Update(_float dt)  override {};
	virtual void    Update(_float dt)           override {};
	virtual void    Late_Update(_float dt)      override {};
	virtual void    Render_GUI()                override { __super::Render_GUI(); }
};
NS_END