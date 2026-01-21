#include "pch.h"
#include "BattleMonsterPoint.h"
#include "GameInstance.h"

#include "Collider.h"

CBattleMonsterPoint::CBattleMonsterPoint()
	: CBattleObject()
{
}

CBattleMonsterPoint::CBattleMonsterPoint(const CBattleMonsterPoint& rhs)
	: CBattleObject(rhs)
{
}

HRESULT CBattleMonsterPoint::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CBattleMonsterPoint::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pColliderCom = Get_Component<CCollider>();

	pColliderCom->Set_MapToolMode(true);
	pColliderCom->Set_ColliderColor({ 0.f, 1.f, 1.f, 1.f });

	return S_OK;
}

void CBattleMonsterPoint::Awake()
{
	__super::Awake();
}

void CBattleMonsterPoint::Priority_Update(_float dt)
{
}

void CBattleMonsterPoint::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CBattleMonsterPoint::Late_Update(_float dt)
{
}

void CBattleMonsterPoint::Export_ObjectData(void* pDesc)
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


void CBattleMonsterPoint::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();

	ImGui::InputText("##TriggerName", &m_InstanceName);
	ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Client Type");

	ImGui::PopID();
}

CBattleMonsterPoint* CBattleMonsterPoint::Create()
{
	CBattleMonsterPoint* instance = new CBattleMonsterPoint();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CBattleMonsterPoint");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CBattleMonsterPoint::Clone(INIT_DESC* pArg)
{
	CBattleMonsterPoint* instance = new CBattleMonsterPoint(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CBattleMonsterPoint");
		Safe_Release(instance);
	}

	return instance;
}

void CBattleMonsterPoint::Free()
{
	__super::Free();
}

