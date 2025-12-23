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

		CreateCanvasPanel();

		LoadPrefab();

		ImGui::End();
	}
}

void CGUIPanel::CreateCanvasPanel()
{
	if (ImGui::Button("Create Canvas_Panel"))
	{
		const string& strCurrentLevelKey = m_pGameInstance->Get_LevelMgr()->Get_NowLevelKey();
		const string& strTypeTag = CCanvasPanel::m_strTypeTag;

		CUI_Object* pObj = Builder::Create_UIObject({ strCurrentLevelKey, "Proto_GameObject_" + strTypeTag })
			.Size({ m_pGameInstance->Get_ClientSize().x, m_pGameInstance->Get_ClientSize().y })
			.Build(strTypeTag + to_string(CCanvasPanel::m_iCount));

		m_pGameInstance->Get_UIMgr()->Add_UIObject(pObj, strCurrentLevelKey);
	}
}

void CGUIPanel::LoadPrefab()
{
	if (ImGui::Button("Load Prefab"))
	{
		string filePath(Helper::OpenFile_Dialogue());
		if (filePath.empty())
			return;

		std::ifstream inputFile(filePath);
		if (!inputFile.is_open())
		{
			MSG_BOX("Failed to open data file");
			return;
		}

		json data;
		inputFile >> data;
		inputFile.close();

		if (!data.contains("parent"))
			return;

		const string& strCurrentLevelKey = m_pGameInstance->Get_LevelMgr()->Get_NowLevelKey();
		CUI_Object* pObj = Builder::Create_UIObject({ strCurrentLevelKey , "Proto_GameObject_" + data["parent"]["typeTag"].get<string>()})
			.Build(data["parent"]["typeTag"].get<string>());

		if (!pObj)
			return;

		CUIObject_Tool* pUI = dynamic_cast<CUIObject_Tool*>(pObj);
		if (!pUI)
			return;

		pUI->LoadPrefab(data["parent"]); 
		m_pGameInstance->Get_UIMgr()->Add_UIObject(pObj, strCurrentLevelKey);
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