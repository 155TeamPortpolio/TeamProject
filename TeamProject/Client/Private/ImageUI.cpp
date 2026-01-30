#include "pch.h"
#include "ImageUI.h"
// Engine
#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CImageUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CImageUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    return S_OK;
}

void CImageUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    auto sprite = Get_Component<CSprite2D>();

    sprite->Change_Texture(0, G_GlobalLevelKey, data.value("textureTag", ""));

    m_colorTextureKey = data.value("colorTexKey", string("empty.png"));
    m_colorTexModeU = (_uint)data.value("colorTexMode", 0u);
    m_colorTexMix = (_float)data.value("colorTexMix", 1.f);

    sprite->Set_Param("ColorTexMode", {&m_colorTexModeU, "uint", sizeof(_uint)});
    sprite->Set_Param("ColorTexMix", {&m_colorTexMix, "float", sizeof(_float)});

    sprite->Change_Texture(1, G_GlobalLevelKey, m_colorTextureKey);
}

CGameObject* CImageUI::Create()
{
    CImageUI* pInstance = new CImageUI();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CImageUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CImageUI::Clone(INIT_DESC* pArg)
{
    CImageUI* pInstance = new CImageUI(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CImageUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}