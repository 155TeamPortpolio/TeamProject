#include "pch.h"
#include "BattleObject.h"
#include "GameInstance.h"

#include "Collider.h"

CBattleObject::CBattleObject()
	: CMapToolObject()
{
}

CBattleObject::CBattleObject(const CBattleObject& rhs)
	: CMapToolObject(rhs)
{
}

HRESULT CBattleObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CBattleObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	Get_Component<CCollider>()->Set_MapToolMode(true);

	BATTLE_INIT_DESC* pDesc = static_cast<BATTLE_INIT_DESC*>(pArg);
	m_iIndex = pDesc->iIndex;
	m_isDirtyName = true;

	return S_OK;
}

void CBattleObject::Awake()
{
	__super::Awake();
}

void CBattleObject::Priority_Update(_float dt)
{
	if (m_isDirtyName) 
	{
		switch (m_eBattleType)
		{
		case MapTool::BATTLE_TYPE::PLAYER:
			m_InstanceName = "Player";
			break;
		case MapTool::BATTLE_TYPE::SPAWNER:
			m_InstanceName = "Spawner" + to_string(m_iIndex);
			break;
		case MapTool::BATTLE_TYPE::MONSTER:
			m_InstanceName = "Monster" + to_string(m_iIndex);
			break;
		case MapTool::BATTLE_TYPE::ENDPOINT:
			m_InstanceName = "EndPoint" + to_string(m_iIndex);
			break;
		}

		m_isDirtyName = false;
	}

}

void CBattleObject::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CBattleObject::Late_Update(_float dt)
{
}

void CBattleObject::Export_ObjectData(void* pDesc)
{
	BATTLE_POINT_DATA* pData = static_cast<BATTLE_POINT_DATA*>(pDesc);

	pData->iIndex = m_iIndex;
	switch (m_eBattleType)
	{
	case MapTool::BATTLE_TYPE::PLAYER:
		pData->tagType = "PlayerPoint";
		break;
	case MapTool::BATTLE_TYPE::MONSTER:
		pData->tagType = "MonsterPoint";
		break;
	case MapTool::BATTLE_TYPE::ENDPOINT:
		pData->tagType = "EndPoint";
		break;
	}
	_float3 vSize = Get_Component<CCollider>()->Get_Size();
	_float4 qRotation; XMStoreFloat4(&qRotation, Get_Component<CTransform>()->Get_QuaternionRotate());
	_vector3 vEulerRotation = _quaternion(qRotation).ToEuler();
	_float3 vPosition; XMStoreFloat3(&vPosition, Get_Component<CTransform>()->Get_Pos());
	pData->vScale = { vSize.x, vSize.y, vSize.z };
	pData->vRotation = { vEulerRotation.x, vEulerRotation.y, vEulerRotation.z };
	pData->vTranslation = { vPosition.x, vPosition.y, vPosition.z };
}

void CBattleObject::Render_GUI()
{
	ImGui::PushID(this);

	if (ImGui::TreeNode("Inspector"))
	{
		__super::Render_GUI();
		ImGui::TreePop();
	}

	ImGui::Separator();
	switch (m_eBattleType)
	{
	case MapTool::BATTLE_TYPE::PLAYER:
		ImGui::Text("Battle Type : Player");
		break;
	case MapTool::BATTLE_TYPE::SPAWNER:
		ImGui::Text("Battle Type : Spawner");
		break;
	case MapTool::BATTLE_TYPE::MONSTER:
		ImGui::Text("Battle Type : Monster");
		break;
	case MapTool::BATTLE_TYPE::ENDPOINT:
		ImGui::Text("Battle Type : EndPoint");
		break;
	}
	ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Index");
	if (ImGui::InputInt("##Version", &m_iIndex))
		m_isDirtyName = true;

	ImGui::PopID();
}

void CBattleObject::Free()
{
	__super::Free();
}
