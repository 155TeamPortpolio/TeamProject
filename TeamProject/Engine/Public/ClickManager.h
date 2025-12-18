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
	virtual void Update(_float dt);

public:
	virtual void Add_ClickableObject(class CUI_Object* object);

private:
	HWND						m_hWnd;
	vector<class CUI_Object*>	m_ClickableObjects;

public:
	static CClickManager* Create(HWND hWnd);
	virtual void Free() override;
};

NS_END