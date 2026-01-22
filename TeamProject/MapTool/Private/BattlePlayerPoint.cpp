#include "pch.h"
#include "BattlePlayerPoint.h"
#include "GameInstance.h"

#include "Collider.h"

CBattlePlayerPoint::CBattlePlayerPoint()
	: CBattleObject()
{
}

CBattlePlayerPoint::CBattlePlayerPoint(const CBattlePlayerPoint& rhs)
	: CBattleObject(rhs)
{
}

HRESULT CBattlePlayerPoint::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CBattlePlayerPoint::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pColliderCom = Get_Component<CCollider>();

	pColliderCom->Set_MapToolMode(true);
	pColliderCom->Set_ColliderColor({ 1.f, 0.f, 1.f, 1.f });

	return S_OK;
}

void CBattlePlayerPoint::Awake()
{
	__super::Awake();
}

void CBattlePlayerPoint::Priority_Update(_float dt)
{
}

void CBattlePlayerPoint::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CBattlePlayerPoint::Late_Update(_float dt)
{
}

void CBattlePlayerPoint::Export_ObjectData(void* pDesc)
{
	//ENTITY* pEntityDesc = static_cast<ENTITY*>(pDesc);
	//pEntityDesc->tagName = m_InstanceName;
	//_float3 vSize = Get_Component<CCollider>()->Get_Size();
	//_float4 qRotation; XMStoreFloat4(&qRotation, Get_Component<CTransform>()->Get_QuaternionRotate());
	//_vector3 vEulerRotation = _quaternion(qRotation).ToEuler();
	//_float3 vPosition; XMStoreFloat3(&vPosition, Get_Component<CTransform>()->Get_Pos());
	//pEntityDesc->vScale = { vSize.x, vSize.y, vSize.z };
	//pEntityDesc->vRotation = { vEulerRotation.x, vEulerRotation.y, vEulerRotation.z };
	//pEntityDesc->vTranslation = { vPosition.x, vPosition.y, vPosition.z };
}


void CBattlePlayerPoint::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();

	ImGui::InputText("##TriggerName", &m_InstanceName);
	ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Client Type");

	ImGui::PopID();
}

CBattlePlayerPoint* CBattlePlayerPoint::Create()
{
	CBattlePlayerPoint* instance = new CBattlePlayerPoint();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CBattlePlayerPoint");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CBattlePlayerPoint::Clone(INIT_DESC* pArg)
{
	CBattlePlayerPoint* instance = new CBattlePlayerPoint(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CBattlePlayerPoint");
		Safe_Release(instance);
	}

	return instance;
}

void CBattlePlayerPoint::Free()
{
	__super::Free();
}

