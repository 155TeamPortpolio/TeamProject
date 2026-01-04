#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CLogo final : public CUI_Object
{
private:
	CLogo() {}
	CLogo(const CLogo& rhs) : CUI_Object(rhs) {}
	virtual ~CLogo() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override;
	virtual void    Update(_float dt)                override;
	virtual void    Late_Update(_float dt)		     override;
	virtual void    Render_GUI()				     override;

private:
	UI_HANDLE		m_handle = {};

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free() override { __super::Free(); }
};

NS_END