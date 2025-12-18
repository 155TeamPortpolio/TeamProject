#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CClickManager :
	public CBase
{
private:
	CClickManager(HWND hWnd);
	virtual ~CClickManager();

public:
	void Update(_float dt);

public:
	void Add_ClickableObject(class CUI_Object* object);

private:
	HWND						m_hWnd;
	vector<class CUI_Object*>	m_ClickableObjects;

	class CUI_Object*			m_pHovered = { nullptr };
	class CUI_Object*			m_pNewHovered = { nullptr };

public:
	static CClickManager* Create(HWND hWnd);
	virtual void Free() override;
};

NS_END