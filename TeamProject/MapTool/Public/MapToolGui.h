#pragma once
#include "BasePanel.h"

NS_BEGIN(Engine)
class CGameInstance;

NS_END

NS_BEGIN(MapTool)

class CMapToolGui final : public CBasePanel
{
private:
	CMapToolGui(GUI_CONTEXT* pContext);
	virtual ~CMapToolGui() = default;

public:
	HRESULT			Initialize();
	virtual void	Update_Panel(_float dt) override;
	virtual void	Render_GUI() override;

private:
	void				Compute_Ray();

private:
	CGameInstance* m_pGameInstance = { nullptr };

	RAY			m_Ray = {};
	_float3		m_vRayHitPos = {};


public:
	static CMapToolGui* Create(GUI_CONTEXT* pContext);
	virtual void Free() override;
};

NS_END