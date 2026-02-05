#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_GachaResult final : public CUI_Object
{
private:
	CUI_GachaResult() {}
	CUI_GachaResult(const CUI_GachaResult& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_GachaResult() DEFAULT;

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
	CUI_Object* m_pTitle = {};

	vector<CUI_Object*> m_pItems = {};

	_bool m_isItemAppear = {};
	_int m_iItemIndex = {};
	_float m_fItemTimer = {};
	_float m_fItemDuration = { 0.1f };

	static const _int COL = 5;
	static const _int ROW = 2; 
	static const _int WIDTH = 220.f;
	static const _int HEIGHT = 148.f;
	static const _int SPACING = 4.f;

private:
	void Cache();
	void Create_Items();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END