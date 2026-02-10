#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_Wipeout final : public CUI_Object
{
private:
	enum CHILD { RAINBOW, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "rainbow" };
	
private:
	CUI_Wipeout() {}
	CUI_Wipeout(const CUI_Wipeout& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Wipeout() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	_bool	m_isVisible = {};

private:
	class CUI_Object* m_pChildren[ENUM(CHILD::END)] = {};

private:
	void Cache();

	void Set_ChildAnimation(CHILD child, _int iIndex, _bool isPlayChild = false);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END