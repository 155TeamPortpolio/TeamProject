#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_DecibelKanji final : public CUI_Object
{
public:
	enum class State { NONE, COMBAT_UPROAR, COMBAT_BLASTING, COMBAT_MAXIMUM, END };

private:
	static const string KANJI_TEXTURES[ENUM(State::END)];

	enum class ChildSlot { BG, KANJI, END };

private:
	CUI_DecibelKanji() {}
	CUI_DecibelKanji(const CUI_DecibelKanji& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_DecibelKanji() DEFAULT;

public:
	void Set_Kanji(State texture);

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	const _float	m_fHeight = 50.f;
	const _vector2	m_vPadding = { 10.f, 10.f };

private:
	void Set_KanjiTexture(CUI_Object* pKanji, string textureKey);
	void Set_Layout(CUI_Object* pKanji, CUI_Object* pBg);

	CUI_Object* Get_Slot(ChildSlot slot);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END