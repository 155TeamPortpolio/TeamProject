#include "pch.h"
#include "NineSliceUI.h"

#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CNineSliceUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CNineSliceUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    return S_OK;
}

void CNineSliceUI::Update(_float dt)
{
    __super::Update(dt);

    auto pSprite = Get_Component<CSprite2D>();

    pSprite->Set_Param("vSizePx", { &m_vSize, "float2", sizeof(_float2) });
    pSprite->Set_Param("vTopLeftPx", { &m_vLeftTop,"float2", sizeof(_float2) });
}

void CNineSliceUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    auto pSprite = Get_Component<CSprite2D>();

    pSprite->Change_Texture(0, G_GlobalLevelKey, data.value("textureTag", ""));

    if (data.contains("nineSlice"))
    {
        const auto& uvAnimationJson = data["nineSlice"];

        auto borderPx = uvAnimationJson.value("borderPx", json::array({ 0.0f, 0.0f, 0.0f, 0.0f }));
        m_vBorderPx = { borderPx[0], borderPx[1], borderPx[2], borderPx[3] };

        auto uvRangeX = uvAnimationJson.value("uvRangeX", json::array({ 0.0f, 0.0f, 0.0f, 0.0f }));
        m_uvRangeX = { uvRangeX[0], uvRangeX[1], uvRangeX[2], uvRangeX[3] };

        auto uvRangeY = uvAnimationJson.value("uvRangeY", json::array({ 0.0f, 0.0f, 0.0f, 0.0f }));
        m_uvRangeY = { uvRangeY[0], uvRangeY[1], uvRangeY[2], uvRangeY[3] };

        auto textureSize = uvAnimationJson.value("textureSize", json::array({ 0.0f, 0.0f }));
        m_vTextureSize = { textureSize[0], textureSize[1] };
         
        pSprite->Set_Param("vBorderPx", { &m_vBorderPx,"float4", sizeof(_float4) });
        pSprite->Set_Param("uvRangeX", { &m_uvRangeX,"float4", sizeof(_float4) });
        pSprite->Set_Param("uvRangeY", { &m_uvRangeY,"float4", sizeof(_float4) });
    }
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