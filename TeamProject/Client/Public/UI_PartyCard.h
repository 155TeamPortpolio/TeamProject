#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_PartyCard final : public CUI_Object
{
public:
	typedef struct tagCardDesc : public UI_DESC {
		string strRenderTargetKey = "";
	}CARD_DESC;

private:
	CUI_PartyCard() {}
	CUI_PartyCard(const CUI_PartyCard& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_PartyCard() DEFAULT;

public:
	void Change_Character(CHARACTER eCharacter);

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	OBJECT_HANDLE m_hAvatar = {};

private:
	void Create_RenderTarget(const string& strRenderTargetKey);
	HRESULT Create_Avatar(const string& strRenderTargetKey);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END