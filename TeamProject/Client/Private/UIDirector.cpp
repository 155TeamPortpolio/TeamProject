#include "pch.h"
#include "UIDirector.h"
#include "GameInstance.h"
#include "UI_Object.h"
#include "UILoader.h"

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

void CUIDirector::SetActive(const string& tag, void* arg)
{
	auto it = m_uiByTag.find(tag);
	if (it == m_uiByTag.end()) return;
	return it->second->UI_Active(arg);
}

void CUIDirector::SetActive(initializer_list<string> tags, void* arg)
{
	for (const auto& tag : tags)
		SetActive(tag, arg);
}

void CUIDirector::SetDeactive(const string& tag, void* arg)
{
	auto it = m_uiByTag.find(tag);
	if (it == m_uiByTag.end()) return;
	return it->second->UI_DeActive();
}

void CUIDirector::SetDeactive(initializer_list<string> tags, void* arg)
{
	for (const auto& tag : tags)
		SetDeactive(tag, arg);
}

void CUIDirector::Free()
{
	__super::Free();
}
