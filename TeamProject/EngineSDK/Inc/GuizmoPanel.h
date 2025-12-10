#pragma once
#include "BasePanel.h"
class CGuizmoPanel :
	public CBasePanel
{

private:
	CGuizmoPanel(GUI_CONTEXT* context);
	virtual ~CGuizmoPanel();
	HRESULT Initialize();

public:
	virtual void Render_GUI() override;

private:
	void ShowObject_Guizmo();
private:
	ImGuizmo::OPERATION gizmoOperation;
public:
	static CGuizmoPanel* Create(GUI_CONTEXT* context);
	virtual void Free();
};

