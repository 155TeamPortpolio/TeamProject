#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_PrimaryAction final : public CUI_Object
{
private:
	enum class MODE { ATTACK, INTERACT, END };

	enum class CHILD { ATTACK, ATTACK_BG, ATTACK_ICON, ATTACK_MOUSE, INTERACT, INTERACT_GRADIENT, END };

	static const string INSTANCENAMES[ENUM(CHILD::END)];

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
	virtual void	UI_Active(void* pArg = nullptr)  override;
	virtual void	UI_DeActive(void* pArg = nullptr)override;

private:
	MODE			m_mode = {};
	UI_HANDLE		m_handles[ENUM(CHILD::END)];

private:
	void Set_ActionMode(MODE eMode);
	void Set_AttackActive(_bool isActive);
	void Set_InteractActive(_bool isActive);

	void Set_Alive(CHILD child, _bool isAlive);
	void Set_Color(CHILD child, _float4 vColor);
	void Set_Animation(CHILD child, _int iIndex);

	template<typename Func>
	void ForChild(CHILD child, Func&& func);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END

template<typename Func>
inline void CUI_PrimaryAction::ForChild(CHILD child, Func&& func)
{
	auto& handle = m_handles[ENUM(child)];
	if (!handle.isValid())
		return;

	func(handle.Get());
}