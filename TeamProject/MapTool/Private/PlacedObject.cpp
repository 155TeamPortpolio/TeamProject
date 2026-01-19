#include "pch.h"
#include "PlacedObject.h"

#include "ModelData.h"
#include "StaticModel.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Collider.h"
#include "MaterialInstance.h"

#include "RayReceiver.h"

#include "GameInstance.h"


CPlacedObject::CPlacedObject()
	: CMapToolObject()
{
}

CPlacedObject::CPlacedObject(const CPlacedObject& rhs)
	: CMapToolObject(rhs)
{
}

HRESULT CPlacedObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CMaterial>();
	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CPlacedObject::Initialize(INIT_DESC* pArg)
{
#pragma region Model Type Check
	MAPTOOL_OBJECT_DESC* pObjDesc = static_cast<MAPTOOL_OBJECT_DESC*>(pArg);

	m_iObjectIndex = pObjDesc->iObjectIndex;
	m_TagModelKey = pObjDesc->TagModelKey;
	m_TagMaterialKey = pObjDesc->TagMaterialKey;

	if (true == m_TagModelKey.empty() ||
		true == m_TagMaterialKey.empty())
		return E_FAIL;

	CModelData* pData = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData(g_TagMapToolLevel, pObjDesc->TagModelKey);
	if (nullptr == pData)
		return E_FAIL;

	_bool isSkinned = pData->isSkinned();

	if (true == isSkinned) {
		Add_Component<CSkeletalModel>();
		Get_Component<CSkeletalModel>()->Link_Model(g_TagMapToolLevel, m_TagModelKey);
	}
	else {
		Add_Component<CStaticModel>();
		Get_Component<CStaticModel>()->Link_Model(g_TagMapToolLevel, m_TagModelKey);
	}

	Get_Component<CMaterial>()->Link_Material(g_TagMapToolLevel, m_TagMaterialKey);

#pragma endregion
	__super::Initialize(pArg);
	static _float4 start;

	auto pMaterialInstance = 	Get_Component<CMaterial>()->Get_MaterialInstance(0);
	//pMaterialInstance->Set_Param("UVOffset", { &start ,"float4",sizeof(_float4) });
	Get_Component<CCollider>()->Set_MapToolMode(true);


	return S_OK; 
}

void CPlacedObject::Awake()
{
	__super::Awake();
}

void CPlacedObject::Priority_Update(_float dt)
{
}

void CPlacedObject::Update(_float dt)
{
}

void CPlacedObject::Late_Update(_float dt)
{
}

void CPlacedObject::Export_ObjectData(void* pDesc)
{
	MapData_Object* pObjectDesc = static_cast<MapData_Object*>(pDesc);

	pObjectDesc->TagModelResourceKey = m_TagModelKey;
	pObjectDesc->TagMaterialResourceKey = m_TagMaterialKey;
	
	_float4x4 matWorld = Get_Component<CTransform>()->Get_WorldMatrix();

	pObjectDesc->vRight =	{ matWorld._11,matWorld._12, matWorld._13, matWorld._14 };
	pObjectDesc->vUp =		{ matWorld._21,matWorld._22, matWorld._23, matWorld._24 };
	pObjectDesc->vLook =	{ matWorld._31,matWorld._32, matWorld._33, matWorld._34 };
	pObjectDesc->vPos =		{ matWorld._41,matWorld._42, matWorld._43, matWorld._44 };
}

void CPlacedObject::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();

	string TagModelKey = "Model Key : " + m_TagModelKey;
	ImGui::Text(TagModelKey.c_str());

	ImGui::PopID();
}

CPlacedObject* CPlacedObject::Create()
{
	CPlacedObject* instance = new CPlacedObject();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CPlacedObject");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CPlacedObject::Clone(INIT_DESC* pArg)
{
	CPlacedObject* instance = new CPlacedObject(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CPlacedObject");
		Safe_Release(instance);
	}

	return instance;
}

void CPlacedObject::Free()
{
	__super::Free();
}