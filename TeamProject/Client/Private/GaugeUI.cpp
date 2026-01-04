#include "pch.h"
#include "GaugeUI.h"

#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CGaugeUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CGaugeUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    return S_OK;
}

void CGaugeUI::Update(_float dt)
{
    if (!m_isAlive) return;

    Get_Component<CSprite2D>()->Set_Param("FillAmount", {&m_fFillAmount, "float", sizeof(_float)});

    Play_Animation(dt);
}

void CGaugeUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    auto pSprite = Get_Component<CSprite2D>();
    pSprite->Change_Texture(0, G_GlobalLevelKey, data.value("textureTag", ""));

    if (data.contains("gauge"))
    {
        const auto& gaugeJson = data["gauge"];

        if(gaugeJson.value("direction", 0.f))
            pSprite->ChangePass("RadialFill");
        else
            pSprite->ChangePass("LinearFill");

        _float fDirection = gaugeJson.value("direction", 0.0f);
        pSprite->Set_Param("Direction", { &fDirection, "float", sizeof(_float) });
    }
}

CGameObject* CGaugeUI::Create()
{
    CGaugeUI* pInstance = new CGaugeUI();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CGaugeUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGaugeUI::Clone(INIT_DESC* pArg)
{
    CGaugeUI* pInstance = new CGaugeUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CGaugeUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}