#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_Decibel final : public CUI_Object
{
public:
	enum State { NONE, COMBAT_UPROAR, COMBAT_BLASTING, COMBAT_MAXIMUM, END };

private:
	enum class ChildSlot { KANJI, DIGITS, PTS, TEXT, END };
	
private:
	CUI_Decibel() {}
	CUI_Decibel(const CUI_Decibel& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Decibel() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	_bool			m_initLayout = {};

	_float			m_fDecibel = {};
	_uint			m_iState = {};
	_float			m_fDigitsOffsetX = {};

	_vector4		m_vTargetColor = {};
	_float			m_fColorLerpSpeed = { 8.f };

private:
	UI_STATUS_OWNER		m_eOwner = { UI_STATUS_OWNER::ROLE1 };
	UI_STATUS_TYPE		m_eType = { UI_STATUS_TYPE::HP };

private:
	void Ready_PartObjects();
	void Set_Decibel(_float fDecibel);

	CUI_Object* Get_Slot(ChildSlot slot);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END