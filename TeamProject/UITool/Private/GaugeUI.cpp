#include "pch.h"
#include "GaugeUI.h"

#include "Sprite2D.h"
#include "UITool_Level.h"

_uint CGaugeUI::m_iCount = {};
const string CGaugeUI::m_strTypeTag = "Gauge";

CGaugeUI::CGaugeUI()
{
}

CGaugeUI::CGaugeUI(const CGaugeUI& rhs)
	: CUIObject_Tool(rhs)
{
}

HRESULT CGaugeUI::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CGaugeUI::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    if (m_isRadial)    
        Get_Component<CSprite2D>()->ChangePass("RadialFill");
    else                
        Get_Component<CSprite2D>()->ChangePass("LinearFill");

    Get_Component<CSprite2D>()->Set_Param("FillAmount", { &m_fFillAmount,"float",sizeof(_float) });

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    if (szTextureKeys.size())
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);

    m_iCount++;

	return S_OK;
}

void CGaugeUI::Priority_Update(_float dt)
{
}

void CGaugeUI::Update(_float dt)
{
	if (!m_isAlive)
		return;

    Play_Animation(dt);
}

void CGaugeUI::Late_Update(_float dt)
{
}

void CGaugeUI::Render_GUI()
{
    __super::Render_GUI();

    // 이미지
    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    ImGui::Text(m_InstanceName.c_str());
    ImGui::SeparatorText(u8"이미지");
    ImGui::SetNextWindowSizeConstraints(ImVec2(300.f, 0), ImVec2(300.f, 200.f));
    if (ImGui::Combo(u8"이미지##메인", &m_iTextureKeyIndex, szTextureKeys.data(), szTextureKeys.size()))
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);

    // 게이지
    if (ImGui::Checkbox(u8"원형", &m_isRadial))
    {
        if (m_isRadial)
            Get_Component<CSprite2D>()->ChangePass("RadialFill");
        else
            Get_Component<CSprite2D>()->ChangePass("LinearFill");
    }

    if(ImGui::DragFloat(u8"게이지", &m_fFillAmount, 0.01f, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
        Get_Component<CSprite2D>()->Set_Param("FillAmount", { &m_fFillAmount,"float",sizeof(_float) });

    if (ImGui::DragFloat(u8"방향 (0 또는 1)", &m_fDirection, 1.f, 0.f, 1.f, "%.f", ImGuiSliderFlags_AlwaysClamp))
        Get_Component<CSprite2D>()->Set_Param("Direction", { &m_fDirection,"float",sizeof(_float) });
}

void CGaugeUI::FillElementData(UI_ELEMENT_DATA& data)
{
    __super::FillElementData(data);

    data.strTypeTag = m_strTypeTag;

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    data.strTextureTag = szTextureKeys[m_iTextureKeyIndex];

    data.isRadial = m_isRadial;
    data.fDirection = m_fDirection;
    data.fFillAmount = m_fFillAmount;
}

void CGaugeUI::ReadElementData(const UI_ELEMENT_DATA& data)
{
    __super::ReadElementData(data);

    const auto& szTextureKeys = CUITool_Level::m_szTextureKeys;
    m_iTextureKeyIndex = Find_TextureIndex(szTextureKeys, data.strTextureTag);
    if (-1 != m_iTextureKeyIndex)
        Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, szTextureKeys[m_iTextureKeyIndex]);

    m_isRadial = data.isRadial;
    m_fDirection = data.fDirection;
    m_fFillAmount = data.fFillAmount;

    if (m_isRadial) 
        Get_Component<CSprite2D>()->ChangePass("RadialFill");
    else            
        Get_Component<CSprite2D>()->ChangePass("LinearFill"); 
    Get_Component<CSprite2D>()->Set_Param("Direction", { &m_fDirection,"float",sizeof(_float) });
    Get_Component<CSprite2D>()->Set_Param("FillAmount", { &m_fFillAmount,"float",sizeof(_float) });
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

void CGaugeUI::Free()
{
    __super::Free();
}