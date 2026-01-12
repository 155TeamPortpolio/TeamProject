#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_SpecialAction final : public CUI_Object
{
private:
	enum class CHILD { BG, ICON, GROUP, MASK, UV, ACTIVE, BLINK, E, END };

	static const string INSTANCENAMES[ENUM(CHILD::END)];

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
	UI_HANDLE		m_handles[ENUM(CHILD::END)];
	_bool			m_isEnabled = { true };
	_bool			m_isAvailable = {};

private:
	/*액션을 사용할 수 있는, 없는 상태로 전환*/
	void Set_Enabled(_bool isEnabled);
	/*스페셜 액션을 사용할 수 있는 준비 상태로 전환*/
	void Set_Available();
	/*스페셜 액션을 사용*/
	void Execute();

	void Set_Alive(CHILD child, _bool isAlive);
	void Set_Animation(CHILD child, _int iIndex);
	void Set_Color(CHILD child, _float4 vColor);

	template<typename Func>
	void ForChild(CHILD child, Func&& func);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END

template<typename Func>
inline void CUI_SpecialAction::ForChild(CHILD child, Func&& func)
{
	auto& handle = m_handles[ENUM(child)];
	if (!handle.isValid())
		return;

	func(handle.Get());
}