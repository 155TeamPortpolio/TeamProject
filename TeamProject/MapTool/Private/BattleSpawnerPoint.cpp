#include "pch.h"
#include "BattleSpawnerPoint.h"
#include "GameInstance.h"

#include "Collider.h"

CBattleSpawnerPoint::CBattleSpawnerPoint()
	: CBattleObject()
{
}

CBattleSpawnerPoint::CBattleSpawnerPoint(const CBattleSpawnerPoint& rhs)
	: CBattleObject(rhs)
{
}

HRESULT CBattleSpawnerPoint::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CBattleSpawnerPoint::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pColliderCom = Get_Component<CCollider>();

	pColliderCom->Set_MapToolMode(true);
	pColliderCom->Set_ColliderColor({ 1.f, 1.f, 0.f, 1.f });

	m_eBattleType = BATTLE_TYPE::SPAWNER;

    m_SpawnMonsterIndices.clear();

	return S_OK;
}

void CBattleSpawnerPoint::Awake()
{
	__super::Awake();
}

void CBattleSpawnerPoint::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CBattleSpawnerPoint::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CBattleSpawnerPoint::Late_Update(_float dt)
{
}

void CBattleSpawnerPoint::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();

    ImGui::Text("");
    ImGui::SeparatorText("Include Monster Index");
    Render_GUI_MonsterIndices();
	
	ImGui::PopID();
}

void CBattleSpawnerPoint::Export_ObjectData(void* pDesc)
{
    BATTLE_POINT_SPAWNER_DATA* pData = static_cast<BATTLE_POINT_SPAWNER_DATA*>(pDesc);

    pData->iIndex = m_iIndex;
    pData->tagType = "SpawnerPoint";

    _float3 vSize = Get_Component<CCollider>()->Get_Size();
    _float4 qRotation; XMStoreFloat4(&qRotation, Get_Component<CTransform>()->Get_QuaternionRotate());
    _vector3 vEulerRotation = _quaternion(qRotation).ToEuler();
    _float3 vPosition; XMStoreFloat3(&vPosition, Get_Component<CTransform>()->Get_Pos());
    pData->vScale = { vSize.x, vSize.y, vSize.z };
    pData->vRotation = { vEulerRotation.x, vEulerRotation.y, vEulerRotation.z };
    pData->vTranslation = { vPosition.x, vPosition.y, vPosition.z };

    for (auto& index : m_SpawnMonsterIndices)
        pData->MonsterIndices.push_back(index);
}

void CBattleSpawnerPoint::Render_GUI_MonsterIndices()
{
   

    ImGui::Text("Count: %d", (int)m_SpawnMonsterIndices.size());
    ImGui::Separator();

    // --- Add 영역 ---
    ImGui::InputInt("Value", &m_InputValue);
    ImGui::SameLine();
    if (ImGui::Button("Add"))
        if (std::find(m_SpawnMonsterIndices.begin(), m_SpawnMonsterIndices.end(), m_InputValue) == m_SpawnMonsterIndices.end())
            m_SpawnMonsterIndices.push_back(m_InputValue);

    ImGui::SameLine();
    ImGui::Checkbox("Unique only", &m_isUniqueOnly);

    ImGui::Separator();

    // --- 삭제 버튼들 ---
    if (ImGui::Button("Clear All"))
    {
        m_SpawnMonsterIndices.clear();
        m_iSelectedIndex = -1;
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete Selected"))
    {
        if (0 <= m_iSelectedIndex && m_iSelectedIndex < (int)m_SpawnMonsterIndices.size())
        {
            m_SpawnMonsterIndices.erase(m_SpawnMonsterIndices.begin() + m_iSelectedIndex);
            m_iSelectedIndex = -1;
        }
    }

    ImGui::Separator();

    // --- 목록(확인/삭제) ---
    // 값이 많으면 clipper가 성능에 좋음
    ImGuiListClipper clipper;
    clipper.Begin((int)m_SpawnMonsterIndices.size());

    if (ImGui::BeginTable("IntTable", 3,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
    {
        ImGui::TableSetupColumn("No.", ImGuiTableColumnFlags_WidthFixed, 60.f);
        ImGui::TableSetupColumn("Index");
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 80.f);
        ImGui::TableHeadersRow();

        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; /* 수동 증가 */)
            {
                ImGui::PushID(i);

                ImGui::TableNextRow();

                // Index
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", i);

                // Value (클릭해서 선택)
                ImGui::TableSetColumnIndex(1);
                bool selected = (m_iSelectedIndex == i);
                char label[64];
                snprintf(label, sizeof(label), "%d", m_SpawnMonsterIndices[i]); // 표시용
                if (ImGui::Selectable(label, selected, ImGuiSelectableFlags_SpanAllColumns))
                    m_iSelectedIndex = i;

                // Action (개별 삭제)
                ImGui::TableSetColumnIndex(2);
                bool deleteThis = ImGui::SmallButton("Delete");

                ImGui::PopID();

                if (deleteThis)
                {
                    m_SpawnMonsterIndices.erase(m_SpawnMonsterIndices.begin() + i);

                    // 선택 인덱스 보정
                    if (m_iSelectedIndex == i) m_iSelectedIndex = -1;
                    else if (m_iSelectedIndex > i) m_iSelectedIndex--;

                    // erase 했으니 i 증가하지 말고 현재 i를 다시 검사
                    continue;
                }

                ++i;
            }
        }

        ImGui::EndTable();
    }
}

CBattleSpawnerPoint* CBattleSpawnerPoint::Create()
{
	CBattleSpawnerPoint* instance = new CBattleSpawnerPoint();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CBattleSpawnerPoint");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CBattleSpawnerPoint::Clone(INIT_DESC* pArg)
{
	CBattleSpawnerPoint* instance = new CBattleSpawnerPoint(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CBattleSpawnerPoint");
		Safe_Release(instance);
	}

	return instance;
}

void CBattleSpawnerPoint::Free()
{
	__super::Free();
}

