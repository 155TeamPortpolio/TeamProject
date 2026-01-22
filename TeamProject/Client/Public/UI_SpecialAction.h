#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_SpecialAction final : public CUI_Object
{
private:
	enum class CHILD { BG, ICON, GROUP, MASK, UV, ACTIVE, BLINK, E, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "bg", "icon", "group", "mask", "uv", "active", "blink", "e" };

	enum class INTERACT_STATE { DISABLE, ENABLE, AVAILABLE };

private:
	CUI_SpecialAction() {}
	CUI_SpecialAction(const CUI_SpecialAction& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_SpecialAction() DEFAULT;

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
	CUI_Object*		m_pChildren[ENUM(CHILD::END)] = {};

	_bool			m_isEnabled = { true };
	_bool			m_isAvailable = {};

	INTERACT_STATE	m_interactState = { INTERACT_STATE::ENABLE };

	UI_ACTION_PRIMARY_MODE m_eMode = { UI_ACTION_PRIMARY_MODE::END };

private:
	void Load_Json(const string& resourceKey);
	void Cache_Children();
	void Bind_EventListener();

	void Set_InteractState(INTERACT_STATE state);
	void Execute();

	void Refresh_Visual();			// 상태 변경시에만 호출

	void Apply_DisableVisual();
	void Apply_EnableVisual();
	void Apply_AvailableVisual();

	void Set_ChildAlive(CHILD child, _bool isAlive); 
	void Set_ChildColor(CHILD child, _float4 vColor);
	void Set_ChildAnimation(CHILD child, _int iIndex);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END