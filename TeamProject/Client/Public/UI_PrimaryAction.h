#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_PrimaryAction final : public CUI_Object
{
private:
	enum class MODE { ATTACK, INTERACT, END };

	enum class Child { ATTACK, ATTACK_BG, ATTACK_ICON, ATTACK_MOUSE, INTERACT, INTERACT_GRADIENT, END };

	static const string INSTANCENAMES[ENUM(Child::END)];

private:
	CUI_PrimaryAction() {}
	CUI_PrimaryAction(const CUI_PrimaryAction& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_PrimaryAction() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	MODE			m_eMode = {};
	UI_HANDLE		m_handles[ENUM(Child::END)];

private:
	void Set_ActionMode(MODE eMode);
	void Set_AttackActive(_bool isActive);
	void Set_InteractActive(_bool isActive);

	void Set_Alive(Child child, _bool isAlive);
	void Set_Animation(Child child, _int iIndex);

	template<typename Func>
	void ForChild(Child child, Func&& func);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END

template<typename Func>
inline void CUI_PrimaryAction::ForChild(Child child, Func&& func)
{
	auto& handle = m_handles[ENUM(child)];
	if (!handle.isValid())
		return;

	func(handle.Get());
}