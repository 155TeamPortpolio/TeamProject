#include "pch.h"
#include "TriggerObject.h"
#include "GameInstance.h"

#include "Collider.h"



CTriggerObject::CTriggerObject()
	: CMapToolObject()
{
}

CTriggerObject::CTriggerObject(const CTriggerObject& rhs)
	: CMapToolObject(rhs)
{
}

HRESULT CTriggerObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CTriggerObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);


	//Get_Component<CCollider>()->Set_Rotation

	return S_OK;
}

void CTriggerObject::Awake()
{
	__super::Awake();
}

void CTriggerObject::Priority_Update(_float dt)
{
}

void CTriggerObject::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CTriggerObject::Late_Update(_float dt)
{
}

void CTriggerObject::Export_ObjectData(void* pDesc)
{
	MapData_Object* pObjectDesc = static_cast<MapData_Object*>(pDesc);

	pObjectDesc->TagModelResourceKey = m_InstanceName;
	switch (Get_Component<CCollider>()->Get_Type())
	{
	case Engine::COLLIDER_TYPE::BOX:
		pObjectDesc->TagMaterialResourceKey = "BOX";
		break;
	case Engine::COLLIDER_TYPE::SPHERE:
		pObjectDesc->TagMaterialResourceKey = "SPHERE";
		break;
	case Engine::COLLIDER_TYPE::CAPSULE:
		pObjectDesc->TagMaterialResourceKey = "CAPSULE";
		break;
	}

	COLLIDER_TYPE eType =  Get_Component<CCollider>()->Get_Type();
	_float3 vPosition; XMStoreFloat3(&vPosition, Get_Component<CTransform>()->Get_Pos());
	_float3 vCenter = Get_Component<CCollider>()->Get_Center();
	_float3 vSize = Get_Component<CCollider>()->Get_Size();
	_float3 vRotation = Get_Component<CCollider>()->Get_Rotation();
	pObjectDesc->vRight = { vCenter.x, vCenter.y, vCenter.z, 0.f};
	pObjectDesc->vUp = { vSize.x, vSize.y, vSize.z, 0.f };
	pObjectDesc->vLook = { vRotation.x, vRotation.y, vRotation.z, 0.f };
	pObjectDesc->vPos = { vPosition.x,vPosition.y, vPosition.z, 0.f };
	//_float4x4 matWorld = Get_Component<CTransform>()->Get_WorldMatrix();
	//pObjectDesc->vRight = { matWorld._11,matWorld._12, matWorld._13, matWorld._14 };
	//pObjectDesc->vUp = { matWorld._21,matWorld._22, matWorld._23, matWorld._24 };
	//pObjectDesc->vLook = { matWorld._31,matWorld._32, matWorld._33, matWorld._34 };
	//pObjectDesc->vPos = { matWorld._41,matWorld._42, matWorld._43, matWorld._44 };
}

void CTriggerObject::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();

	ImGui::InputText("##TriggerName", &m_InstanceName);

	string TagModelKey = "Trigger Key : " + m_InstanceName;
	ImGui::Text(TagModelKey.c_str());


	ImGui::PopID();
}

CTriggerObject* CTriggerObject::Create()
{
	CTriggerObject* instance = new CTriggerObject();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CTriggerObject");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CTriggerObject::Clone(INIT_DESC* pArg)
{
	CTriggerObject* instance = new CTriggerObject(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CTriggerObject");
		Safe_Release(instance);
	}

	return instance;
}

void CTriggerObject::Free()
{
	__super::Free();
}