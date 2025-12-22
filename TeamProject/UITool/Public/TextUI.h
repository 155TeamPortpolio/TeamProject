#pragma once
#include "UIObject_Tool.h"

// 텍스트를 트랜스폼에 맞춰서 피봇 설정, 회전, 스케일하는 것 넣어야

NS_BEGIN(UITool)

class CTextUI final : public CUIObject_Tool
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

public:
	virtual void ToJson(json& data) override;
	virtual void FromJson(const json& data) override;

	virtual void SavePrefab(json& data) override;
	virtual void LoadPrefab(const json& data) override;

private:
	_char		m_szText[MAX_PATH] = {};
	_float		m_fFontScale = { 1.f }; 
	_float4		m_vFontColor = { 1.f, 1.f, 1.f, 1.f }; 

	_bool		m_isOutlined = {};
	_float		m_fOutlineThickness = { 1.f };
	_float4		m_vOutlineColor = { 0.f, 0.f, 0.f, 1.f };

	_int		m_iAlign = {};

	_int		m_iFontKeyIndex = { 0 };
	_bool		m_isSizeToContent = { true };

public:
	static const string m_strTypeTag;
	static _uint m_iCount;

private:
	virtual void Render_GUI_Layout() override;
	virtual void Render_GUI_Transform() override;

private:
	void UpdateAnchorOffsetByAlign();
	_int Find_FontIndex(const vector<const _char*> FontKeys, const string strFontTag);

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END

// 폰트 크기 32로 해서 스케일
// /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 font.spritefont