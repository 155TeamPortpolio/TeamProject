#pragma once
#include "UI_Object.h"
#include "UI_Decibel.h"

NS_BEGIN(Engine)
class CSprite2D;
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_DecibelText final : public CUI_Object
{
public:
	typedef struct tagTextDesc : public UI_DESC {
		const _uint* pState = { nullptr };
		const _float4* pColor = { nullptr };
	}TEXT_DESC;

private:
	inline static const wstring TEXT_CONTENTS[ENUM(CUI_Decibel::State::END)] = { L"", L"UPROAR", L"Blasting", L"Maximum" };

	enum class CHILD { BG, TEXTS, END };

private:
	CUI_DecibelText() {}
	CUI_DecibelText(const CUI_DecibelText& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_DecibelText() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	CUI_Object*		m_pChildren[ENUM(CHILD::END)] = {};
	class CSprite2D* m_pBg = { nullptr };
	class CTextSlot* m_pText = { nullptr };

	const _float	m_fHeight = { 12.f };
	const _float	m_fTextScale = { 0.32f };
	const _vector2	m_vPadding = { 10.f, 5.f };

	const _uint*	m_pState = { nullptr };
	const _float4*	m_pColor = { nullptr };
	_uint			m_iPrevState = { 999 };

private:
	void Ready_PartObjects();

	void Init_TextSlot();

	void Set_Text(const wstring& wstrText);
	void Update_Layout();
	_float2 Calc_TextPxSize() const;

	void Set_ChildColor(CHILD child, _float4 vColor);
	void Set_ChildAlpha(CHILD child, _float fAlpha);
	void Change_BgTexture(const string& strTextureKey);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END