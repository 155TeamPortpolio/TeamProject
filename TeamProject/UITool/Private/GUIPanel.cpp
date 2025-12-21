#include "pch.h"
#include "GUIPanel.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "UIObject_Tool.h"
#include "CanvasPanel.h"
#include "MainApp.h"

CGUIPanel::CGUIPanel(GUI_CONTEXT* pContext)
	: CBasePanel(pContext)
	, m_pGameInstance ( CGameInstance::GetInstance())
{
}

void CGUIPanel::Update_Panel(_float dt)
{
	// Inspector 창에 떠있는 오브젝트 삭제
	if (m_pGameInstance->Get_InputDev()->Key_Tap(VK_DELETE))
	{
		auto pGuiContext = m_pGameInstance->Get_GUISystem()->Get_Context();

		if (pGuiContext->pSelectedObject)
		{
			if (CUIObject_Tool* pUISelected = dynamic_cast<CUIObject_Tool*>(pGuiContext->pSelectedObject))
			{
				pUISelected->Remove_SelfFromParent();						// 자신을 자식으로 가진 부모 컨테이너에서 자신을 지움
				m_pGameInstance->Get_UIMgr()->Remove_UIObject(pUISelected);	// UI Mgr에서 자신을 제거 
				pGuiContext->pSelectedObject = nullptr;						// Gui에 selectedObject를 nullptr로 
			}
		}
	}
}

void CGUIPanel::Render_GUI()
{
	{
		ImGui::Begin("UI Tool");

		//ChangeLevel();

		CreateCanvasPanel();

		SaveToJson();

		LoadFromJson();

		ImGui::Text(to_string(CCanvasPanel::m_iCount).c_str());

		ImGui::End();
	}
}

void CGUIPanel::ChangeLevel()
{
	// 레벨 바꾸고 다시 로드하게 추후 추가할 예정

	auto& szLevelTags = CMainApp::m_szLevelTags;

	ImGui::Combo(u8"레벨", &m_iLevelIndex, szLevelTags.data(), szLevelTags.size());
}

void CGUIPanel::CreateCanvasPanel()
{
	if (ImGui::Button("Create Canvas_Panel"))
	{
		string strCurrentLevelKey = m_pGameInstance->Get_LevelMgr()->Get_NowLevelKey();

		CUI_Object* pCanvasPanel = Builder::Create_UIObject({ strCurrentLevelKey, "Proto_GameObject_CanvasPanel" })
			.Size({ m_pGameInstance->Get_ClientSize().x, m_pGameInstance->Get_ClientSize().y })
			.Build("UI_CanvasPanel" + to_string(CCanvasPanel::m_iCount));

		m_pGameInstance->Get_UIMgr()->Add_UIObject(pCanvasPanel, strCurrentLevelKey);
	}
}

void CGUIPanel::SaveToJson()
{
	if (ImGui::Button("Save"))
	{
		json data;
		data["uiObjects"] = json::array();

		auto& objects = m_pGameInstance->Get_UIMgr()->Get_LevelUI(m_pGameInstance->Get_LevelMgr()->Get_NowLevelKey());

		for (auto& pObj : objects)
		{
			if (!pObj)
				continue;

			CUIObject_Tool* pUI = dynamic_cast<CUIObject_Tool*>(pObj);

			if (!pUI)
				continue;

			json objData;
			pUI->ToJson(objData);
			data["uiObjects"].push_back(objData);
		}

		std::ofstream outputFile(Helper::SaveFileDialogByWinAPI(m_pGameInstance->Get_LevelMgr()->Get_NowLevelKey(), "json"));

		if (outputFile.is_open())
		{
			outputFile << data.dump(4);
			outputFile << std::endl;
			outputFile.close();
		}
	}
}

void CGUIPanel::LoadFromJson()
{
	if (ImGui::Button("Load"))
	{
		std::ifstream inputFile(Helper::OpenFile_Dialogue());

		if (!inputFile.is_open())
		{
			MSG_BOX("Failed to open data file");
			return;
		}

		json data;

		inputFile >> data;

		inputFile.close();

		IUI_Service* pUIMgr = m_pGameInstance->Get_UIMgr();
		pUIMgr->Clear(m_pGameInstance->Get_LevelMgr()->Get_NowLevelKey());

		// json에 있는 ui object 로드해서 level에 저장
		for (auto& uiData : data["uiObjects"])
		{
			const string strType = uiData["typeTag"];
			const string strLevel = uiData["levelTag"];
			string strProtoTag = {};

			// 타입에 따라 프로토타입태그 분기 처리
			if (strType == "CanvasPanel")
				strProtoTag = "Proto_GameObject_CanvasPanel";
			else if (strType == "ImageUI")
				strProtoTag = "Proto_GameObject_ImageUI";
			else if (strType == "TextUI")
				strProtoTag = "Proto_GameObject_TextUI";
			else if (strType == "ButtonUI")
				strProtoTag = "Proto_GameObject_ButtonUI";

			CUI_Object* pObj = Builder::Create_UIObject({ strLevel , strProtoTag })
				.Offset(_float2(uiData["transform"]["anchorOffset"]["x"].get<float>(), uiData["transform"]["anchorOffset"]["y"].get<float>()))
				.Size(_float2(uiData["transform"]["size"]["x"].get<float>(), uiData["transform"]["size"]["y"].get<float>()))
				.Scale(_float2(uiData["transform"]["scale"]["x"].get<float>(), uiData["transform"]["scale"]["y"].get<float>()))
				.Rotate(uiData["transform"]["rotation"].get<float>())
				.Anchor(static_cast<ANCHOR>(uiData["transform"]["anchor"]))
				.Build(uiData["instanceKey"]);

			if (!pObj)
				continue;

			CUIObject_Tool* pUI = dynamic_cast<CUIObject_Tool*>(pObj);
			if (!pUI)
				continue;
			 
			pUI->FromJson(uiData);
			pUIMgr->Add_UIObject(pObj, strLevel);
		}
	}
}

CGUIPanel* CGUIPanel::Create(GUI_CONTEXT* pContext)
{
	return new CGUIPanel(pContext);
}

void CGUIPanel::Free()
{
	__super::Free();
}