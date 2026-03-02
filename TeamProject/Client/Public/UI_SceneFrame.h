#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_SceneFrame final : public CUI_Object
{
private:
	enum CHILD { TOP, BOTTOM, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "top", "bottom" };

	enum class STATE { INVISIBLE, VISIBLE, END };

private:
	CUI_SceneFrame() {}
	CUI_SceneFrame(const CUI_SceneFrame& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_SceneFrame() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr)  override;
	virtual void	UI_DeActive(void* pArg = nullptr) override;

private:
	CUI_Object*		m_pChildren[CHILD::END] = {};
	STATE			m_eState = { STATE::END };

private:
	void Change_State(STATE eState);

	void Set_ChildAnimation(CHILD child, _int iIndex);
	_bool Is_ChildAnimFinished(CHILD child);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END