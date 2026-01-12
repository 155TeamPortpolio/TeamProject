#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_EvadeAction final : public CUI_Object
{
private:
	enum class CHILD { BG, ICON, GAUGE_BG, GAUGE, MOUSE, END };

	static const string INSTANCENAMES[ENUM(CHILD::END)];

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
	UI_HANDLE		m_hChildren[ENUM(CHILD::END)];

private:
	void Set_Active(_bool isActive);
	void Set_FillAmount(_float fFillAmount);

	void Set_Alive(CHILD child, _bool isAlive);
	void Set_Color(CHILD child, _float4 vColor);

private:
	template<typename Func>
	void ForChild(CHILD child, Func&& func);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END

template<typename Func>
inline void CUI_EvadeAction::ForChild(CHILD child, Func&& func)
{
	auto& handle = m_hChildren[ENUM(child)];
	if (!handle.isValid())
		return;

	func(handle.Get());
}