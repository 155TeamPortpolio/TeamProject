#include "pch.h"
#include "StaticObject.h"
#include "StaticModel.h"
#include "Material.h"

#include "GameInstance.h"


CStaticObject::CStaticObject()
{
}

CStaticObject::CStaticObject(const CStaticObject& rhs)
	:CGameObject(rhs)
{
}

HRESULT CStaticObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();

	// 임시
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("Zero_Vehicle_Bus_01.model", "../../DemoResource/static/Zero_Vehicle_Bus_01.model");
	pRcsMgr->Add_ResourcePath("Zero_Vehicle_Bus_01.mat", "../../DemoResource/static/Zero_Vehicle_Bus_01.mat");
	
	return S_OK;
}

HRESULT CStaticObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

void CStaticObject::Awake()
{
	// 임시
	Get_Component<CStaticModel>()->Link_Model("MapTool_Level", "Zero_Vehicle_Bus_01.model");
	Get_Component<CMaterial>()->Link_Material("MapTool_Level", "Zero_Vehicle_Bus_01.mat");
	
}

void CStaticObject::Priority_Update(_float dt)
{
}

void CStaticObject::Update(_float dt)
{
}

void CStaticObject::Late_Update(_float dt)
{
}

void CStaticObject::Render_GUI()
{
	__super::Render_GUI();
}

CStaticObject* CStaticObject::Create()
{
	CStaticObject* instance = new CStaticObject();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CStaticObject");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CStaticObject::Clone(INIT_DESC* pArg)
{
	CStaticObject* instance = new CStaticObject(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CStaticObject");
		Safe_Release(instance);
	}

	return instance;
}

void CStaticObject::Free()
{
	__super::Free();
}