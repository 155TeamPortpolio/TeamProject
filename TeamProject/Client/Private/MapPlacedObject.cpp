#include "pch.h"
#include "MapPlacedObject.h"
#include "MapData_Defines.h"

#include "ModelData.h"
#include "StaticModel.h"
#include "SkeletalModel.h"
#include "Material.h"

//#include "RayReceiver.h"

#include "GameInstance.h"


CMapPlacedObject::CMapPlacedObject()
	: CMapObject()
{
}

CMapPlacedObject::CMapPlacedObject(const CMapPlacedObject& rhs)
	: CMapObject(rhs)
{
}

HRESULT CMapPlacedObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CMapPlacedObject::Initialize(INIT_DESC* pArg)
{
#pragma region Model Type Check
	MAPOBJ_DESC* pObjDesc = static_cast<MAPOBJ_DESC*>(pArg);

	m_TagModelKey = pObjDesc->TagModelKey;
	m_TagMaterialKey = pObjDesc->TagMaterialKey;

	if (true == m_TagModelKey.empty() ||
		true == m_TagMaterialKey.empty())
		return E_FAIL;

	if (true == pObjDesc->bCooking)
		Add_Component<CCollider>();

	CModelData* pData = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData(pObjDesc->TagLevel, pObjDesc->TagModelKey);
	if (nullptr == pData)
		return E_FAIL;

	_bool isSkinned = pData->isSkinned();

	if (true == isSkinned) {
		Add_Component<CSkeletalModel>();
		Get_Component<CSkeletalModel>()->Link_Model(pObjDesc->TagLevel, m_TagModelKey);
	}
	else {
		Add_Component<CStaticModel>();
		Get_Component<CStaticModel>()->Link_Model(pObjDesc->TagLevel, m_TagModelKey);
	}

	Get_Component<CMaterial>()->Link_Material(pObjDesc->TagLevel, m_TagMaterialKey);
#pragma endregion
	__super::Initialize(pArg);

	auto iter = pObjDesc->SlotDataValues.find("Effect");
	if (iter != pObjDesc->SlotDataValues.end()) {
		
		
			
		for (auto& slotvalue : iter->second) {
			//slotvalue.defaultvalue
		}
	}

	return S_OK;
}

void CMapPlacedObject::Awake()
{
}

void CMapPlacedObject::Priority_Update(_float dt)
{
}

void CMapPlacedObject::Update(_float dt)
{
}

void CMapPlacedObject::Late_Update(_float dt)
{
 }

void CMapPlacedObject::Export_ObjectData(void* pDesc)
{
	MapData_Object* pObjectDesc = static_cast<MapData_Object*>(pDesc);

	pObjectDesc->TagModelResourceKey = m_TagModelKey;
	pObjectDesc->TagMaterialResourceKey = m_TagMaterialKey;

	_float4x4 matWorld = Get_Component<CTransform>()->Get_WorldMatrix();

	pObjectDesc->vRight = { matWorld._11,matWorld._12, matWorld._13, matWorld._14 };
	pObjectDesc->vUp = { matWorld._21,matWorld._22, matWorld._23, matWorld._24 };
	pObjectDesc->vLook = { matWorld._31,matWorld._32, matWorld._33, matWorld._34 };
	pObjectDesc->vPos = { matWorld._41,matWorld._42, matWorld._43, matWorld._44 };
}

void CMapPlacedObject::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();

	ImGui::PopID();
}

CMapPlacedObject* CMapPlacedObject::Create()
{
	CMapPlacedObject* instance = new CMapPlacedObject();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CMapPlacedObject");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CMapPlacedObject::Clone(INIT_DESC* pArg)
{
	CMapPlacedObject* instance = new CMapPlacedObject(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CMapPlacedObject");
		Safe_Release(instance);
	}

	return instance;
}

void CMapPlacedObject::Free()
{
	__super::Free();
}