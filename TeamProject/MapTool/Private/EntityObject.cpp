#include "pch.h"
#include "EntityObject.h"
#include "GameInstance.h"

#include "Collider.h"

CEntityObject::CEntityObject()
	: CMapToolObject()
{
}

CEntityObject::CEntityObject(const CEntityObject& rhs)
	: CMapToolObject(rhs)
{
}

HRESULT CEntityObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CEntityObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	Get_Component<CCollider>()->Set_MapToolMode(true);

	return S_OK;
}

void CEntityObject::Awake()
{
	__super::Awake();
}

void CEntityObject::Priority_Update(_float dt)
{
}

void CEntityObject::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CEntityObject::Late_Update(_float dt)
{
}

void CEntityObject::Export_ObjectData(void* pDesc)
{
	ENTITY* pEntityDesc = static_cast<ENTITY*>(pDesc);

	pEntityDesc->tagProto = m_InstanceName;
	pEntityDesc->iType = m_iType;

	_float3 vSize = Get_Component<CCollider>()->Get_Size();
	_float4 qRotation; XMStoreFloat4(&qRotation, Get_Component<CTransform>()->Get_QuaternionRotate());
	_vector3 vEulerRotation = _quaternion(vEulerRotation).ToEuler();
	_float3 vPosition; XMStoreFloat3(&vPosition, Get_Component<CTransform>()->Get_Pos());
	pEntityDesc->vScale = { vSize.x, vSize.y, vSize.z };
	pEntityDesc->vRotation = { vEulerRotation.x, vEulerRotation.y, vEulerRotation.z };
	pEntityDesc->vTranslation = { vPosition.x, vPosition.y, vPosition.z };
}


void CEntityObject::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();
	
	ImGui::InputText("##TriggerName", &m_InstanceName);
	ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Client Type");
	ImGui::InputInt("##Version", &m_iType);

	ImGui::PopID();
}

CEntityObject* CEntityObject::Create()
{
	CEntityObject* instance = new CEntityObject();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CEntityObject");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CEntityObject::Clone(INIT_DESC* pArg)
{
	CEntityObject* instance = new CEntityObject(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CEntityObject");
		Safe_Release(instance);
	}

	return instance;
}

void CEntityObject::Free()
{
	__super::Free();
}

