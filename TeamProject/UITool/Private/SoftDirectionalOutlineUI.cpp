#include "pch.h"
#include "SoftDirectionalOutlineUI.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "Sprite2D.h"
#include "Texture.h"
#include "UITool_Level.h"
#include "Engine_Math.h"

HRESULT CSoftDirectionalOutlineUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CSoftDirectionalOutlineUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Set_OriginTexSize(true);

    auto pSprite = Get_Component<CSprite2D>();
    pSprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    pSprite->ChangePass("SoftDirectionalOutline");

    m_strTextureKey = "empty.png";
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, true);

    m_iCount++;

    return S_OK;
}

void CSoftDirectionalOutlineUI::Render_GUI()
{
    __super::Render_GUI();

    Render_GUI_Image(m_strTextureKey);

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

    auto pSprite = Get_Component<CSprite2D>();

    if (ImGui::DragInt("Radius", &m_iRadius, 1, 1, 10, "%d", ImGuiSliderFlags_AlwaysClamp))
        pSprite->Set_Param("iRadius", { &m_iRadius, "int", sizeof(_int) });

    if (ImGui::DragFloat("Glow Strength", &m_fGlowStrength, 0.1f, 0.1f, 3.f, "%f", ImGuiSliderFlags_AlwaysClamp))
        pSprite->Set_Param("fGlowStrength", { &m_fGlowStrength, "float", sizeof(_float) });

    if (ImGui::DragFloat2("Padding Direction", reinterpret_cast<_float*>(&m_vPaddingDir), 0.1f))
        pSprite->Set_Param("vPaddingDir", { &m_vPaddingDir, "float2", sizeof(_float2) });

    if (ImGui::DragFloat("Gaussian Power", &m_fGaussianPower, 0.1f, 1.f, 10.f, "%f", ImGuiSliderFlags_AlwaysClamp))
        pSprite->Set_Param("fGaussianPower", { &m_fGaussianPower, "float", sizeof(_float) });

    m_vTexelSize = 1.f / m_vSize;// pSprite->Get_TexelSize();
    pSprite->Set_Param("vTexelSize", { &m_vTexelSize, "float2", sizeof(_float2) });
}

void CSoftDirectionalOutlineUI::Save(nlohmann::ordered_json& data)
{
    __super::Save(data);

    data["typeTag"] = m_strTypeTag;
    data["textureTag"] = m_strTextureKey;

    auto& json = data["softDirectionalOutline"];
    json["texelSize"] = m_vTexelSize;
    json["radius"] = m_iRadius;
    json["glowStrength"] = m_fGlowStrength;
    json["paddingDir"] = m_vPaddingDir;
    json["gaussianPower"] = m_fGaussianPower;
}

void CSoftDirectionalOutlineUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    m_strTextureKey = data.value("textureTag", "");
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, false);

    if (data.contains("softDirectionalOutline"))
    {
        const auto& json = data["softDirectionalOutline"];
        m_vTexelSize = json.value("texelSize", _float2(0.1f, 0.1f));
        m_iRadius = json.value("radius", 3);
        m_fGlowStrength = json.value("glowStrength", 1.2f);
        m_vPaddingDir = json.value("paddingDir", _float2( 1.f,  0.f));
        m_fGaussianPower = json.value("gaussianPower", 3.f);
    }
}

CGameObject* CSoftDirectionalOutlineUI::Create()
{
    CSoftDirectionalOutlineUI* pInstance = new CSoftDirectionalOutlineUI();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CSoftDirectionalOutlineUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSoftDirectionalOutlineUI::Clone(INIT_DESC* pArg)
{
    CSoftDirectionalOutlineUI* pInstance = new CSoftDirectionalOutlineUI(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CSoftDirectionalOutlineUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}