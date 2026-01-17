#pragma once
#include "BasePanel.h"
NS_BEGIN(Engine)
class CEnvPanel :
    public CBasePanel
{
private:
	CEnvPanel(GUI_CONTEXT* context);
	virtual ~CEnvPanel();

public:
	HRESULT Initialize();
	virtual void Update_Panel(_float dt) override;
	virtual void Render_GUI() override;

private:
	void Check_Env();

private:
	void Render_FogDesc();
	_bool EditFogDescUI(FOG_DESC& fogDesc, _bool& outDirty);
private:
	_bool m_bInitialized = false;
	_bool m_isOpen = false;
	_float m_fOpentime = {};

	_bool m_FogDirty = false;
	FOG_DESC m_fogSnapShot = {};

public:
	static CEnvPanel* Create(GUI_CONTEXT* context);
	virtual void Free();
};

NS_END