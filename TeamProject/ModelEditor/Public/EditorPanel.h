#pragma once
#include "BasePanel.h"
NS_BEGIN(ModelEdit)
class CEditorPanel :
    public CBasePanel
{
private:
	CEditorPanel(GUI_CONTEXT* context);
	virtual ~CEditorPanel() DEFAULT;
	HRESULT Initialize();

public:
	virtual void Render_GUI() override;
	virtual void Update_Panel(_float dt) override;

private:
	void CreateObject();
	void Read_Material(_bool Json);

public:
	static CEditorPanel* Create(GUI_CONTEXT* context);
	virtual void Free() override;
};

NS_END