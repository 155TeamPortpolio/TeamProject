 #pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_BattleHUDAction final : public CUI_Object
{
private:
	enum class Child { PRIMARY, EVADE, SPECIAL, SWITCH, ULTIMATE, END };

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
	UI_HANDLE		m_handles[ENUM(Child::END)];

	const _float2	m_vOffset = { 91.f, 116.f };

private:
	void Ready_PartObjects();

	void Attach_Child(const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, UI_HANDLE* pHandleOut = nullptr, _float2 vOffset = {});

	void Set_EnableAll(_bool isActive);
	void Set_Enable(Child child, _bool isActive);

	template<typename Func>
	void ForChild(Child child, Func&& func);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END

template<typename Func>
inline void CUI_BattleHUDAction::ForChild(Child child, Func&& func)
{
	auto& handle = m_handles[ENUM(child)];
	if (!handle.isValid())
		return;

	func(handle.Get());
}