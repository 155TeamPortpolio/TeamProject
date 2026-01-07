#include "pch.h"
#include "SpriteAnimationUI.h"

#include "Sprite2D.h"

HRESULT CSpriteAnimationUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CSpriteAnimationUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->ChangePass("SpriteAnimation");

    return S_OK;
}

void CSpriteAnimationUI::Update(_float dt)
{
    __super::Update(dt);

    auto pSprite = Get_Component<CSprite2D>();

    if (m_isPlaying)
    {
        m_fFrameAccTime += dt;

        if (!m_isLoop && m_iCurrentFrameIndex >= m_iFrameCountTotal - 1)
        {
            m_fFrameAccTime = 0.f;
            m_iCurrentFrameIndex = 0;
            m_isPlaying = false;
            return;
        }

        if (m_fFrameAccTime >= (1.f / m_fFrameSpeed))
        {
            m_fFrameAccTime = 0.f;
            m_iCurrentFrameIndex = (m_iCurrentFrameIndex + 1) % m_iFrameCountTotal;// (m_iFrameCountX * m_iFrameCountY);
            pSprite->Set_Param("FrameIndex", { &m_iCurrentFrameIndex,"uint",sizeof(_uint) });
        }

        pSprite->Set_Param("Col", { &m_iFrameCountX, "uint", sizeof(_uint) });
        pSprite->Set_Param("Row", { &m_iFrameCountY, "uint", sizeof(_uint) });
    }
}

void CSpriteAnimationUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, data.value("textureTag", ""));

    if (data.contains("spriteAnimation"))
    {
        const auto& spriteAnimationJson = data["spriteAnimation"];

        m_isLoop           = spriteAnimationJson.value("loop", true);
        m_iFrameCountX     = spriteAnimationJson.value("frameCountX", 1);
        m_iFrameCountY     = spriteAnimationJson.value("frameCountY", 1);
        m_iFrameCountTotal = spriteAnimationJson.value("frameCountTotal", 1);
        m_fFrameSpeed      = spriteAnimationJson.value("frameSpeed", 30.0f); 
    }
}

CGameObject* CSpriteAnimationUI::Create()
{
    CSpriteAnimationUI* pInstance = new CSpriteAnimationUI();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CSpriteAnimationUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSpriteAnimationUI::Clone(INIT_DESC* pArg)
{
    CSpriteAnimationUI* pInstance = new CSpriteAnimationUI(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CSpriteAnimationUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}