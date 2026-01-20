 #pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_BattleHUDAction final : public CUI_Object
{
private:
	enum class CHILD { PRIMARY, EVADE, SPECIAL, SWITCH, ULTIMATE, END };

private:
	CUI_BattleHUDAction() {}
	CUI_BattleHUDAction(const CUI_BattleHUDAction& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_BattleHUDAction() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	CUI_Object*		m_pChildren[ENUM(CHILD::END)];

	const _float2	m_vOffset = { 91.f, 116.f };

private:
	void Ready_PartObjects();
	void Bind_EventListener();

	void Attach_Child(const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, CHILD child, _float2 vOffset = {});

	void Set_EnableAll(_bool isActive);
	void Set_Enable(CHILD child, _bool isActive);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END