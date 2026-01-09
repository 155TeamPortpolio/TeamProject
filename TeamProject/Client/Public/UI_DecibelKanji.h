#pragma once
#include "UI_Object.h"
#include "UI_Decibel.h"

NS_BEGIN(Client)

class CUI_DecibelKanji final : public CUI_Object
{
public:
	typedef struct tagKanjiDesc : public UI_DESC {
		const _uint*	pState = { nullptr };
		const _float4*	pColor = { nullptr };
	}KANJI_DESC;

private:
	static const string KANJI_TEXTURES[ENUM(CUI_Decibel::State::END)];

	enum class ChildSlot { BG, KANJI, END };

private:
	CUI_DecibelKanji() {}
	CUI_DecibelKanji(const CUI_DecibelKanji& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_DecibelKanji() DEFAULT;

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

	const _uint*	m_pState = { nullptr };
	const _float4*	m_pColor = { nullptr };

private:
	void Ready_PartObjects();
	void Set_Kanji(CUI_Decibel::State texture);
	void Set_KanjiTexture(CUI_Object* pKanji, string textureKey);
	void Set_Layout(CUI_Object* pKanji, CUI_Object* pBg);

	CUI_Object* Get_Slot(ChildSlot slot);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END