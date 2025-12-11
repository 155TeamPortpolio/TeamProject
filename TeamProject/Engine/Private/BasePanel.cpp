#include "Engine_Defines.h"
#include "BasePanel.h"
#include "GameInstance.h"

CBasePanel::CBasePanel(GUI_CONTEXT* pContext)
	:m_pContext(pContext)
{
}

void CBasePanel::Update_Panel(_float dt)
{
}

void CBasePanel::Render_GUI()
{
}

void CBasePanel::Free()
{
	__super::Free();
}
