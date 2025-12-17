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
	CCamObject(const CCamObject& rhs) : CGameObject(rhs), m_pLight(rhs.m_pLight), m_pGame(rhs.m_pGame) {}
	virtual ~CCamObject() DEFAULT;

public:
	HRESULT Initialize_Prototype()      override;
	HRESULT Initialize(INIT_DESC* pArg) override;

	void    Priority_Update(_float dt)  override PURE;
	void    Update(_float dt)           override PURE;
	void    Late_Update(_float dt)      override PURE;

protected:
	CGameInstance* m_pGame{};
	CCamera*       m_pCam{};
	CLight*        m_pLight{};
	CamType        m_eCamType = CamType::Debug;
	CamRigType     m_eRigType = CamRigType::Free;

public:
	CGameObject* Clone(INIT_DESC* pArg) override PURE;
	virtual void Free() override { __super::Free(); }
};
NS_END