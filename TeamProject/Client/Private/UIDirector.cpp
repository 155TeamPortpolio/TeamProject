#include "pch.h"
#include "UIDirector.h"
#include "GameInstance.h"
#include "UI_Object.h"

IMPLEMENT_SINGLETON(CUIDirector);

void CUIDirector::Initialize(const string& levelKey)
{
	m_levelKey = levelKey;

	UILoader::RegisterUI(m_levelKey);
}

void CUIDirector::Register(CUI_Object* uiObj)
{
	const string& tag = uiObj->Get_InstanceName();
	auto [it, inserted] = m_uiByTag.emplace(tag, uiObj);
	ASSERT_MSG(inserted, "CUIDirector::Register Same Tag: " + tag);
}

void CUIDirector::SetVisible(const string& tag, bool visible)
{
	auto it = m_uiByTag.find(tag);
	ASSERT_MSG(it != m_uiByTag.end(), "CUIDirector::SetVisible Missing Tag: " + tag);

	if (visible) 
		it->second->UI_Active();
	else     
		it->second->UI_DeActive();
}