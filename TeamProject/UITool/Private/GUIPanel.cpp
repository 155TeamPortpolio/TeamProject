#include "pch.h"
#include "GUIPanel.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "UIObject_Tool.h"
#include "CanvasPanel.h"

CGUIPanel::CGUIPanel(GUI_CONTEXT* pContext)
	: CBasePanel(pContext)
{
}

void CGUIPanel::Update_Panel(_float dt)
{
}

void CGUIPanel::Render_GUI()
{
	{
		ImGui::Begin("UI Tool");

		if (ImGui::Button("Create Canvas_Panel", ImVec2::ImVec2(100.f, 20.f)))
		{
			CUI_Object* pCanvasPanel = Builder::Create_UIObject({ "UITool_Level" ,"Proto_GameObject_CanvasPanel" })
				.Size({ CGameInstance::GetInstance()->Get_ClientSize().x, CGameInstance::GetInstance()->Get_ClientSize().y })
				.Build("UI_CanvasPanel" + to_string(CCanvasPanel::m_iCount++));

			IUI_Service* pUIMgr = CGameInstance::GetInstance()->Get_UIMgr();
			pUIMgr->Add_UIObject(pCanvasPanel, "UITool_Level");
		}

		if (ImGui::Button("Save"))
		{
			json data;
			data["uiObjects"] = json::array();

			auto& levelUIObjects = CGameInstance::GetInstance()->Get_UIMgr()->Get_LevelUI("UITool_Level");

			for (auto& pObj : levelUIObjects)
			{
				CUIObject_Tool* pUI = dynamic_cast<CUIObject_Tool*>(pObj);

				if (!pUI)
				{
					MSG_BOX("Warning : UI Object is not serializable");
					continue;
				} 

				data["uiObjects"].push_back(pUI->ToJson());
			}

			std::ofstream outputFile(Helper::SaveFileDialogByWinAPI("uiObjects", "json"));

			if (outputFile.is_open())
			{
				outputFile << data.dump(4);
				outputFile << std::endl;
				outputFile.close();
			}
		}

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

			vector<CUIObject_Tool*> UIObjects;

			// json에 있는 ui object 로드해서 level에 저장
			for (auto& uiData : data["uiObjects"])
			{
				string strType = uiData["typeTag"];
				string strLevel = uiData["levelTag"];
				string strProtoTag = {};

				CUI_Object* pObj = { nullptr };

				if (strType == "CanvasPanel")
					strProtoTag = "Proto_GameObject_CanvasPanel";
				else if (strType == "ImageUI")
					strProtoTag = "Proto_GameObject_ImageUI";
				else if (strType == "TextUI")
					strProtoTag = "Proto_GameObject_TextUI";

				pObj = Builder::Create_UIObject({ strLevel , strProtoTag })
					.Offset(_float2(uiData["transform"]["anchorOffset"]["x"].get<float>(), uiData["transform"]["anchorOffset"]["y"].get<float>()))
					.Size(_float2(uiData["transform"]["size"]["x"].get<float>(), uiData["transform"]["size"]["y"].get<float>()))
					.Scale(_float2(uiData["transform"]["scale"]["x"].get<float>(), uiData["transform"]["scale"]["y"].get<float>()))
					.Rotate(uiData["transform"]["rotation"].get<float>())
					.Build(uiData["instanceKey"]);

				if (!pObj)
					continue;

				CUIObject_Tool* pUI = dynamic_cast<CUIObject_Tool*>(pObj);
				if (!pUI)
					continue;

				UIObjects.push_back(pUI);
				pUI->FromJson(uiData);

				IUI_Service* pUIMgr = CGameInstance::GetInstance()->Get_UIMgr();
				pUIMgr->Add_UIObject(pObj, strLevel);
			}

			// 자식을 찾아서 부모의 컨테이너 컴포넌트에 추가
			_int iIndex = {};
			for (auto& uiData : data["uiObjects"])
			{
				if (uiData.contains("children") && uiData["children"].size())
					UIObjects[iIndex++] ->LinkChildFromJson(uiData);
			}
		}

		ImGui::End();
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