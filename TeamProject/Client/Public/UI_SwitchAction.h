#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_SwitchAction final : public CUI_Object
{
private:
	enum class CHILD { GROUP, BG, GAUGEBG, GAUGE, ICONBG, ICON, OUTLINE, SPACE, END };

	static const string INSTANCENAMES[ENUM(CHILD::END)];

	enum class INTERACT_STATE { DISABLE, ENABLE, AVAILABLE };
	enum class EXECUTE_MODE { ANIM, NONANIM };

private:
	CUI_SwitchAction() {}
	CUI_SwitchAction(const CUI_SwitchAction& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_SwitchAction() DEFAULT;

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
	UI_HANDLE		m_handles[ENUM(CHILD::END)];

	INTERACT_STATE	m_interactState = { INTERACT_STATE::ENABLE };
	_bool			m_isVisualInitialized = {};

private: 
	void Set_InteractState(INTERACT_STATE state);
	void Execute(EXECUTE_MODE mode);
	void Set_FillAmount(_float fFillAmount);

	void Refresh_Visual();			// 상태 변경시에만 호출

	_bool Apply_DisableVisual();
	void Apply_EnableVisual();
	void Apply_AvailableVisual();

	_bool Set_Alive(CHILD child, _bool isAlive);
	_bool Set_Animation(CHILD child, _int iIndex);
	_bool Set_Color(CHILD child, _float4 vColor);

	template<typename Func>
	_bool ForChild(CHILD child, Func&& func);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END

template<typename Func>
inline _bool CUI_SwitchAction::ForChild(CHILD child, Func&& func)
{
	auto& handle = m_handles[ENUM(child)];
	if (!handle.isValid())
		return false;

	func(handle.Get());
	return true;
}