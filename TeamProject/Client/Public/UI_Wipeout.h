#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_Wipeout final : public CGameObject
{
private:
	CUI_Wipeout() {}
	CUI_Wipeout(const CUI_Wipeout& rhs) : CGameObject(rhs) {}
	virtual ~CUI_Wipeout() DEFAULT;

public:
	HRESULT Initialize_Prototype()     override;
	HRESULT Initialize(INIT_DESC* pArg) override;
	void    Priority_Update(_float dt) override {}
	void    Update(_float dt)          override;
	void    Late_Update(_float dt)     override {}

private:
	_uint m_iMtrlInstIdx = {};
	_float4x4 m_ViewMatrix = {};
	_float4x4 m_ProjMatrix = {};

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END