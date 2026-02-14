#include "pch.h"
#include "UI_BattleLineupCardRT.h"

#include "GameInstance.h"
#include "Sprite2D.h"

HRESULT CUI_BattleLineupCardRT::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_BattleLineupCardRT::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    auto pSprite = Get_Component<CSprite2D>();

    pSprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    //pSprite->Add_Texture(G_GlobalLevelKey, "empty.png");

    //SHADER_PARAM param = {};
    //auto pSRV = RenderSystem()->Get_EngineTargetSRV("Target_Combined_SkinnedMesh");
    //param.pData = pSRV;
    //param.typeName = "Texture2D";
    //param.iSize = 0;
    //pSprite->Set_Param("SpriteTexture", param);

    // ·»´õÅ¸°Ù »ý¼º
    RenderTargetDesc desc = {};
    desc.Key = "avatarTest";
    desc.Width = 1600.f;// m_vSize.x;
    desc.Height = 900.f;// m_vSize.y;
    RenderSystem()->Create_RenderTarget(desc);
    
    //// ·»´õÅ¸°Ù¿¡ SRV ¹ÙÀÎµù
    SHADER_PARAM param = {};
    auto pSRV = RenderSystem()->Get_CustomTargetSRV("avatarTest");
    param.pData = pSRV;
    param.typeName = "Texture2D";
    param.iSize = 0;
    pSprite->Set_Param("SpriteTexture", param);

    pSprite->ChangePass("Opaque_StencilTest");    // ¸¶½ºÅ© ¹Ù±ù ºÎºÐ¿¡ ±×·ÁÁü
    m_stencilMode = StencilMode::Test;

    return S_OK;
}

void CUI_BattleLineupCardRT::Awake()
{
}

void CUI_BattleLineupCardRT::Update(_float dt)
{
    __super::Update(dt);
}

CGameObject* CUI_BattleLineupCardRT::Create()
{
    CUI_BattleLineupCardRT* pInstance = new CUI_BattleLineupCardRT();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_BattleLineupCardRT");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_BattleLineupCardRT::Clone(INIT_DESC* pArg)
{
    CUI_BattleLineupCardRT* pInstance = new CUI_BattleLineupCardRT(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_BattleLineupCardRT");
        Safe_Release(pInstance);
    }
    return pInstance;
}