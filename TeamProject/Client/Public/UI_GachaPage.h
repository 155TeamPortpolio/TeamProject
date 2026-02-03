#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_GachaPage final : public CUI_Object
{
private:
	CUI_GachaPage() {}
	CUI_GachaPage(const CUI_GachaPage& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_GachaPage() DEFAULT;

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
	void Create_BackButton();
	void Create_Currency();
	void Create_Channels();
	void Create_Conversions();

	void OnClick_Back();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END