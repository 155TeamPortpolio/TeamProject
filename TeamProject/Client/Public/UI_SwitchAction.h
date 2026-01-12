#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_SwitchAction final : public CUI_Object
{
private:
	enum class CHILD { GROUP, BG, GAUGEBG, GAUGE, ICONBG, ICON, OUTLINE, SPACE, END };

	static const string INSTANCENAMES[ENUM(CHILD::END)];

	enum class INTERACT_STATE { DISABLED, ENABLED };
	enum class ACTION_STATE { UNAVAILABLE, READY };
	enum class EXECUTE_STATE { IDLE, EXECUTING };
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

	INTERACT_STATE	m_interactState = INTERACT_STATE::ENABLED;
	ACTION_STATE	m_actionState = ACTION_STATE::READY;
	EXECUTE_STATE	m_executeState = EXECUTE_STATE::IDLE;

private: 
	void Set_InteractState(INTERACT_STATE state);
	void Set_ActionState(ACTION_STATE state);
	void Start_Execute(EXECUTE_MODE mode, _float fFillAmount);
	void Finish_Execute();

	void RefreshVisual();			// 상태 변경시에만 호출

	void ApplyDisableVisual();
	void ApplyExecuteVisual();
	void ApplyReadyVisual();
	void ApplyUnavailableVisual();

	void Set_Alive(CHILD child, _bool isAlive);
	void Set_Animation(CHILD child, _int iIndex);
	void Set_Color(CHILD child, _float4 vColor);
	_bool Is_AnimFinished(CHILD child);

	template<typename Func>
	void ForChild(CHILD child, Func&& func);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END

template<typename Func>
inline void CUI_SwitchAction::ForChild(CHILD child, Func&& func)
{
	auto& handle = m_handles[ENUM(child)];
	if (!handle.isValid())
		return;

	func(handle.Get());
}