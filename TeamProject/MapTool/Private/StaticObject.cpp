#include "pch.h"
#include "StaticObject.h"

#include "ModelData.h"
#include "StaticModel.h"
#include "SkeletalModel.h"
#include "Material.h"

#include "RayReceiver.h"

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
	//Add_Component<CStaticModel>();
	Add_Component<CMaterial>();
	Add_Component<CRayReceiver>();

	// 임시
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("Zero_Vehicle_Bus_01.model", "../../DemoResource/static/Zero_Vehicle_Bus_01.model");
	pRcsMgr->Add_ResourcePath("Zero_Vehicle_Bus_01.mat", "../../DemoResource/static/Zero_Vehicle_Bus_01.mat");

	return S_OK;
}

HRESULT CStaticObject::Initialize(INIT_DESC* pArg)
{
#pragma region Model Type Check
	STATIC_OBJECT_DESC* pObjDesc = static_cast<STATIC_OBJECT_DESC*>(pArg);
	
	m_TagModelKey = pObjDesc->TagModelKey;
	m_TagMaterialKey = pObjDesc->TagMaterialKey;

	if (true == m_TagModelKey.empty() ||
		true == m_TagMaterialKey.empty())
		return E_FAIL;

	CModelData* pData = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData("MapTool_Level", pObjDesc->TagModelKey);
	if (nullptr == pData)
		return E_FAIL;
	
	_bool isSkinned = pData->isSkinned();

	if (true == isSkinned)
		Add_Component<CSkeletalModel>();
	else
		Add_Component<CStaticModel>();

#pragma endregion
	__super::Initialize(pArg);

	Get_Component<CRayReceiver>()->Set_CompActive(pObjDesc->isRayReceiver);

	if (true == isSkinned)
		Get_Component<CSkeletalModel>()->Link_Model("MapTool_Level", m_TagModelKey);
	else
		Get_Component<CStaticModel>()->Link_Model("MapTool_Level", m_TagModelKey);

	Get_Component<CMaterial>()->Link_Material("MapTool_Level", m_TagMaterialKey);
	
	// 임시
	//Get_Component<CStaticModel>()->Link_Model("MapTool_Level", "Zero_Vehicle_Bus_01.model");
	//Get_Component<CMaterial>()->Link_Material("MapTool_Level", "Zero_Vehicle_Bus_01.mat");

	return S_OK;
}

void CStaticObject::Awake()
{
	// (!!포인터 접근해서 값 변경) 만들어 지면 무조건 Inspector창에 띄움.
	CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = this;
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
	ImGui::PushID(this);

	__super::Render_GUI();

	if (ImGui::Button("Delete")) {
		Delete_Object();
	}

	ImGui::PopID();
}

void CStaticObject::Delete_Object()
{
	CGameInstance::GetInstance()->Get_ObjectMgr()->Remove_Object(this);
	CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = nullptr;
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