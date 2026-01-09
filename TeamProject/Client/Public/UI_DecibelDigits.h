#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_DecibelDigits final : public CUI_Object
{
private:
	enum class DigitTexture { DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4, DIGIT5, DIGIT6, DIGIT7, DIGIT8, DIGIT9, END };
	static const string DIGIT_TEXTURES[ENUM(DigitTexture::END)];

	enum class ChildSlot { BG, DIGIT_1000, DIGIT_100, DIGIT_10, DIGIT_1, END };
	enum class DigitSlot { DIGIT_1000, DIGIT_100, DIGIT_10, DIGIT_1, END };

private:
	CUI_DecibelDigits() {}
	CUI_DecibelDigits(const CUI_DecibelDigits& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_DecibelDigits() DEFAULT;

public:
	void Set_Digits(_int iDecibel);

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	const _float	m_fDigitHeight = 32.f;
	const _vector2	m_vPadding = { 10.f, 10.f };

	_float			m_fDigitTotalWidth = {};

private:
	static DigitSlot digitOrder[];

private:
	void Set_Digit(ChildSlot slot, DigitTexture texture);
	void Set_LayoutBg();
	void Set_LayoutDigits();

	CUI_Object* Get_Slot(ChildSlot slot);
	CUI_Object* Get_DigitSlot(DigitSlot slot);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END