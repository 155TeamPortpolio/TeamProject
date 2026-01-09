#include "pch.h"
#include "GaugeUI.h"

#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"

HRESULT CGaugeUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CEventListener>();

    return S_OK;
}

HRESULT CGaugeUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CEventListener>()->Add_Listner<GAUGE_DESC>([&](const GAUGE_DESC& desc) 
        { 
            if(desc.owner == m_eOwner && 
            desc.type == m_eType)   
            Set_Gauge(desc); 
        });

    return S_OK;
}

void CGaugeUI::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CSprite2D>()->Set_Param("FillAmount", {&m_fFillAmount, "float", sizeof(_float)});
}

void CGaugeUI::Render_GUI()
{
    __super::Render_GUI();
    
    auto pSprite = Get_Component<CSprite2D>();

    ImGui::Text(u8"게이지 방향 : ");
    ImGui::SameLine();
    (m_fDirection == 0) ? ImGui::Text(u8"오른쪽에서 왼쪽으로") : ImGui::Text(u8"왼쪽에서 오른쪽으로");

    if (ImGui::DragFloat("FillAmount", &m_fFillAmount, 0.01f, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
        pSprite->Set_Param("FillAmount", { &m_fFillAmount,"float", sizeof(_float) });
}
void CGaugeUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    auto pSprite = Get_Component<CSprite2D>();
    pSprite->Change_Texture(0, G_GlobalLevelKey, data.value("textureTag", ""));

    if (data.contains("gauge"))
    {
        const auto& gaugeJson = data["gauge"];

        if(gaugeJson.value("radial", 0.f))
            pSprite->ChangePass("RadialFill");
        else
            pSprite->ChangePass("LinearFill");

        m_fDirection = gaugeJson.value("direction", 0.0f);
        pSprite->Set_Param("Direction", { &m_fDirection, "float", sizeof(_float) });
    }
}

void CGaugeUI::Set_Gauge(const GAUGE_DESC& desc)
{
    m_fFillAmount = desc.state.fCurrent / max(desc.state.fMax, 1.f);
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