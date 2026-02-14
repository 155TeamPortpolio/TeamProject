#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CSprite2D;
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_PartySynergy final : public CUI_Object
{
private:
	CUI_PartySynergy() {}
	CUI_PartySynergy(const CUI_PartySynergy& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_PartySynergy() DEFAULT;

public:
	void Set_Synergy(_int iPartySynergyCount, _int iTotalPartyCount);

	void Set_Synergy(vector<CHARACTER> characters);

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	class CSprite2D* m_pIcon = {};
	class CTextSlot* m_pText = {};

private:
	void Cache();

	void Set_Text(const _wstring& strText);
	void Set_Icon(const string& strTextureKey);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END