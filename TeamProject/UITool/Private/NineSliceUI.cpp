#include "pch.h"
#include "NineSliceUI.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "Sprite2D.h"
#include "Texture.h"
#include "UITool_Level.h"
#include "Engine_Math.h"

HRESULT CNineSliceUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CNineSliceUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Set_OriginTexSize(true);

    auto pSprite = Get_Component<CSprite2D>();
    pSprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    pSprite->ChangePass("NineSlice");

    m_strTextureKey = "empty.png";
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, true);

    m_iCount++;

    return S_OK;
}

void CNineSliceUI::Update(_float dt)
{
    __super::Update(dt);

    auto pSprite = Get_Component<CSprite2D>();

    pSprite->Set_Param("vSizePx", { &m_vSize, "float2", sizeof(_float2) });
    pSprite->Set_Param("vTopLeftPx", { &m_vLeftTop,"float2", sizeof(_float2) });

    if (m_isNineSliceDirty)
    {
        pSprite->Set_Param("vBorderPx", { &m_vBorderPx,"float4", sizeof(_float4) });
        m_isNineSliceDirty = false;

        _uint2 vTextureSize = pSprite->Get_TextureSize();

        m_uvRangeX.x = 0.f;
        m_uvRangeX.y = m_vBorderPx.x / vTextureSize.x;
        m_uvRangeX.z = 1.f - (m_vBorderPx.y / vTextureSize.x);
        m_uvRangeX.w = 1.f;

        m_uvRangeY.x = 0.f;
        m_uvRangeY.y = m_vBorderPx.z / vTextureSize.y;
        m_uvRangeY.z = 1.f - (m_vBorderPx.w / vTextureSize.y);
        m_uvRangeY.w = 1.f;

        pSprite->Set_Param("uvRangeX", { &m_uvRangeX,"float4", sizeof(_float4) });
        pSprite->Set_Param("uvRangeY", { &m_uvRangeY,"float4", sizeof(_float4) });
    } 
}

void CNineSliceUI::Render_GUI()
{
    __super::Render_GUI();

    m_isNineSliceDirty |= Render_GUI_NineSlice();

    m_isNineSliceDirty |= Render_GUI_Image(m_strTextureKey);

    _bool isFlip = {};
    if (ImGui::Checkbox("flip X", &m_isFlipX))
    {
        m_vFlip.x = (m_isFlipX) ? 1.f : 0.f;
        isFlip = true;
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("flip Y", &m_isFlipY))
    {
        m_vFlip.y = (m_isFlipY) ? 1.f : 0.f;
        isFlip = true;
    }
    if (isFlip)
        Get_Component<CSprite2D>()->Set_Param("vFlip", { &m_vFlip, "float2", sizeof(_float2) });
}

void CNineSliceUI::Save(nlohmann::ordered_json& data)
{
    __super::Save(data);

    data["typeTag"] = m_strTypeTag;
    data["textureTag"] = m_strTextureKey;
}

void CNineSliceUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    m_strTextureKey = data.value("textureTag", "");
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, false);
}

_bool CNineSliceUI::Render_GUI_NineSlice()
{
    _bool isDirty = {};

    ImGui::SeparatorText(u8"9Slice Border");
    if (ImGui::DragFloat2("left, right", reinterpret_cast<_float*>(&m_vBorderPx), 1.f, 0.f, m_vSize.x, "%.2f", ImGuiSliderFlags_AlwaysClamp))
    {
        _float fSum = m_vBorderPx.x + m_vBorderPx.y;
        if (fSum > m_vSize.x)
        {
            _float fScale = m_vSize.x / fSum;
            m_vBorderPx.x *= fScale;
            m_vBorderPx.y *= fScale;
        }

        isDirty = true;
    }

    if (ImGui::DragFloat2("top, bottom", reinterpret_cast<_float*>(&m_vBorderPx.z), 1.f, 0.f, m_vSize.y, "%.2f", ImGuiSliderFlags_AlwaysClamp))
    {
        _float fSum = m_vBorderPx.z + m_vBorderPx.w;
        if (fSum > m_vSize.y)
        {
            _float fScale = m_vSize.y / fSum;
            m_vBorderPx.z *= fScale;
            m_vBorderPx.w *= fScale;
        }

        isDirty = true;
    }

    return isDirty;
}

CGameObject* CNineSliceUI::Create()
{
    CNineSliceUI* pInstance = new CNineSliceUI();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CNineSliceUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CNineSliceUI::Clone(INIT_DESC* pArg)
{
    CNineSliceUI* pInstance = new CNineSliceUI(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CNineSliceUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}