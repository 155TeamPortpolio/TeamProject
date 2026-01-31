#include "pch.h"
#include "MonsterSpawnConsole.h"
#include "GameInstance.h"
#include "DataBase.h"
#include "BattleSystem.h"

CMonsterSpawnConsole::CMonsterSpawnConsole(GUI_CONTEXT* pContext)
	: CBasePanel(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
	, m_pBattleSystem(CBattleSystem::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pBattleSystem);
}

HRESULT CMonsterSpawnConsole::Initialize()
{
	if (FAILED(LoadMonsterTableData("../../Resources/Data/MonsterTable/MonsterTable.csv")))
		return E_FAIL;

	return S_OK;
}

void CMonsterSpawnConsole::Update_Panel(_float dt)
{
}

void CMonsterSpawnConsole::Render_GUI()
{
	//ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowPos(ImVec2(200, 50), ImGuiCond_FirstUseEver);
	//ImGui::Begin("Monster Spawn Console");

	// ---- 애니메이션 ----
	const _float deltaTime = ImGui::GetIO().DeltaTime;
	const _float speed = 10.0f;
	const _float targetTime = m_isOpen ? 1.0f : 0.0f;
	m_fOpentime = m_fOpentime + (targetTime - m_fOpentime) * (1.0f - expf(-speed * deltaTime));
	if (m_fOpentime < 0.001f) m_fOpentime = 0.0f;
	if (m_fOpentime > 0.999f) m_fOpentime = 1.0f;

	// ---- 레이아웃(우측 상단 고정) ----
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	const ImVec2 workPos = viewport->WorkPos;
	const ImVec2 workSize = viewport->WorkSize;

	const float buttonHeight = 24.0f;
	const float envButtonWidth = 80.0f;            // "ENV" 가독성용(높이는 buttonHeight)
	const float panelWidthClosed = envButtonWidth + 20.0f;
	const float panelWidthOpen = 300.0f;

	const float panelHeightClosed = buttonHeight + 20.0f;
	const float panelHeightOpen = 420.0f;

	const float panelWidth = panelWidthClosed + (panelWidthOpen - panelWidthClosed) * (float)m_fOpentime;
	const float panelHeight = panelHeightClosed + (panelHeightOpen - panelHeightClosed) * (float)m_fOpentime;

	const float marginRight = 12.0f;
	const float marginTop = 150.0f;

	const ImVec2 panelPos(
		workPos.x + workSize.x - panelWidth - marginRight,
		workPos.y + marginTop
	);
	const ImVec2 panelSize(panelWidth, panelHeight);

	ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(panelSize, ImGuiCond_Always);

	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoSavedSettings;

	// ---- 스타일 ----
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_isOpen ? 12.0f : 0, 10.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);

	ImVec4 bg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
	const float minBgAlpha = 0.02f;  // 닫힘 상태에서 박스 거의 안 보이게
	const float maxBgAlpha = 0.92f;
	bg.w = minBgAlpha + (maxBgAlpha - minBgAlpha) * (float)m_fOpentime;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, bg);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.08f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.14f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));

	ImGui::Begin("##MonsterSpawnConsole", nullptr, windowFlags);

	{
		const float contentWidth = ImGui::GetContentRegionAvail().x;
		ImVec2 envBtnSize(envButtonWidth, buttonHeight);
		ImGui::SetCursorPosX((contentWidth - envBtnSize.x) * 0.5f);

		if (ImGui::Button("Monster", envBtnSize))
			m_isOpen = !m_isOpen;
	}

	// ---- 열림 컨텐츠 ----
	if (m_fOpentime > 0.01f)
	{
		Render_GUI_Contents();
	}

	if (m_isOpen && m_fOpentime > 0.8f)
	{
		if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			m_isOpen = false;
		}
	}
	
	ImGui::End();

	ImGui::PopStyleColor(5); // WindowBg + Button(3) + Text
	ImGui::PopStyleVar(3);   // Rounding, Padding, FrameRounding
}

HRESULT CMonsterSpawnConsole::LoadMonsterTableData(const string& csvpath)
{
	/*
		column_count = read_header에 넣는 컬럼 수(헤더 개수)
		trim_chars = 앞 뒤 공백 제거
		double_quote_escape = "..." 안의 쉼표 및 따옴표 처리 */
	io::CSVReader<
		2,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvpath);

	/*
	헤더 이름으로 매핑(컬럼 순서 바뀌어도 무관)
	파일에 다른 컬럼이 더 있거나 누락된 컬럼이 있어도 무시함
	*/
	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"ProtoTag", "DisplayName"
	);

	string	ProtoTag{}, DisplayName{};

	while (in.read_row(
		ProtoTag, DisplayName
	))
	{
		if (ProtoTag.empty())
			continue;

		m_MonsterTags.emplace(DisplayName, ProtoTag);
	}

	return S_OK;
}

void CMonsterSpawnConsole::CheckCoolTime(_float dt)
{
	if (m_isSpawnFailed) {
		m_vSpawnFailedTime.y += dt;
		if (m_vSpawnFailedTime.x < m_vSpawnFailedTime.y) {
			m_vSpawnFailedTime.y = 0.f;
			m_isSpawnFailed = false;
		}
	}
}

void CMonsterSpawnConsole::Render_GUI_Contents()
{
	ImGui::PushID(this);

	RenderGuiMonsterSelect();

	RenderGuiSetValue();

	ImGui::Spacing();
	if (ImGui::Button("Spawn Monster"))
	{
		if (m_tagSelectedKey.empty() || m_tagSelectedProtoTag.empty())
			m_isSpawnFailed = true;

		BattleSystem()->SpawnMosnter(m_tagSelectedProtoTag, m_vSpawnPos);
	}

	if (true == m_isSpawnFailed)
	{
		ImGui::SameLine(0.f, 20.f);
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Faild Spawn Monster");
	}




	ImGui::PopID();
}

void CMonsterSpawnConsole::RenderGuiMonsterSelect()
{
	if (m_MonsterTags.empty())
	{
		ImGui::TextDisabled("Empty");
		return;
	}

	if (m_MonsterKeys.empty())
	{
		m_MonsterKeys.clear();
		m_MonsterKeys.reserve(m_MonsterTags.size());
		for (auto& Pair : m_MonsterTags)
			m_MonsterKeys.push_back(Pair.first);
		sort(m_MonsterKeys.begin(), m_MonsterKeys.end());
		if (m_tagSelectedKey.empty())
			m_tagSelectedKey = m_MonsterKeys[0];
	}

	ImGui::SeparatorText("Select Mosnter");

	const _char* preview = m_tagSelectedKey.empty() ? m_MonsterKeys[0].c_str() : m_tagSelectedKey.c_str();
	if (ImGui::BeginCombo("##MonsterSpawnConsoleKey", preview))
	{
		for (auto& Key : m_MonsterKeys)
		{
			_bool isSelected = (Key == m_tagSelectedKey);
			if (ImGui::Selectable(Key.c_str(), isSelected))
				m_tagSelectedKey = Key;
			
			if (isSelected) 
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (auto iter = m_MonsterTags.find(m_tagSelectedKey); iter != m_MonsterTags.end())
		m_tagSelectedProtoTag = iter->second;

	//ImGui::Text("Select Monster : %s", m_tagSelectedKey);
}

void CMonsterSpawnConsole::RenderGuiSetValue()
{
	ImGui::SeparatorText("Set Spawn Pos");

	float v[3] = { m_vSpawnPos.x, m_vSpawnPos.y, m_vSpawnPos.z };

	if (ImGui::DragFloat3("##MonsterSpawnConsoleSetSpawnPos", v, 0.1f))  // speed = 0.1
	{
		m_vSpawnPos.x = v[0];
		if (0.f > v[1])
			v[1] = 0.f;
		m_vSpawnPos.y = v[1];
		m_vSpawnPos.z = v[2];
	}

	_bool isUseInspector = m_pBattleSystem->IsUseInspector();

	if (ImGui::Checkbox("UseInspector", &isUseInspector))
	{
		m_pBattleSystem->SetUseInspector(isUseInspector);
	}

}

void CMonsterSpawnConsole::RenderGuiSpawnMonster()
{
	

}

CMonsterSpawnConsole* CMonsterSpawnConsole::Create(GUI_CONTEXT* pContext)
{
	CMonsterSpawnConsole* instance = new CMonsterSpawnConsole(pContext);

	if (FAILED(instance->Initialize()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CMonsterSpawnConsole");
	}

	return instance;
}

void CMonsterSpawnConsole::Free()
{
	__super::Free();

	Safe_Release(m_pBattleSystem);
	Safe_Release(m_pGameInstance);
}
