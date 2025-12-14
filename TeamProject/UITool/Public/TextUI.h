#pragma once
#include "UI_Object.h"

NS_BEGIN(UITool)

class CTextUI final : public CUI_Object
{
private:
	CTextUI();
	CTextUI(const CTextUI& rhs);
	virtual ~CTextUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;

	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;

	virtual void Render_GUI() override;

private:
	_int		m_iFontKeyIndex = { 0 };
	_char		m_szText[MAX_PATH] = {};
	_float		m_fFontScale = { 1.f };
	_float4		m_vColor = { 1.f, 1.f, 1.f, 1.f }; 

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END

// 폰트 크기 32로 해서 스케일
// /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 font.spritefont