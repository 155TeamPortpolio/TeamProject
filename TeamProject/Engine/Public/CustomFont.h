#pragma once
#include "Base.h"
NS_BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont();
	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice,  const wstring& FontFilePath);
	void Draw(SpriteBatch* pBatch, wstring pText, const _float2& vPosition, _fvector vColor,
		_float rotation, const _float2& origin, _float scale);

	void DrawOutlinedText(
		SpriteBatch* pBatch, wstring text,
		const _float2& vPosition, _fvector outlineColor, 
		_fvector textColor, _float thickness,
		_float rotation, const _float2& origin, _float scale);


	_vector TextSize(wstring text, _bool ignorWhite) { return m_pFont->MeasureString(text.c_str(), ignorWhite); }
	_vector TextSize(wstring text) { return m_pFont->MeasureString(text.c_str(), false); }
	_float LineSpace() { return m_pFont->GetLineSpacing(); }

public:
	void DrawSelf(wstring pText, const _float2& vPosition, _fvector vColor,_float rotation, const _float2& origin, _float scale);
private:
	SpriteFont* m_pFont = { nullptr };
	SpriteBatch* m_pBatch = { nullptr };
	CommonStates* m_pStates = { nullptr };

public:
	static CCustomFont* Create(ID3D11Device* pDevice,  const wstring& FontFilePath);
	virtual void Free() override;
};

NS_END