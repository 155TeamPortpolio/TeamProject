#include "pch.h"
#include "GUIPanel.h"

#include "GameInstance.h"

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
		static int counter = 0;

		ImGui::Begin("UI Tool");

		if (ImGui::Button("Create Canvas_Panel", ImVec2::ImVec2(100.f, 20.f)))
		{
			CUI_Object* pCanvasPanel = Builder::Create_UIObject({ "UITool_Level" ,"Proto_GameObject_CanvasPanel" })
				.Scale({ CGameInstance::GetInstance()->Get_ClientSize().x, CGameInstance::GetInstance()->Get_ClientSize().y })
				.Set_Anchor(ANCHOR::Left | ANCHOR::Top, {})
				.Build("UI_CanvasPanel" + to_string(counter));

			IUI_Service* pUIMgr = CGameInstance::GetInstance()->Get_UIMgr();
			pUIMgr->Add_UIObject(pCanvasPanel, "UITool_Level");

			counter++;
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