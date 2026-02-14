#include "pch.h"
#include "UI_PartyCardRTDraw.h"

#include "GameInstance.h"
#include "Sprite2D.h"

HRESULT CUI_PartyCardRTDraw::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_PartyCardRTDraw::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    
    CARD_DESC* pDesc = static_cast<CARD_DESC*>(pArg);

    auto pSprite = Get_Component<CSprite2D>();
    pSprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    // ·»´õÅ¸°Ù¿¡ SRV ¹ÙÀÎµù
    SHADER_PARAM param = {};
    auto pSRV = RenderSystem()->Get_CustomTargetSRV(pDesc->strRenderTargetKey);
    param.pData = pSRV;
    param.typeName = "Texture2D";
    param.iSize = 0;
    pSprite->Set_Param("SpriteTexture", param);

    // ½ºÅÙ½Ç Å×½ºÆ®
    pSprite->ChangePass("Opaque_StencilTest");
    m_stencilMode = StencilMode::Test;

    SetRenderLayer(RENDER_LAYER::None);

    return S_OK;
}

void CUI_PartyCardRTDraw::Awake()
{
}

void CUI_PartyCardRTDraw::Update(_float dt)
{
    __super::Update(dt);
}

CGameObject* CUI_PartyCardRTDraw::Create()
{
    CUI_PartyCardRTDraw* pInstance = new CUI_PartyCardRTDraw();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_PartyCardRTDraw");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_PartyCardRTDraw::Clone(INIT_DESC* pArg)
{
    CUI_PartyCardRTDraw* pInstance = new CUI_PartyCardRTDraw(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_PartyCardRTDraw");
        Safe_Release(pInstance);
    }
    return pInstance;
}