#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_PartyAvatar final : public CGameObject
{
public:
	typedef struct tagAvatarDesc : public GAMEOBJECT_DESC {
		string strRenderTargetKey = "";
	}AVATAR_DESC;

private:
	CUI_PartyAvatar() {}
	CUI_PartyAvatar(const CUI_PartyAvatar& rhs) : CGameObject(rhs) {}
	virtual ~CUI_PartyAvatar() DEFAULT;

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
	string m_strTargetKey = "";

private:
	void Render_RT(ID3D11DeviceContext* pContext);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END