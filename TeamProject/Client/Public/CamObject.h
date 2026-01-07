#pragma once

#include "GameObject.h"
#include "CameraMgr.h"

NS_BEGIN(Engine)
class CGameInstance; class CLight;
NS_END

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
	virtual void    Priority_Update(_float dt)  override PURE;
	virtual void    Update(_float dt)           override PURE;
	virtual void    Late_Update(_float dt)      override PURE;
	virtual void    Render_GUI()                override;

public:
	virtual CGameObject* Clone(INIT_DESC* pArg) override PURE;
	virtual void Free()                         override { __super::Free(); }
};
NS_END