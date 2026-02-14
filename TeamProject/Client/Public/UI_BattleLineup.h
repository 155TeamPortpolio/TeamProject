#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_BattleLineup final : public CUI_Object
{
private:
	CUI_BattleLineup() {}
	CUI_BattleLineup(const CUI_BattleLineup& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_BattleLineup() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	static constexpr _int CARD_COUNT = 3;
	array<string, CARD_COUNT> m_RenderTargetKeys;

	class CUI_BattleLineupCard* m_pLineupCard[CARD_COUNT] = {};

private:
	void Create_BackButton();
	void Create_HomeButton();
	void Create_ElementalResonance();

	void Create_BattleSettingButton();
	void Create_BackupButton(); 
	void Create_EnterButton();

	void Create_RenderTargets();
	void Create_BattleLineupCards();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END