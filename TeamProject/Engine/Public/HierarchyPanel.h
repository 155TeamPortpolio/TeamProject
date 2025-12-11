#pragma once
#include "BasePanel.h"
NS_BEGIN(Engine)

class CHierarchyPanel final: 
	public CBasePanel
{
private:
	CHierarchyPanel(GUI_CONTEXT* context);
	virtual ~CHierarchyPanel();
	 HRESULT Initialize() ;

public:
	virtual void Render_GUI() override;
	virtual void Update_Panel(_float dt) override;

private:
	void ShowLevelList();
	void ShowLayerList(const string& nowLevel);
	void ShowObjectList();
	void ShowUIObjectList();
	_bool ToggleButton(const char* str_id, bool* v);
	vector<string> ConvertObjectNameList(class CLayer* layer);

private:
	_bool m_bOpened = { true };
	_float m_fPosX = {};
	_uint m_iSelectedLevel = {};
	_uint m_iSelectedLayer = {};
	_bool m_bShowUI = false;
public:
	static CHierarchyPanel* Create(GUI_CONTEXT* context);
	virtual void Free();

};

NS_END
