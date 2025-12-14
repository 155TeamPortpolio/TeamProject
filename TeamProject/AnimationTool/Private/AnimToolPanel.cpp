#include "AnimToolPanel.h"
#include "Helper_Func.h"

CAnimToolPanel::CAnimToolPanel(GUI_CONTEXT* pContext)
	: CBasePanel{pContext}
{
}

void CAnimToolPanel::Update_Panel(_float dt)
{
}

void CAnimToolPanel::Render_GUI()
{
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight + 2) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::SeparatorText("Model Load");
	ImGui::BeginChild("##Loaded OBJECT BTN", ImVec2{ 0, childHeight }, true);

	if (ImGui::Button("Model Load")) {
		Extract_Clip();
	}

	ImGui::SameLine();

	if (ImGui::Button("Material Loadar")) {
		
	}
	ImGui::EndChild();

	__super::Render_GUI();
}

void CAnimToolPanel::Extract_Clip()
{
	m_Importer.FreeScene();
	//fbx 파일 열기
	string path = Helper::OpenFile_Dialogue();

	unsigned int iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	m_pAIScene = m_Importer.ReadFile(path.c_str(), iFlag);
	 
	if (nullptr == m_pAIScene)
		return;

	if (!m_pAIScene->HasAnimations())
		return;

	//조건에 모두 만족하면 애니매이션 파일 생성
	string ClipDir = std::filesystem::path(path).parent_path().string() + "\\Animations";
	CreateDirectoryA(ClipDir.c_str(), nullptr);
	
	//클립만큼 반복
	_int iNumClips = m_pAIScene->mNumAnimations;
	for (_int i = 0; i < iNumClips; i++) {
		aiAnimation* pClip = m_pAIScene->mAnimations[i];

		string ClipName = pClip->mName.C_Str(); //클립이름

		string FullPath = ClipDir + "\\" + ClipName + ".anim"; //을 .anim으로 새로운 폴더에 저장
		ofstream ofs(FullPath.c_str(), ios::binary);
		
		if (!ofs.is_open()) {
			MessageBoxA(0, string(ClipName + " save Failed").c_str(), "Animation Save", MB_OK);
			return;
		}
		
		ANIMATION_CLIP_HEADER tClipHeader{};
		strcpy_s(tClipHeader.ClipName, sizeof(tClipHeader.ClipName), ClipName.c_str());
		tClipHeader.fDuration = pClip->mDuration;
		tClipHeader.fTickPerSecond = pClip->mTicksPerSecond;
		tClipHeader.iNumChannels = pClip->mNumChannels;
		
		ofs.write(reinterpret_cast<const char*>(&tClipHeader), sizeof(tClipHeader));
		
		Extract_Channels(pClip, nullptr);
		
		ofs.close();
	}
}

void CAnimToolPanel::Extract_Channels(aiAnimation* pAIAnimation, ofstream* ofs)
{
	_int iNumChannels = pAIAnimation->mNumChannels;
	
	ANIMATION_CHANNEL_HEADER ChannelHeader{};
	//strcpy_s(ChannelHeader.BoneName, sizeof(ChannelHeader.BoneName), pAIAnimation->mB )
	//
	//for (_int i = 0; i < iNumChannels; i++) {
	//	test.push_back(pAIAnimation->mChannels[i]->mNodeName.C_Str());
	//	//Extract_KeyFrame(pAIAnimation->mChannels[i], ofs);
	//}
}

void CAnimToolPanel::Extract_KeyFrame(aiNodeAnim* pAIChannel, ofstream* ofs)
{
	KEYFRAME tKeyFrame{};
	//_int iNumKeyFrames = 
}

CBasePanel* CAnimToolPanel::Create(GUI_CONTEXT* context)
{
	return new CAnimToolPanel(context);
}

void CAnimToolPanel::Free()
{
	__super::Free();
	m_Importer.FreeScene();
}
