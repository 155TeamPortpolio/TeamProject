#include "pch.h"
#include "GaugeUI.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "Sprite2D.h"
#include "UITool_Level.h"

_uint CGaugeUI::m_iCount = {};
const string CGaugeUI::m_strTypeTag = "Gauge";

HRESULT CGaugeUI::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CGaugeUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Set_OriginTexSize(true);

    auto sprite = Get_Component<CSprite2D>();

    sprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    if (m_isRadial) sprite->ChangePass("RadialFill");
    else            sprite->ChangePass("LinearFill");

    sprite->Set_Param("FillAmount", {&m_fFillAmount, "float", sizeof(_float)});
    sprite->Set_Param("Direction",  {&m_fDirection,  "float", sizeof(_float)});

    m_strTextureKey = "empty.png";
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, true);

    m_iCount++;

    return S_OK;
}

void CGaugeUI::Update(_float dt)
{
	if (!m_isAlive) return;

    Play_Animation(dt);
}

void CGaugeUI::Render_GUI()
{
    __super::Render_GUI();
     
    Render_GUI_Image(m_strTextureKey);

    auto sprite = Get_Component<CSprite2D>();

    // 게이지
    ImGui::SeparatorText(u8"게이지?"); 
    if (ImGui::Checkbox(u8"원형", &m_isRadial))
    {
        if (m_isRadial) sprite->ChangePass("RadialFill");
        else            sprite->ChangePass("LinearFill");
    }

    if(ImGui::DragFloat(u8"채움 정도", &m_fFillAmount, 0.01f, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
        sprite->Set_Param("FillAmount", { &m_fFillAmount,"float", sizeof(_float) });

    if (ImGui::DragFloat(u8"방향 (0 - 왼쪽 / 1 - 오른쪽)", &m_fDirection, 1.f, 0.f, 1.f, "%.f", ImGuiSliderFlags_AlwaysClamp))
        sprite->Set_Param("Direction",  { &m_fDirection, "float", sizeof(_float) });
}

void CGaugeUI::Save(nlohmann::ordered_json& data)
{
    __super::Save(data);

    data["typeTag"] = m_strTypeTag;
    data["textureTag"] = m_strTextureKey;

    auto& gaugeJson = data["gauge"];
    gaugeJson["radial"] = m_isRadial;
    gaugeJson["direction"] = m_fDirection;
    gaugeJson["fillAmount"] = m_fFillAmount;
}

void CGaugeUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    m_strTextureKey = data.value("textureTag", "");

    if (data.contains("gauge"))
    {
        const auto& gaugeJson = data["gauge"];
        m_isRadial = gaugeJson.value("radial", false);
        m_fDirection = gaugeJson.value("direction", 0.0f);
        m_fFillAmount = gaugeJson.value("fillAmount", 1.0f);
    }

    auto pSprite = Get_Component<CSprite2D>();

    pSprite->Change_Texture(0, G_GlobalLevelKey, m_strTextureKey);

    if (m_isRadial) pSprite->ChangePass("RadialFill");
    else            pSprite->ChangePass("LinearFill");

    pSprite->Set_Param("Direction", { &m_fDirection,  "float", sizeof(_float) });
    pSprite->Set_Param("FillAmount", { &m_fFillAmount, "float", sizeof(_float) });
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