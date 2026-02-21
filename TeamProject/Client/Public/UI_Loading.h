#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
class CSprite2D;
NS_END

NS_BEGIN(Client)

class CUI_Loading final : public CUI_Object
{
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
	class CSprite2D* m_pBg = {};
	class CTextSlot* m_pSubtitle = {};

private:
	void Cache();
	wstring Get_RandomText(const string& strNextLevelKey);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END