#include "pch.h"
#include "MaskUI.h"

#include "Sprite2D.h"

HRESULT CMaskUI::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CMaskUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    m_stencilMode = StencilMode::Write;

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    return S_OK;
}

void CMaskUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    m_stencilMode = StencilMode::Write;
     
    auto pSprite = Get_Component<CSprite2D>();
    const string maskTextureKey = data.value("maskTextureKey", "empty.png");
    pSprite->Change_Texture(0, G_GlobalLevelKey, maskTextureKey);
    pSprite->Set_Param("MaskThreshold", { &m_fMaskThreshold, "float", sizeof(_float) });
}

CGameObject* CMaskUI::Create()
{
    CMaskUI* pInstance = new CMaskUI();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CMaskUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMaskUI::Clone(INIT_DESC* pArg)
{
    CMaskUI* pInstance = new CMaskUI(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CMaskUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}