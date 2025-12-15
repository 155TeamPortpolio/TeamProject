#include "AnimToolPanel.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "AnimationClipEX.h"

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

	ImGui::BeginChild("##Loaded OBJECT BTN", ImVec2{ 0, fChildHeight }, true);

	if (ImGui::Button("Load Clips")) {
		Load_Clips();
	}
	ImGui::SameLine();
	if (ImGui::Button("Create Meta")) {
		Load_Clips();
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
	vector<ANIM_CLIP>	MetaData;

	for (auto& path : files) {
		string ClipTag = Helper::GetFileNameWithOutExtension(path);

		//이름 자르기
		auto pos = ClipTag.find(CutStr);
		string nameTag = ClipTag.substr(0, pos); // Avatar_Female_Size02_Unagi
		string aniTag = ClipTag.substr(pos + CutStr.length());// Attack_Counter
		
		//만약 처음 불러온 파일이라면 그것을 기준으로 이후 파일체크 및 메타파일이름이 됌
		if (bFirstFile) {
			//이미 들어온 데이터인지 비교
			auto iter = m_Clips.find(nameTag);
			if (iter != m_Clips.end()) { 
				//데이터가 존재하며 데이터가 있으면 제거
				for (ANIM_CLIP meta : iter->second) { 
					if (meta.ClipTag == aniTag) { 
						hr = E_FAIL;
						break;
					}
				}
				//없으면 추가
				ANIM_CLIP tClip{};
				tClip.ClipTag = aniTag;
				iter->second.push_back(tClip);
			}
			//처음 불러온 이름이 전체 이름일것.
			MetaTag = nameTag; 
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
		ANIM_CLIP tClip{};
		tClip.ClipTag = aniTag;
		MetaData.push_back(tClip);
	}

	//데이터 추가
	if(SUCCEEDED(hr))
		m_Clips.emplace(MetaTag, MetaData);
}

void CAnimToolPanel::Set_Aniation()
{
	CGameObject* pSelectedObject = m_pGameInstance->Get_GUISystem()->Get_Context()->pSelectedObject;
}

CBasePanel* CAnimToolPanel::Create(GUI_CONTEXT* context)
{
	return new CAnimToolPanel(context);
}

void CAnimToolPanel::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
