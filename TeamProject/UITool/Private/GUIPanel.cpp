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
}

CGUIPanel* CGUIPanel::Create(GUI_CONTEXT* pContext)
{
	return new CGUIPanel(pContext);
}

void CGUIPanel::Free()
{
	__super::Free();
}