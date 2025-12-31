#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CTextUI final : public CUIObject_Tool
{
public:
	enum class TEXTALIGN { LEFT, CENTER, RIGHT };

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

public:
	virtual void FillElementData(UI_ELEMENT_DATA& data) override;
	virtual void ReadElementData(const UI_ELEMENT_DATA& data) override;

private:
	_char		m_szText[MAX_PATH] = {};
	_float		m_fFontScale = { 1.f }; 

	_bool		m_isOutlined = {};
	_float		m_fOutlineThickness = { 1.f };
	_float4		m_vOutlineColor = { 0.f, 0.f, 0.f, 1.f };

	_int		m_iTextAlign = {};

	_bool		m_isSizeToContent = { true };

private:
	string		m_strFontTag = {};
	_int		m_iFontKeyIndex = { 0 }; 

public:
	static const string m_strTypeTag;
	static _uint m_iCount;

private:
	virtual void Render_GUI_Layout() override;
	virtual void Render_GUI_Transform() override;

private:
	void UpdateAnchorOffset_TextAlign();

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END

// 폰트 크기 32로 해서 스케일
// /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 font.spritefont