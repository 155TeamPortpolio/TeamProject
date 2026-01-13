#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_UltimateAction final : public CUI_Object
{
private:
	enum class CHILD { GROUP1, BG, UV, GROUP2, BLACK, STAR, Q, END };

	static const string INSTANCENAMES[ENUM(CHILD::END)];

	enum class INTERACT_STATE { DISABLE, ENABLE };

private:
	CUI_UltimateAction() {}
	CUI_UltimateAction(const CUI_UltimateAction& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_UltimateAction() DEFAULT;

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

	INTERACT_STATE	m_interactState = { INTERACT_STATE::ENABLE };

private:
	void Set_InteractState(INTERACT_STATE state);
	void Execute();

	void Refresh_Visual();			// 상태 변경시에만 호출

	void Apply_DisableVisual();
	void Apply_EnableVisual();

	void Set_Alive(CHILD child, _bool isAlive);
	void Set_Color(CHILD child, _float4 vColor);
	void Set_Animation(CHILD child, _int iIndex);

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
inline void CUI_UltimateAction::ForChild(CHILD child, Func&& func)
{
	auto& handle = m_hChildren[ENUM(child)];
	if (!handle.isValid())
		return;

	func(handle.Get());
}