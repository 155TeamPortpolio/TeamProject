#pragma once
#include "UI_Object.h"
#include "UI_Decibel.h"

NS_BEGIN(Engine)
class CSprite2D;
NS_END

NS_BEGIN(Client)

class CUI_DecibelKanji final : public CUI_Object
{
public:
	typedef struct tagKanjiDesc : public UI_DESC {
		const _uint* pState = { nullptr };
		const _float4* pColor = { nullptr };
	}KANJI_DESC;

private:
	inline static const string KANJI_TEXTURES[ENUM(CUI_Decibel::State::END)] = { "CombatMaximum.png", "CombatUproar.png", "CombatBlasting.png", "CombatMaximum.png" };

	enum class CHILD { BG, KANJI, END };

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
	CUI_Object*		m_pChildren[ENUM(CHILD::END)] = {};
	class CSprite2D* m_pSprites[ENUM(CHILD::END)] = {};

	const _float	m_fHeight = 50.f;
	const _vector2	m_vPadding = { 20.f, 10.f };

	const _uint*	m_pState = { nullptr };
	const _float4*	m_pColor = { nullptr };
	_uint			m_iPrevState = { 999 };

private:
	void Ready_PartObjects();

	void Set_ChildColor(CHILD child, _float4 vColor);
	void Set_ChildAlpha(CHILD child, _float fAlpha);
	void Change_SpriteTexture(CHILD child, const string& strTextureKey); 

	void Set_KanjiTexture(string textureKey);
	void Set_Layout();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END