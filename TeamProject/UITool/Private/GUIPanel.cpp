#include "pch.h"
#include "GUIPanel.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "ObjectContainer.h"
#include "UIObject_Tool.h"
#include "CanvasPanel.h"
#include "MainApp.h"

#include "ImageUI.h"
#include "TextUI.h"
#include "ButtonUI.h"
#include "SpriteAnimationUI.h"
#include "UVAnimationUI.h"
#include "GaugeUI.h"
#include "MaskUI.h"
#include "NineSliceUI.h"

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
			if (CUIObject_Tool* pSelected = dynamic_cast<CUIObject_Tool*>(pGuiContext->pSelectedObject))
			{
				if (pSelected->Is_Root())
					m_pGameInstance->Get_UIMgr()->Remove_UIObject(pSelected);	// 루트면 ui매니저에서 삭제
				else
					pSelected->Remove_SelfFromParent();							// 루트가 아니면 자신을 자식으로 가진 부모 컨테이너에서 자신을 지움

				pGuiContext->pSelectedObject = nullptr;							// Gui에 selectedObject를 nullptr로 
			}
		}
	}
}

void CGUIPanel::Render_GUI()
{
	{
		ImGui::SetNextWindowPos(ImVec2(220.f, 40.f), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(200.f, 520.f), ImGuiCond_FirstUseEver);

		ImGui::Begin("UI Tool");

		// 캔버스 패널 생성
		Render_GUI_CreateCanvasPanel();

		// 프리팹 새로 열기
		Render_GUI_OpenPrefab();
		// 씬에 프리팹 추가하기
		Render_GUI_AddPrefab();

		// 현재 선택된 캔버스 패널에 프리팹 자식으로 추가하기, 캔버스 패널 저장하기
		Render_GUI_CanvasPanel();

		ImGui::End();
	}
}

void CGUIPanel::Render_GUI_CreateCanvasPanel()
{
	if (ImGui::Button("Create Canvas_Panel"))
	{
		const string& strCurrentLevelKey = m_pGameInstance->Get_LevelMgr()->Get_NowLevelKey();
		const string& strTypeTag = CCanvasPanel::m_strTypeTag;

		CUI_Object* pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_" + strTypeTag })
			.Size({ m_pGameInstance->Get_ClientSize().x, m_pGameInstance->Get_ClientSize().y })
			.Build(strTypeTag);

		m_pGameInstance->Get_UIMgr()->Add_UIObject(pObj, strCurrentLevelKey);
	}
}

void CGUIPanel::Render_GUI_OpenPrefab()
{
	if (ImGui::Button(u8"프리팹 새로 열기"))
	{
		CUI_Object* pNew = LoadPrefab();
		if (!pNew)
			return;

		const string& strCurrentLevel = m_pGameInstance->Get_LevelMgr()->Get_NowLevelKey();

		m_pGameInstance->Get_UIMgr()->Clear(strCurrentLevel);
		m_pGameInstance->Get_UIMgr()->Add_UIObject(pNew, strCurrentLevel);
	}

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip(u8"기존 오브젝트를 삭제하고 프리팹을 새로 열기");
}

void CGUIPanel::Render_GUI_AddPrefab()
{
	if (ImGui::Button(u8"레벨에 프리팹 추가"))
	{
		m_pGameInstance->Get_UIMgr()->Add_UIObject(LoadPrefab(), m_pGameInstance->Get_LevelMgr()->Get_NowLevelKey());
	}

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip(u8"기존 오브젝트를 유지하고 프리팹을 추가");
}

void CGUIPanel::Render_GUI_CanvasPanel()
{
	CGameObject* pObj = Get_SelectedObject();
	if (!pObj)
		return;

	CObjectContainer* pContainer = pObj->Get_Component<CObjectContainer>();
	if (!pContainer)
		return;

	ImGui::Spacing();
	ImGui::SeparatorText(u8"선택된 캔버스");
	ImGui::TextDisabled((u8"이름 : " + pObj->Get_InstanceName()).c_str());

	if (ImGui::Button(u8"프리팹으로 저장"))
		SavePrefab(dynamic_cast<CUIObject_Tool*>(pObj));
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip(u8"선택된 캔버스를 프리팹으로 저장");

	if (ImGui::Button(u8"프리팹을 자식으로 추가"))
		pObj->Get_Component<CObjectContainer>()->Add_Child(LoadPrefab());
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip(u8"선택된 캔버스에 프리팹을 자식으로 추가");

	{
		_bool isCreateChild = {};
		string strType = {};

		ImGui::Spacing();
		ImGui::Text(u8"- 새로 만들어서 자식으로 추가");
		ImGui::Indent(10);
		if (ImGui::Button(u8"캔버스 패널"))
		{
			isCreateChild = true;
			strType = CCanvasPanel::m_strTypeTag;
		}

		if (ImGui::Button(u8"이미지"))
		{
			isCreateChild = true;
			strType = CImageUI::m_strTypeTag;
		}

		if (ImGui::Button(u8"텍스트"))
		{
			isCreateChild = true;
			strType = CTextUI::m_strTypeTag;
		}

		if (ImGui::Button(u8"버튼"))
		{
			isCreateChild = true;
			strType = CButtonUI::m_strTypeTag;
		}

		if (ImGui::Button(u8"스프라이트 애니메이션"))
		{
			isCreateChild = true;
			strType = CSpriteAnimationUI::m_strTypeTag;
		}

		if (ImGui::Button(u8"UV 애니메이션"))
		{
			isCreateChild = true;
			strType = CUVAnimationUI::m_strTypeTag;
		}

		if (ImGui::Button(u8"게이지"))
		{
			isCreateChild = true;
			strType = CGaugeUI::m_strTypeTag;
		}

		if (ImGui::Button(u8"마스크"))
		{
			isCreateChild = true;
			strType = CMaskUI::m_strTypeTag;
		}

		if (ImGui::Button(u8"9슬라이스"))
		{
			isCreateChild = true;
			strType = CNineSliceUI::m_strTypeTag;
		}

		// 자식 생성
		if (isCreateChild)
		{
			string strCurrentLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

			CUI_Object* pChild = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_" + strType })
				.Anchor(ANCHOR::Center)
				.Build(strType);

			if (!pChild)
				return;

			pObj->Get_Component<CObjectContainer>()->Add_Child(pChild);                                                  // 컨테이너에 자식 추가
		}
	} 
}

CUI_Object* CGUIPanel::LoadPrefab()
{
	string filePath = Helper::OpenFile({{"JSON Files", "*.json"}}, "json");
	if (filePath.empty())
		return nullptr;

	const string strExtension = filesystem::path(filePath).extension().string();
	if (strExtension != ".json")
		return nullptr;

	ifstream file(filePath);
	if (!file.is_open())
		return nullptr;

	nlohmann::ordered_json data;
	file >> data;
	file.close();

	string strTypeTag = data.value("typeTag", "");
	CUI_Object* pObj = Builder::Create_UIObject({ G_GlobalLevelKey , "Proto_GameObject_" + strTypeTag })
		.Build(strTypeTag);

	if (!pObj)
		return nullptr;

	CUIObject_Tool* pUIObj = dynamic_cast<CUIObject_Tool*>(pObj);
	if (pUIObj)
		pUIObj->Load(data);

	return pObj;
}

void CGUIPanel::SavePrefab(CUIObject_Tool* pObj)
{
	if (!pObj) return;

	nlohmann::ordered_json data{};
	pObj->Save(data);

	string baseName = pObj->Get_InstanceName();
	if (baseName.empty())
		baseName = "prefab";

	string filePath = Helper::SaveFileDialogByWinAPI(baseName, "json");

	ofstream file(filePath);
	if (file.is_open())
	{
		file << data.dump(4);
		file.close();
	}
}

CGameObject* CGUIPanel::Get_SelectedObject()
{
	auto pGuiContext = m_pGameInstance->Get_GUISystem()->Get_Context();

	return pGuiContext->pSelectedObject;
}