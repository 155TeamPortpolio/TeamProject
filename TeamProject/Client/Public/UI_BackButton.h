#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_BackButton final : public CUI_Object
{
private:
	enum class CHILD { BUTTON, OVERLAY, ICON, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "button", "overlay", "icon" };

private:
	CUI_BackButton() {}
	CUI_BackButton(const CUI_BackButton& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_BackButton() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	_bool m_isClicked = {};
	CUI_Object* m_pChildren[ENUM(CHILD::END)] = {};

private:
	void Cache_Children();

	void Set_ChildAnimation(CHILD child, _int iIndex);
	_bool Is_ChildAnimationFinished(CHILD child);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END