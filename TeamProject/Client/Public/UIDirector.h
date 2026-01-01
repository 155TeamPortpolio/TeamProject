#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance; class CUI_Object; class IUI_Service;
NS_END

NS_BEGIN(Client)

class CUIDirector final : public CBase
{
	DECLARE_SINGLETON(CUIDirector);
private:
	CUIDirector() {}
	virtual ~CUIDirector() = default;

public:
	void Initialize(const string& levelKey);
	void Register(CUI_Object* uiObj);

public:
	void Dispatch(UIEventType type, void* arg = {});

private:
	void EnterMonitor(void* arg);
	void ExitMonitor(void* arg);

private:
	void SetActive(const string& tag, void* arg = {});
	void SetActive(initializer_list<string> tags, void* arg = {});
	
	void SetDeactive(const string& tag, void* arg = {});
	void SetDeactive(initializer_list<string> tags, void* arg = {});

private:
	string                             m_levelKey;
	unordered_map<string, CUI_Object*> m_uiByTag;

public:
	virtual void Free() override { __super::Free(); }
};

NS_END