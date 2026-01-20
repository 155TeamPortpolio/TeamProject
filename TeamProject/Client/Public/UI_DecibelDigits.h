#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CSprite2D;
NS_END

NS_BEGIN(Client)

class CUI_DecibelDigits final : public CUI_Object
{
public:
	typedef struct tagDigitsDesc : public UI_DESC {
		const _float4* pColor = { nullptr };
		const _float* pDecibel = { nullptr };
	}DIGITS_DESC;

private:
	enum class DigitTexture { DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4, DIGIT5, DIGIT6, DIGIT7, DIGIT8, DIGIT9, END };
	inline static const string DIGIT_TEXTURES[ENUM(DigitTexture::END)] = { "00.png", "01.png", "02.png", "03.png", "04.png", "05.png", "06.png", "07.png", "08.png", "09.png" };

	enum class CHILD { BG, DIGIT_1000, DIGIT_100, DIGIT_10, DIGIT_1, END };
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
	CUI_Object*		m_pChildren[ENUM(CHILD::END)] = {};
	class CSprite2D* m_pSprites[ENUM(CHILD::END)] = {};

	const _float	m_fHeight = 32.f;
	const _vector2	m_vPadding = { 20.f, 10.f }; 
	_float			m_fDigitTotalWidth = {};

	const _float*	m_pDecibel = { nullptr };
	const _float4*	m_pColor = { nullptr };
	_int			m_iPrevDecibel = {};

private:
	static Digit digitOrder[];

private:
	void Ready_PartObjects();
	 
	void Update_Digits(_int iDecibel);
	void Update_Layout(); 

	void Set_Digit(CHILD child, DigitTexture texture);
	void Set_LayoutDigits();
	void Set_LayoutBg();

	void Set_ChildColor(CHILD child, _float4 vColor);
	void Change_SpriteTexture(CHILD child, const string& strTextureKey);

	CUI_Object* Get_Digit(Digit digit) const;

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END