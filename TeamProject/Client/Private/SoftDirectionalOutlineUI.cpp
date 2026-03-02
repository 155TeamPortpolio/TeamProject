#include "pch.h"
#include "SoftDirectionalOutlineUI.h"

#include "GameInstance.h"
#include "Sprite2D.h"

HRESULT CSoftDirectionalOutlineUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CSoftDirectionalOutlineUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    return S_OK;
}

void CSoftDirectionalOutlineUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);
    
    auto pSprite = Get_Component<CSprite2D>();
    pSprite->Change_Texture(0, G_GlobalLevelKey, data.value("textureTag", ""));

    if (data.contains("softDirectionalOutline"))
    {
        const auto& json = data["softDirectionalOutline"];

        auto vTexelSize = json.value("texelSize", json::array({ 0.1f, 0.1f }));
        m_vTexelSize = { vTexelSize[0], vTexelSize[1] };
        m_iRadius = json.value("radius", 3);
        m_fGlowStrength = json.value("glowStrength", 1.2f);
        auto vPaddingDir = json.value("paddingDir", json::array({ 0.1f, 0.1f }));
        m_vPaddingDir = { vPaddingDir[0], vPaddingDir[1] };
        m_fGaussianPower = json.value("gaussianPower", 3.f);

        pSprite->Set_Param("vTexelSize", { &m_vTexelSize, "float2", sizeof(_float2) });
        pSprite->Set_Param("iRadius", { &m_iRadius, "int", sizeof(_int) });
        pSprite->Set_Param("fGlowStrength", { &m_fGlowStrength, "float", sizeof(_float) });
        pSprite->Set_Param("vPaddingDir", { &m_vPaddingDir, "float2", sizeof(_float2) });
        pSprite->Set_Param("fGaussianPower", { &m_fGaussianPower, "float", sizeof(_float) });
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