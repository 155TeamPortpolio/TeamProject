#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_SpecialAction final : public CUI_Object
{
private:
	enum class Child { BG, ICON, GROUP, MASK, UV, ACTIVE, BLINK, E, END };

	static const string INSTANCENAMES[ENUM(Child::END)];

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
	UI_HANDLE		m_handles[ENUM(Child::END)];
	_bool			m_isEnabled = {};
	_bool			m_isReady = {};

private:
	void Set_Enabled(_bool isEnabled);
	void Set_Ready();
	void Use();

	void Set_Alive(Child child, _bool isAlive);
	void Set_Animation(Child child, _int iIndex);
	void Set_Color(Child child, _float4 vColor);

	template<typename Func>
	void ForChild(Child child, Func&& func);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END

template<typename Func>
inline void CUI_SpecialAction::ForChild(Child child, Func&& func)
{
	auto& handle = m_handles[ENUM(child)];
	if (!handle.isValid())
		return;

	func(handle.Get());
}