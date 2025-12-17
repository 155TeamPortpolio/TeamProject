#pragma once

#include "GameObject.h"
#include "CameraMgr.h"

NS_BEGIN(Client)
class CGameInstance;

class CCamObject abstract : public CGameObject
{
protected:
	CCamObject() : CGameObject() {}
	CCamObject(const CCamObject& rhs) : CGameObject(rhs), game(rhs.game) {}
	virtual ~CCamObject() DEFAULT;

public:
	HRESULT Initialize_Prototype()      override;
	HRESULT Initialize(INIT_DESC* pArg) override;

	void    Priority_Update(_float dt)  override PURE;
	void    Update(_float dt)           override PURE;
	void    Late_Update(_float dt)      override PURE;

protected:
	CGameInstance* game{};
	CCamera*       cam{};
	CamType        camType = CamType::Debug;
	CamRigType     rigType = CamRigType::Free;

public:
	CGameObject* Clone(INIT_DESC* pArg) override PURE;
	virtual void Free() override;
};

NS_END