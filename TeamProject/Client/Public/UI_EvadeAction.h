#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_EvadeAction final : public CUI_Object
{
private:
	enum class CHILD { BG, GAUGE_BG, GAUGE, ICON, TEXT, MOUSE, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "bg", "gaugeBg", "gauge", "icon", "text", "mouse" };

	enum class INTERACT_STATE { DISABLE, ENABLE };

private:
	CUI_EvadeAction() {}
	CUI_EvadeAction(const CUI_EvadeAction& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_EvadeAction() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr)  override;
	virtual void	UI_DeActive(void* pArg = nullptr)override;

private:
	CUI_Object*			m_pChildren[ENUM(CHILD::END)] = {};
	class CGaugeUI*		m_pGauge = { nullptr };
	class CTextSlot*	m_pTextSlot = { nullptr };

	INTERACT_STATE		m_interactState = { INTERACT_STATE::ENABLE };
	_bool				m_isPerfect = {};

	UI_ACTION_PRIMARY_MODE m_eMode = { UI_ACTION_PRIMARY_MODE::ATTACK };

private:
	void Load_Json(const string& resourceKey);
	void Cache_Children();
	void Bind_EventListener();

	void Set_InteractState(INTERACT_STATE state);
	void Set_Gauge(_float fFillAmount);

	void Refresh_Visual();			// 상태 변경시에만 호출

	void Apply_DisableVisual();
	void Apply_EnableVisual();

	void Set_ChildAlive(CHILD child, _bool isAlive);
	void Set_ChildColor(CHILD child, _float4 vColor);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END