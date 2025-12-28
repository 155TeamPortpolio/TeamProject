#pragma once

#include "Base.h"
#include "UILoader.h"

NS_BEGIN(Engine)
class CGameInstance; class CUI_Object;
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
	void SetVisible(const string& tag, bool visible);

private:
	string                             m_levelKey;
	unordered_map<string, CUI_Object*> m_uiByTag;

public:
	virtual void Free() override { __super::Free(); }
};
NS_END