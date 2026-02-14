#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_AvatarTest final : public CGameObject
{
private:
	CUI_AvatarTest() {}
	CUI_AvatarTest(const CUI_AvatarTest& rhs) : CGameObject(rhs) {}
	virtual ~CUI_AvatarTest() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()      override;
	virtual HRESULT Initialize(INIT_DESC* pArg) override;
	virtual void    Awake()                     override;
	virtual void    Priority_Update(_float dt)  override;
	virtual void    Update(_float dt)           override;
	virtual void    Late_Update(_float dt)      override;

private:
	_float4x4 m_ViewMatrix = {};
	_float4x4 m_ProjMatrix = {};

	const string m_strPassConstant = "UI_RenderTarget";
	string m_strTargetKey = "avatarTest";

private:
	void Render_RT(ID3D11DeviceContext* pContext);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END