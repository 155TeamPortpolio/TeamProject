#include "Engine_Defines.h"
#include "TextSlot.h"
#include "GameInstance.h"
#include "IFontService.h"
#include "CustomFont.h"

CTextSlot::CTextSlot()
{
}

CTextSlot::CTextSlot(const CTextSlot& rhs)
	:m_Info(rhs.m_Info)
{
}

HRESULT CTextSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTextSlot::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

HRESULT CTextSlot::Set_Font(const string FontTag)
{
	CCustomFont* pFont = CGameInstance::GetInstance()->Get_FontSystem()->Get_Font(FontTag);
	if (!pFont)
		return E_FAIL;

	if (m_pFont)
		Safe_Release(m_pFont);

	m_pFont = pFont;
	m_Info.FontTag = FontTag;

	Safe_AddRef(m_pFont);
	return S_OK;
}

void CTextSlot::Set_Color(_float4 color)
{
	m_Info.TextColor = color;
}

void CTextSlot::Set_Size(_float scale)
{
	m_Info.Scale = scale;
}

void CTextSlot::Set_Position(_float2 Pos)
{
	m_Info.TextPos = Pos;
}

void CTextSlot::Set_Rotate(_float Rotate)
{
	m_Info.Rotation = Rotate;
}

void CTextSlot::Set_OutLine(_float Thickness, _float4 OutlineColor)
{
	m_Info.OutLined = true;
	m_Info.OutLineColor = OutlineColor;
	m_Info.Thickness = Thickness;
}

void CTextSlot::Push_Text()
{
	if (!m_bActive || m_Info.Text.empty() || m_Info.FontTag.empty()) {
		return;
	}

	if (m_AnchorInfo.bAutoPos) {
		Set_Anchor(m_AnchorInfo.eAnchor, m_AnchorInfo.vPivot);
	}
	if(isOutLined){
		CGameInstance::GetInstance()->Get_FontSystem()->Push_Text(m_Info);
	}
	else {
		CGameInstance::GetInstance()->Get_FontSystem()->Push_Text(m_Info);
	}
}

void CTextSlot::Render_Text()
{
	if (!m_bActive || m_Info.Text.empty() || m_Info.FontTag.empty()) {
		return;
	}

	m_pFont->DrawSelf(
		m_Info.Text,
		m_Info.TextPos,
		XMLoadFloat4(&m_Info.TextColor),
		m_Info.Rotation,
		m_Info.Origin,
		m_Info.Scale
	);
}

void CTextSlot::Set_AutoPos(ANCHOR anchor, _float2 Pivot)
{
	m_AnchorInfo.bAutoPos = true;
	m_AnchorInfo.vPivot = Pivot;
	m_AnchorInfo.eAnchor = anchor;
}

void CTextSlot::Set_Anchor(ANCHOR anchor, _float2 Pivot)
{
	_vector size = m_pFont->TextSize(m_Info.Text);
	float w = XMVectorGetX(size) * m_Info.Scale;
	float h = XMVectorGetY(size) * m_Info.Scale;
	_uint anchorFlags = static_cast<_uint>(anchor);

	// X√‡
	if (anchorFlags & static_cast<_uint>(ANCHOR::Left))
		m_Info.TextPos.x = Pivot.x;
	else if (anchorFlags & static_cast<_uint>(ANCHOR::Right) )
		m_Info.TextPos.x = Pivot.x - w;
	else // Center X
		m_Info.TextPos.x = Pivot.x - w * 0.5f;

	// Y√‡
	if (anchorFlags & static_cast<_uint>(ANCHOR::Top))
		m_Info.TextPos.y = Pivot.y;
	else if (anchorFlags & static_cast<_uint>(ANCHOR::Bottom))
		m_Info.TextPos.y = Pivot.y - h;
	else // Center Y
		m_Info.TextPos.y = Pivot.y - h * 0.5f;
}


_float2 CTextSlot::Get_Anchor(ANCHOR anchot)
{
	_vector Size = m_pFont->TextSize(m_Info.Text);
	_uint anchorFlags = static_cast<_uint>(anchot);
	_float2 result = {};

	if (anchorFlags & static_cast<_uint>(ANCHOR::Left))
		result.x = m_Info.TextPos.x;
	else if (anchorFlags & static_cast<_uint>(ANCHOR::Right))
		result.x = m_Info.TextPos.x+ XMVectorGetX(Size);
	else
		result.x = m_Info.TextPos.x - XMVectorGetX(Size) * 0.5f;


	if (anchorFlags & static_cast<_uint>(ANCHOR::Top))
		result.y = m_Info.TextPos.y;
	else if (anchorFlags & static_cast<_uint>(ANCHOR::Bottom))
		result.y = m_Info.TextPos.y + XMVectorGetY(Size);
	else
		result.y = m_Info.TextPos.y + XMVectorGetY(Size)*0.5f;


	return result;
}

_float CTextSlot::Get_TextSize()
{
	if (m_pFont)
		return XMVectorGetX(m_pFont->TextSize(m_Info.Text)); 
	else 
		return 0.f;
}

CTextSlot* CTextSlot::Create()
{
	CTextSlot* instance = new CTextSlot();
	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("CTextSlot Comp Failed To Create : CTextSlot");
	}
	return instance;
}

CComponent* CTextSlot::Clone()
{
	return new CTextSlot(*this);
}

void CTextSlot::Free()
{
	__super::Free();
	Safe_Release(m_pFont);
}
