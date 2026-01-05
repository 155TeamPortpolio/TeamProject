#include "pch.h"
#include "UVAnimationUI.h"

#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUVAnimationUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CUVAnimationUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->ChangePass("UVAnimation");

    return S_OK;
}

void CUVAnimationUI::Update(_float dt)
{
    __super::Update(dt);

    m_vUVOffset.x += m_vUVOffsetSpeed.x * dt;
    m_vUVOffset.y += m_vUVOffsetSpeed.y * dt;
    Get_Component<CSprite2D>()->Set_Param("UVOffset", { &m_vUVOffset,"float2",sizeof(_float2) });
}

void CUVAnimationUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, data.value("textureTag", ""));

    if (data.contains("uvAnimation"))
    {
        const auto& uvAnimationJson = data["uvAnimation"];
        auto uvOffsetSpeed = uvAnimationJson.value("uvOffsetSpeed", json::array({ 0.0f, 0.0f }));
        m_vUVOffsetSpeed   = { uvOffsetSpeed[0], uvOffsetSpeed[1] };
    }
}

CGameObject* CUVAnimationUI::Create()
{
    CUVAnimationUI* pInstance = new CUVAnimationUI();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUVAnimationUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUVAnimationUI::Clone(INIT_DESC* pArg)
{
    CUVAnimationUI* pInstance = new CUVAnimationUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUVAnimationUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}