#pragma once
#include "BasePanel.h"
NS_BEGIN(Engine)
class CInspectorPanel :
    public CBasePanel
{

private:
	CInspectorPanel(GUI_CONTEXT* context);
	virtual ~CInspectorPanel();
	HRESULT Initialize();

public:
	virtual void Render_GUI() override;
	virtual void Update_Panel(_float dt) override;
	void Set_SelectedObject(class CGameObject* objPtr);

private:
	void ShowObject_Inspector();
private:
	_bool m_bLocked = {false};
public:
	static CInspectorPanel* Create(GUI_CONTEXT* context);
	virtual void Free();
};

NS_END