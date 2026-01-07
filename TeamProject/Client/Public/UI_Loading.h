#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_Loading final : public CUI_Object
{
	enum PREFAB { NOW_LOADING, END };

private:
	CUI_Loading() {}
	CUI_Loading(const CUI_Loading& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Loading() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	UI_HANDLE			m_hRoot;
	vector<UI_HANDLE>	m_hChildren;

private:
	void CacheHandle(CUI_Object* pRoot);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END