#pragma once
#include "BasePanel.h"
NS_BEGIN(Engine)
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
	void DrawLookVectorLine(class CTransform* transform);
private:
	ImGuizmo::OPERATION gizmoOperation;
	_bool m_isActive = {true};
public:
	static CGuizmoPanel* Create(GUI_CONTEXT* context);
	virtual void Free();
};

NS_END