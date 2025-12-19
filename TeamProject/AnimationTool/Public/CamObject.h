#pragma once

#include "GameObject.h"
#include "Camera_Enum.h"
#include "Camera_Struct.h"

#define CAM   CGameInstance::GetInstance()->Get_CameraMgr()
#define GAME  CGameInstance::GetInstance()
#define OBJ   CGameInstance::GetInstance()->Get_ObjectMgr()

NS_BEGIN(Engine)
class CGameInstance; class CLight;
NS_END

NS_BEGIN(AnimTool)
class CCamObject abstract : public CGameObject
{
protected:
	CCamObject() : CGameObject() {}
	CCamObject(const CCamObject& rhs) : CGameObject(rhs) {}
	virtual ~CCamObject() DEFAULT;

public:
	HRESULT Initialize_Prototype()      override;
	HRESULT Initialize(INIT_DESC* pArg) override;
	void    Priority_Update(_float dt)  override PURE;
	void    Update(_float dt)           override PURE;
	void    Late_Update(_float dt)      override PURE;

protected:
	CamType    m_eCamType = CamType::Debug;
	CamRigType m_eRigType = CamRigType::Free;

public:
	CGameObject* Clone(INIT_DESC* pArg) override PURE;
	virtual void Free() override { __super::Free(); }
};
NS_END