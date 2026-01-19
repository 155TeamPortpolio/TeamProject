#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_DecibelDigits final : public CUI_Object
{
public:
	typedef struct tagDigitsDesc : public UI_DESC {
		const _float4*	pColor = { nullptr };
		const _float*	pDecibel = { nullptr };
	}DIGITS_DESC;

private:
	enum class DigitTexture { DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4, DIGIT5, DIGIT6, DIGIT7, DIGIT8, DIGIT9, END };
	static const string DIGIT_TEXTURES[ENUM(DigitTexture::END)];

	enum class Child { BG, DIGIT_1000, DIGIT_100, DIGIT_10, DIGIT_1, END };
	enum class Digit { DIGIT_1000, DIGIT_100, DIGIT_10, DIGIT_1, END };

private:
	CUI_DecibelDigits() {}
	CUI_DecibelDigits(const CUI_DecibelDigits& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_DecibelDigits() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	const _float	m_fHeight = 32.f;
	const _vector2	m_vPadding = { 20.f, 10.f }; 
	_float			m_fDigitTotalWidth = {};

	const _float*	m_pDecibel = { nullptr };
	const _float4*	m_pColor = { nullptr };
	_int			m_iPrevDecibel = {};

	UI_HANDLE		m_handles[ENUM(Child::END)];

private:
	static Digit digitOrder[];

private:
	void Ready_PartObjects();

	void Set_Color();
	void Update_Digits(_int iDecibel);
	void Update_Layout(); 

	void Set_Digit(Child child, DigitTexture texture);
	void Set_LayoutDigits();
	void Set_LayoutBg(); 

	CUI_Object* Get_Digit(Digit digit) const;

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END