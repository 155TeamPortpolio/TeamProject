#include "pch.h"
#include "MapTriggerObject.h"
#include "GameInstance.h"

#include "Collider.h"



CMapTriggerObject::CMapTriggerObject()
	: CMapObject()
{
}

CMapTriggerObject::CMapTriggerObject(const CMapTriggerObject& rhs)
	: CMapObject(rhs)
{
}

HRESULT CMapTriggerObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CMapTriggerObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);


	//Get_Component<CCollider>()->Set_Rotation

	return S_OK;
}

void CMapTriggerObject::Awake()
{
	__super::Awake();
}

void CMapTriggerObject::Priority_Update(_float dt)
{
}

void CMapTriggerObject::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CMapTriggerObject::Late_Update(_float dt)
{
}

void CMapTriggerObject::OnTriggerEnter()
{
	//MSG_BOX("ºÎµúÈû");
}

void CMapTriggerObject::OnTriggerExit()
{
}

void CMapTriggerObject::OnCollisionEnter()
{
	//MSG_BOX("ÄÝ¸®Á¯µé¾î¿È");
}

void CMapTriggerObject::Export_ObjectData(void* pDesc)
{
	MapData_Object* pObjectDesc = static_cast<MapData_Object*>(pDesc);

	pObjectDesc->TagModelResourceKey = m_InstanceName;
	pObjectDesc->TagMaterialResourceKey = "";

	_float4x4 matWorld = Get_Component<CTransform>()->Get_WorldMatrix();

	pObjectDesc->vRight = { matWorld._11,matWorld._12, matWorld._13, matWorld._14 };
	pObjectDesc->vUp = { matWorld._21,matWorld._22, matWorld._23, matWorld._24 };
	pObjectDesc->vLook = { matWorld._31,matWorld._32, matWorld._33, matWorld._34 };
	pObjectDesc->vPos = { matWorld._41,matWorld._42, matWorld._43, matWorld._44 };
}

void CMapTriggerObject::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();

	string TagModelKey = "Trigger Key : " + m_InstanceName;
	ImGui::Text(TagModelKey.c_str());


	ImGui::PopID();
}

CMapTriggerObject* CMapTriggerObject::Create()
{
	CMapTriggerObject* instance = new CMapTriggerObject();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CMapTriggerObject");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CMapTriggerObject::Clone(INIT_DESC* pArg)
{
	CMapTriggerObject* instance = new CMapTriggerObject(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CMapTriggerObject");
		Safe_Release(instance);
	}

	return instance;
}

void CMapTriggerObject::Free()
{
	__super::Free();
}