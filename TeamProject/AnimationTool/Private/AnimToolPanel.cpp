#include "AnimToolPanel.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "Animator3DEX.h"
#include "AnimationClip.h"
#include "AnimModel.h"
#include "AnimationLayout.h"
#include "Channel.h"


CAnimToolPanel::CAnimToolPanel(GUI_CONTEXT* pContext)
	: CBasePanel{pContext}
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

void CAnimToolPanel::Update_Panel(_float dt)
{
	CGameObject* CurSelected = m_pGameInstance->Get_GUISystem()->Get_Context()->pSelectedObject;

	if (m_pSelectModel != CurSelected) {
		m_pSelectModel = CurSelected;
		dynamic_cast<CAnimModel*>(m_pSelectModel)->Set_Panel(this);
		Reset_Panel();
	}
	
	if (nullptr != m_pSelectAnimator) {
		float fPause = 1.f;
		if (m_bPause) fPause = 0.f;

	
		if (m_ePanelType == PANELTYPE::CLIP) {
			m_pSelectAnimator->Update_Animation(dt * fPause * m_fPlaySpeed);
		}
		else if (m_ePanelType == PANELTYPE::PREVIEW) {
			if (m_bPreviewPlay) {
				m_pSelectAnimator->Update_Animation(dt * fPause * m_fPlaySpeed);

				if (m_pSelectAnimator->isCurrentAnimEnd()) {
					if (m_iCurrentPrevIndex < m_PreviewList.size() - 1) {
						m_fTrackPos = 0.f;
						m_pSelectAnimator->Set_Animation(m_PreviewList[++m_iCurrentPrevIndex])
							.Loop(false);
					}
					else
						m_bPreviewPlay = false;
				}
			}
		}
	}

	if (m_pGameInstance->Get_InputDev()->Key_Tap('P')) {
		if (nullptr != m_pSelectAnimator) {
			m_pSelectAnimator->Resize_Layer(2);
			m_pSelectAnimator->Set_StartBone(23, 1);
			m_pSelectAnimator->Set_Animation(1, 5).
				Loop(true);
		}
	}
}

void CAnimToolPanel::Render_GUI()
{
	GUI_DefaultSetting();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

	ImGui::Begin("AnimTool", nullptr, flags);

	if (ImGui::BeginTabBar("##ToolTabs"))
	{
		const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
		const float childHeight = (textLineHeight + 2) + (ImGui::GetStyle().WindowPadding.y * 2);

		// 현재 프레임에서 활성화된 패널
		PANELTYPE CurPanelType = m_ePanelType;

		if (ImGui::BeginTabItem("Setting Clip"))
		{
			CurPanelType = PANELTYPE::CLIP;
			GUI_Setting_Clips(childHeight);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Preview Animation"))
		{
			CurPanelType = PANELTYPE::PREVIEW;
			GUI_Preview(childHeight);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Create Meta"))
		{
			CurPanelType = PANELTYPE::RESOURCE;
			GUI_Create_MetaData(childHeight);
			ImGui::EndTabItem();
		}

		if (CurPanelType != m_ePanelType)
		{
			m_bPause = true;
			m_ePanelType = CurPanelType;
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void CAnimToolPanel::GUI_DefaultSetting()
{
	constexpr float defaultHeight = 200.f;
	constexpr float leftX = 200.f;
	constexpr float rightMargin = 275.f;

	const ImGuiViewport* vp = ImGui::GetMainViewport();
	const ImVec2 workPos = vp->WorkPos;
	const ImVec2 workSize = vp->WorkSize;

	ImVec2 bottomLeft(workPos.x + leftX, workPos.y + workSize.y);
	bottomLeft.x = floorf(bottomLeft.x);
	bottomLeft.y = floorf(bottomLeft.y);

	float width = workSize.x - leftX - rightMargin;
	width = floorf(width);

	ImGui::SetNextWindowPos(bottomLeft, ImGuiCond_Always, ImVec2(0.f, 1.f));
	ImGui::SetNextWindowSize(ImVec2(width, defaultHeight), ImGuiCond_FirstUseEver);

	Helper::DarkThemeStyle styleScope;
}

void CAnimToolPanel::GUI_Setting_Clips(_float fChildHeight)
{
	ImGui::Text("ClipTag : "); ImGui::SameLine();
	ImGui::SetNextItemWidth(300.f);
	if (ImGui::BeginCombo("##Model Combo", m_CurClipTag.c_str())) //Model
	{
		if (!m_AnimClip.empty()) {
			int iIndex = 0;
			for (auto& Clip : m_AnimClip)
			{
				string ClipTag = Clip.ClipTag;
				bool selected = (m_CurClipTag == ClipTag);
				if (ImGui::Selectable(ClipTag.c_str(), selected))
				{
					//새로운 클립을 눌렀다면
					m_CurClipTag = ClipTag;
					m_iCurClipIndex = iIndex;
					m_pSelectAnimator->Set_Animation(0, iIndex)
						.Loop(m_bLoop);
					
					//디버그용 레이어에 데이터넣기
					auto& Clip = (*m_pSelectAnimator->Get_Clips())[iIndex];
					m_fTrackPos = 0.f;					
					m_fTickPerSec = Clip->Get_TickPerSec();
					m_fDuration = Clip->Get_Duration();
				}
				if (selected)
					ImGui::SetItemDefaultFocus();

				iIndex++;
			}
		}
		ImGui::EndCombo();
	}

	//Set Layer
	ImGui::SameLine();
	ImGui::Text("	Layer : "); ImGui::SameLine();

	static int LayerIndex = 0;
	int LayerCount = m_pSelectAnimator ? m_pSelectAnimator->Get_NumLayer() : 0;

	char preview[16];
	sprintf_s(preview, "%d", LayerIndex);
	ImGui::PushItemWidth(50.f);
	if (ImGui::BeginCombo("##LayerCombo", preview))
	{
		for (int i = 0; i < LayerCount; ++i)
		{
			bool selected = (LayerIndex == i);
			char label[16];
			sprintf_s(label, "%d", i);

			if (ImGui::Selectable(label, selected))
				LayerIndex = i;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	//Start Bone
	ImGui::SameLine();
	ImGui::Text("	Start Bone : "); ImGui::SameLine();
	static int StartBoneIndex = -1;
	ImGui::PushItemWidth(50.f);
	ImGui::InputInt("##StartBone", &StartBoneIndex, 0, 0);
	ImGui::SameLine();
	if (ImGui::Button("Set##StartBone", { 55.f, 0.f }))
	{
		if (nullptr != m_pSelectAnimator) m_pSelectAnimator->Set_StartBone(StartBoneIndex);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset##StartBone", { 55.f, 0.f }))
	{
		if (nullptr != m_pSelectAnimator) m_pSelectAnimator->Reset_StartBone();
	}

	//Extract Bone
	ImGui::SameLine();
	ImGui::Text("	Extrack Bone : "); ImGui::SameLine();
	static int RootBoneIndex = -1;
	ImGui::PushItemWidth(50.f);
	ImGui::InputInt("##ExtractBone", &RootBoneIndex, 0, 0);
	ImGui::SameLine();
	if (ImGui::Button("Set##ExtractBone", { 55.f, 0.f }))
	{
		if (nullptr != m_pSelectAnimator)
			m_pSelectAnimator->Set_ExtractBoneMovement(RootBoneIndex, false, true, false);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset##ExtractBone", { 55.f, 0.f }))
	{
		if (nullptr != m_pSelectAnimator) m_pSelectAnimator->Reset_ExtractBoneMovement();
	}

	Draw_ToolbarUI();

	Draw_EventListUI();
}

void CAnimToolPanel::Draw_ToolbarUI()
{
	const ImVec2 buttonSize(55.f, 0.f);
	static float timeScale = 1.f;

	/* 버튼 */

	if (ImGui::Button(m_bPause ? "Play" : "Pause", buttonSize)) {
		m_bPause = !m_bPause;
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop", buttonSize))
	{
		m_bPause = true;
		m_fTrackPos = 0.f;
		if (m_pSelectAnimator) m_pSelectAnimator->Get_AnimLayers()[0].fCurrentTrackPosition = 0.f;
	}
	ImGui::SameLine();
	if (ImGui::Button("Snap", buttonSize))
	{
		Add_Event();
	}

	ImGui::SameLine();
	ImGui::Checkbox("Loop", &m_bLoop);

	ImGui::SameLine();
	ImGui::Dummy(ImVec2(10.f, 0.f));
	ImGui::SameLine();

	/* 재생속도 버튼 */
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(u8"속도");
	ImGui::SameLine();

	ImGui::PushID("AnimSpeedUI");

	auto SpeedBtn = [&](const char* label, float v)
		{
			const bool active = fabsf(m_fPlaySpeed - v) < 1e-6f;
			if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
			if (ImGui::SmallButton(label)) m_fPlaySpeed = v;
			if (active) ImGui::PopStyleColor();
		};

	SpeedBtn(u8"x0.25", 0.25f); ImGui::SameLine();
	SpeedBtn(u8"x0.5", 0.5f);  ImGui::SameLine();
	SpeedBtn(u8"x1", 1.0f);  ImGui::SameLine();
	SpeedBtn(u8"x2", 2.0f);  ImGui::SameLine();
	SpeedBtn(u8"x4", 4.0f);

	ImGui::SameLine();
	ImGui::SetNextItemWidth(90.f);
	ImGui::DragFloat("##scale", &m_fPlaySpeed, 0.01f, 0.05f, 8.0f, "x%.2f");

	ImGui::PopID();

	ImGui::SameLine();


	//재생 바 UI
	if (m_pSelectAnimator)
		m_fTrackPos = m_pSelectAnimator->Get_AnimLayers()[0].fCurrentTrackPosition;

	Draw_TimelineUI(m_fDuration, m_fTrackPos, "##AnimTimeline");
	
	if (m_pSelectAnimator)
		m_pSelectAnimator->Get_AnimLayers()[0].fCurrentTrackPosition = m_fTrackPos;

	//저장버튼
	ImGui::SameLine();
	if (ImGui::Button("Save", buttonSize))
	{
		Save_Event();
	}
}

void CAnimToolPanel::Draw_TimelineUI(float duration, float& ioTime, const char* id)
{
	ImVec2 avail = ImGui::GetContentRegionAvail();
	float barH = ImGui::GetFrameHeight();

	if (avail.x < 140.f) return;

	float endT = (duration > 1e-6f) ? duration : 1.f;

	ImVec2 barPos = ImGui::GetCursorScreenPos();
	ImVec2 barSize(avail.x - 80.f, barH); //재생바 크기

	ImGui::InvisibleButton(id, barSize);

	ImDrawList* dl = ImGui::GetWindowDrawList();

	ImU32 colBg = ImGui::GetColorU32(ImGuiCol_FrameBg);
	ImU32 colBorder = ImGui::GetColorU32(ImGuiCol_Border);
	ImU32 colFill = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	ImU32 colTick = ImGui::GetColorU32(ImGuiCol_TextDisabled);
	ImU32 colCursor = ImGui::GetColorU32(ImGuiCol_Text);
	ImU32 colText = ImGui::GetColorU32(ImGuiCol_TextDisabled);
	ImU32 colHot = ImGui::GetColorU32(ImGuiCol_ButtonActive);

	dl->AddRectFilled(barPos, ImVec2(barPos.x + barSize.x, barPos.y + barSize.y), colBg, 4.f);
	dl->AddRect(barPos, ImVec2(barPos.x + barSize.x, barPos.y + barSize.y), colBorder, 4.f);

	const int gridN = 8;
	for (int i = 1; i < gridN; ++i)
	{
		float t = (float)i / (float)gridN;
		float x = barPos.x + barSize.x * t;
		dl->AddLine(ImVec2(x, barPos.y + 2.f), ImVec2(x, barPos.y + barSize.y - 2.f), colTick, 1.f);
	}

	float t01 = clamp(ioTime / endT, 0.f, 1.f);
	dl->AddRectFilled(barPos, ImVec2(barPos.x + barSize.x * t01, barPos.y + barSize.y), colFill, 4.f);

	float cx = barPos.x + barSize.x * t01;
	dl->AddLine(ImVec2(cx, barPos.y - 2.f), ImVec2(cx, barPos.y + barSize.y + 2.f), colCursor, 2.0f);

	//클립마다 타임라인에 막대기 보이도록
	if (!m_AnimClip.empty() && -1 != m_iCurClipIndex) {
		for (auto& Event : m_AnimClip[m_iCurClipIndex].Events) {
			float t = clamp(Event.EventTime / endT, 0.f, 1.f);
			float x = barPos.x + barSize.x * t;
			dl->AddLine(ImVec2(x, barPos.y + 2.f), ImVec2(x, barPos.y + barSize.y - 2.f), GetEventColor(Event.EventType), 3.f);
		}
	}

	ImVec2 tri0(cx, barPos.y + barSize.y + 1.f);
	ImVec2 tri1(cx - 5.f, barPos.y + barSize.y + 9.f);
	ImVec2 tri2(cx + 5.f, barPos.y + barSize.y + 9.f);
	dl->AddTriangleFilled(tri0, tri1, tri2, colCursor);

	char buf[64];
	sprintf_s(buf, "%.2fs / %.2fs", ioTime, duration);

	ImVec2 textSize = ImGui::CalcTextSize(buf);
	ImVec2 textPos(barPos.x + 8.f, barPos.y + (barSize.y - textSize.y) * 0.5f);
	dl->AddText(textPos, colText, buf);

	const bool hovered = ImGui::IsItemHovered();
	if (hovered)
	{
		float mx = ImGui::GetIO().MousePos.x;
		float local01 = clamp((mx - barPos.x) / barSize.x, 0.f, 1.f);
		float hoverTime = local01 * endT;

		ImGui::BeginTooltip();
		ImGui::Text("t = %.2fs", hoverTime);
		ImGui::EndTooltip();

		dl->AddLine(ImVec2(barPos.x + barSize.x * local01, barPos.y), ImVec2(barPos.x + barSize.x * local01, barPos.y + barSize.y), colHot, 1.5f);
	}

	if (ImGui::IsItemActive())
	{
		float mx = ImGui::GetIO().MousePos.x;
		float local01 = clamp((mx - barPos.x) / barSize.x, 0.f, 1.f);
		ioTime = local01 * endT;
	}
}

void CAnimToolPanel::Draw_EventListUI()
{
	ImGui::BeginTable("##EventTable", 4,
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_ScrollY |
		ImGuiTableFlags_BordersInnerV);

	ImGui::TableSetupColumn("TrackPos", ImGuiTableColumnFlags_WidthFixed, 70.f);
	ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 90.f);
	ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthStretch);
	ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed, 40.f);
	ImGui::TableHeadersRow();
	
	//클립이 없거나 선택을 하지 않으면 렌더하지 않음
	if (!m_AnimClip.empty() && -1 != m_iCurClipIndex) {
		auto& Events = m_AnimClip[m_iCurClipIndex].Events;
		for (size_t i = 0; i < Events.size(); ++i)
		{
			ANIM_EVENT& e = Events[i];
			ImGui::PushID((int)i);

			ImGui::TableNextRow();

			// Time
			ImGui::TableNextColumn();
			ImGui::DragFloat("##TrackPosition", &e.EventTime, 0.01f, 0.f, m_fDuration, "%.2f");

			// Type
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(80.f);   // ← 여기서 폭 조절
			int type = (int)e.EventType;
			ImGui::Combo("##EventType", &type, "Notify\0Effect\0Sound\0");
			e.EventType = (CLIP_EVENT_TYPE)type;

			// Tag
			ImGui::TableNextColumn();
			char tagBuf[64];
			strcpy_s(tagBuf, e.EventTag.c_str());
			if (ImGui::InputText("##EventTag", tagBuf, IM_ARRAYSIZE(tagBuf)))
				e.EventTag = tagBuf;

			// Delete
			ImGui::TableNextColumn();
			if (ImGui::SmallButton("X"))
			{
				Events.erase(Events.begin() + i);
				ImGui::PopID();
				break;
			}

			ImGui::PopID();
		}
	}

	ImGui::EndTable();
}

void CAnimToolPanel::GUI_Preview(_float fChildHeight)
{
	const ImVec2 buttonSize(55.f, 0.f);

	if (ImGui::Button(m_bPreviewPlay ? "Stop" : "Play", buttonSize)) {
		if (!m_bPreviewPlay) // Play 눌렀을 때만
		{
			m_bPreviewPlay = true;
			m_bPause = false;


			if (m_pSelectAnimator && !m_PreviewList.empty())
			{
				m_iCurrentPrevIndex = 0;
				m_fTrackPos = 0.f;
				m_pSelectAnimator->Get_AnimLayers()[0].fCurrentTrackPosition = 0.f;
				m_pSelectAnimator
					->Set_Animation(m_PreviewList[0])
					.Loop(false);
			}
		}
		else // Stop
		{
			m_bPreviewPlay = false;
			m_bPause = true;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Add", buttonSize)) {
		m_PreviewList.push_back("");
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear", buttonSize)) {
		m_PreviewList.clear();
	}

	for (int i = 0; i < m_PreviewList.size(); i++) {
		string ComboTag = "##Preview Combo" + to_string(i);
		if (ImGui::BeginCombo(ComboTag.c_str(), m_PreviewList[i].c_str())) //Model
		{
			if (!m_AnimClip.empty()) {
				int iIndex = 0;
				for (auto& Clip : m_AnimClip)
				{
					string ClipTag = Clip.ClipTag;
					bool selected = (m_PreviewList[i] == ClipTag);
					if (ImGui::Selectable(ClipTag.c_str(), selected))					{
						//새로운 클립을 눌렀다면
						m_PreviewList[i] = ClipTag;
					}
					if (selected)
						ImGui::SetItemDefaultFocus();

					iIndex++;
				}
			}
			ImGui::EndCombo();
		}
	}
}

void CAnimToolPanel::GUI_Setting_Effect(_float fChildHeight)
{
	//여기서 작업하면댐
}

void CAnimToolPanel::GUI_Create_MetaData(_float fChildHeight)
{
	ImGui::SeparatorText("Clip Datas");
	ImGui::BeginChild("##Loaded OBJECT BTN", ImVec2{ 0, fChildHeight * 2 }, true);

	static string CurMetaTag = { "" };
	if (ImGui::BeginCombo("##Meta Combo", CurMetaTag.c_str())) //Model
	{
		if (!m_Meta.empty()) {
			for (auto& iter : m_Meta)
			{
				bool selected = (CurMetaTag == iter.first);
				if (ImGui::Selectable(iter.first.c_str(), selected))
				{
					CurMetaTag = iter.first;
				}
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Load Clips")) {
		Load_Clips();
	}
	ImGui::SameLine();
	if (ImGui::Button("Create Meta")) {
		Create_ClipMeta(CurMetaTag);
	}

	ImGui::EndChild();
}

void CAnimToolPanel::Setting_NewClip()
{
	m_pSelectAnimator = m_pSelectModel->Get_Component<CAnimator3DEX>();
	if (nullptr == m_pSelectAnimator)
		return;

	m_CurClipTag = m_pSelectAnimator->Get_CurAnimName(0);
	//클립을 지정하지 않은 상태로 함
	m_iCurClipIndex = -1;

	for (auto& clip : m_AnimClip)
		clip.Events.clear();
	m_AnimClip.clear();

	for (auto& Clips : *m_pSelectAnimator->Get_Clips()) {
		ANIM_CLIP newClip{};
		//ClipName
		newClip.ClipTag = Clips->Get_Name();
		//EventData
		newClip.Events = Clips->Get_Events();
		//Pushback
		m_AnimClip.push_back(newClip);
	}
}

void CAnimToolPanel::Reset_Panel()
{
	m_bPause = true;
	m_fTrackPos = 0.f;
	m_fDuration = 0.f;
	m_CurClipTag = "";
	m_AnimClip.clear();

	if (nullptr == m_pSelectModel)
		return;

	Setting_NewClip();
}

void CAnimToolPanel::Add_Event()
{
	if (m_AnimClip.empty() || m_iCurClipIndex < 0)
		return;

	ANIM_EVENT tEvent{ m_fTrackPos, CLIP_EVENT_TYPE::NOTIFY, "" };
	m_AnimClip[m_iCurClipIndex].Events.push_back(tEvent);
}

void CAnimToolPanel::Save_Event()
{
	if (m_AnimClip.empty())
		return;

	size_t pos = m_AnimClip[0].ClipTag.find("_Ani_");
	string ClipKey = m_AnimClip[0].ClipTag.substr(0, pos) + "_Meta.json";

	string MetaPath = m_pGameInstance->Get_ResourceMgr()->Get_ResourcePath(ClipKey);
	Helper::SaveJson<vector<ANIM_CLIP>>(m_AnimClip, MetaPath);
}

ImU32 CAnimToolPanel::GetEventColor(CLIP_EVENT_TYPE eType)
{
	switch (eType)
	{
	case CLIP_EVENT_TYPE::NOTIFY: return IM_COL32(120, 200, 255, 255);
	case CLIP_EVENT_TYPE::EFFECT: return IM_COL32(120, 255, 120, 255);
	case CLIP_EVENT_TYPE::SOUND:  return IM_COL32(255, 200, 120, 255);
	default:                      return IM_COL32(200, 200, 200, 255);
	}
}

void CAnimToolPanel::Load_Clips()
{
	//애니매이션은 전부 이 이름 전후로 잘림
	//ex) Avatar_Female_Size02_Unagi / _Ani_ / Attack_ChargeAttack_Start_Front.anim
	string CutStr = "_Ani_";

	//여러파일 불러오기
	vector<string> files = Helper::OpenMultiFiles();
	
	//.anim 파일 불러오기
	_bool				bFirstFile = true;
	HRESULT				hr = S_OK;
	string				MetaTag = "";
	string				SavePath = "";
	vector<ANIM_CLIP>	MetaData;

	for (auto& path : files) {
		string ClipTag = Helper::GetFileNameWithOutExtension(path);

		//이름 자르기
		auto pos = ClipTag.find(CutStr);
		string nameTag = ClipTag.substr(0, pos);
				
		//만약 처음 불러온 파일이라면 그것을 기준으로 이후 파일체크 및 메타파일이름이 됌
		if (bFirstFile) {
			//이미 들어온 데이터인지 비교
			auto iter = m_Meta.find(nameTag);
			if (iter != m_Meta.end()) {
				//데이터가 존재하며 데이터가 있으면 제거
				for (auto& meta : iter->second) { 
					if (meta.ClipTag == ClipTag) {
						hr = E_FAIL;
						break;
					}
				}
				//없으면 추가
				Create_Clips(MetaData, ClipTag, path);
			}
			//처음 불러온 이름이 전체 이름일것.
			MetaTag = nameTag;
			SavePath = filesystem::path(path).parent_path().string();
			bFirstFile = false;
		}
		//연속으로 들어온 데이터면 이름이 같은지 비교
		else { 
			if (MetaTag != nameTag) {
				OutputDebugStringA("Different from other animation names");
				hr = E_FAIL;
				break;
			}
		}

		//클립 데이터 하나씩 넣기
		Create_Clips(MetaData, ClipTag, path);
	}

	//데이터 추가
	if (SUCCEEDED(hr)) {
		m_Meta.emplace(MetaTag, MetaData);
		m_Paths.emplace(MetaTag, SavePath);
	}
}

void CAnimToolPanel::Create_Clips(vector<ANIM_CLIP>& pMetaData, const string& ClipTag, const string& FilePath)
{
	ANIM_CLIP tClip{};
	tClip.ClipTag = ClipTag;
	pMetaData.push_back(tClip);
	m_pGameInstance->Get_ResourceMgr()->Add_ResourcePath(ClipTag, FilePath);
}

void CAnimToolPanel::Create_ClipMeta(const string& CurMetaTag)
{
	auto iter = m_Meta.find(CurMetaTag);

	if (iter == m_Meta.end()) {
		OutputDebugStringA("Failed creating meta data");
		return;
	}

	string SavePath = m_Paths.find(CurMetaTag)->second + "\\" + CurMetaTag + "_Meta.json";
	Helper::SaveJson<vector<ANIM_CLIP>>(iter->second, SavePath);
}

CBasePanel* CAnimToolPanel::Create(GUI_CONTEXT* context)
{
	return new CAnimToolPanel(context);
}

void CAnimToolPanel::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);

	for (auto clip : m_Meta)
		clip.second.clear();
	m_Meta.clear();
	m_Paths.clear();
}
