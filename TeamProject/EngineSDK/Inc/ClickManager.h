#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CUI_Object;

class CClickManager : public CBase
{
private:
	CClickManager(HWND hWnd) : m_hWnd(hWnd) {}
	virtual ~CClickManager() {}

public:
	void Update(_float dt);

	void Add_ClickableObject(CUI_Object* object);

private:
	HWND				m_hWnd;
	vector<CUI_Object*>	m_ClickableObjects;
	CUI_Object*         m_pHovered{};
	CUI_Object*         m_pNewHovered{};

public:
	static CClickManager* Create(HWND hWnd) { return new CClickManager(hWnd); }
	virtual void Free() override;
};

NS_END