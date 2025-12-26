#include "pch.h"
#include "GaugeUI.h"

#include "GameInstance.h"
#include "Helper_Func.h"
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

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "empty.png");

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

    // �ؽ���
    ImGui::SeparatorText(u8"�̹���");
    if (ImGui::Button(u8"����"))
    {
        string filePath = Helper::OpenFile_Dialogue();
        if (!filePath.empty())
        {
            string fileName = Helper::GetFileNameWithExtension(filePath);

            CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(fileName, filePath);
            m_strTextureKey = fileName;
            Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, m_strTextureKey);
        }
    }

    ImGui::SeparatorText(u8"������");
    // ������
    if (ImGui::Checkbox(u8"����", &m_isRadial))
    {
        if (m_isRadial)
            Get_Component<CSprite2D>()->ChangePass("RadialFill");
        else
            Get_Component<CSprite2D>()->ChangePass("LinearFill");
    }

    if(ImGui::DragFloat(u8"������", &m_fFillAmount, 0.01f, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
        Get_Component<CSprite2D>()->Set_Param("FillAmount", { &m_fFillAmount,"float",sizeof(_float) });

    if (ImGui::DragFloat(u8"���� (0 �Ǵ� 1)", &m_fDirection, 1.f, 0.f, 1.f, "%.f", ImGuiSliderFlags_AlwaysClamp))
        Get_Component<CSprite2D>()->Set_Param("Direction", { &m_fDirection,"float",sizeof(_float) });
}

void CGaugeUI::FillElementData(UI_ELEMENT_DATA& data)
{
    __super::FillElementData(data);

    data.strTypeTag = m_strTypeTag;

    data["textureTag"] = m_strTextureKey;

    data.isRadial = m_isRadial;
    data.fDirection = m_fDirection;
    data.fFillAmount = m_fFillAmount;
}

void CGaugeUI::ReadElementData(const UI_ELEMENT_DATA& data)
{
    __super::ReadElementData(data);

    m_strTextureKey = data["textureTag"];
    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, m_strTextureKey);

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