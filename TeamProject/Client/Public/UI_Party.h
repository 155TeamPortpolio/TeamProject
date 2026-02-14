#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_Party final : public CUI_Object
{
public:
	typedef struct tagPartyDesc {
		vector<CHARACTER> characters;
	}UI_PARTY_DESC;

private:
	CUI_Party() {}
	CUI_Party(const CUI_Party& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Party() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void    UI_Active(void* pArg = nullptr)	 override;

private:
	static constexpr _int PARTY_COUNT = 3;
	array<string, PARTY_COUNT> m_RenderTargetKeys;

	class CUI_PartyCard* m_pPartyCard[PARTY_COUNT] = {};

private:
	void Create_BackButton();
	void Create_HomeButton();
	void Create_PartySynergy();

	void Create_SettingButton();
	void Create_BackupButton(); 
	void Create_EnterButton();

	void Create_RenderTargets();
	void Create_PartyCards();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END