#include "AnimToolPanel.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "AnimationClip.h"


CAnimToolPanel::CAnimToolPanel(GUI_CONTEXT* pContext)
	: CBasePanel{pContext}
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

void CAnimToolPanel::Update_Panel(_float dt)
{
}

void CAnimToolPanel::Render_GUI()
{
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight + 2) + (ImGui::GetStyle().WindowPadding.y * 2);

	GUI_Setting_Clips(childHeight);

	__super::Render_GUI();
}

void CAnimToolPanel::GUI_Setting_Clips(_float fChildHeight)
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

	string SavePath = m_Paths.find(CurMetaTag)->second + "\\";
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
