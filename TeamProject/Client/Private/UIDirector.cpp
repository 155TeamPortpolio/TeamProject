#include "pch.h"
#include "UIDirector.h"
#include "GameInstance.h"
#include "UI_Object.h"

IMPLEMENT_SINGLETON(CUIDirector);

void CUIDirector::Initialize(const string& levelKey)
{
	m_levelKey = levelKey;
	UILoader::Load(m_levelKey);
}

void CUIDirector::Register(CUI_Object* uiObj)
{
	m_uiByTag.emplace(uiObj->Get_InstanceName(), uiObj);
	CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(uiObj, m_levelKey);
}

void CUIDirector::SetVisible(const string& tag, bool visible)
{
	auto it = m_uiByTag.find(tag);

	if (visible) 
		it->second->Set_Alive(true);
	else     
		it->second->Set_Alive(false);
}

void CUIDirector::Free()
{
	__super::Free();
}
