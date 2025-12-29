#include "pch.h"
#include "SpriteAnimationUI.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "Sprite2D.h"
#include "UITool_Level.h"

_uint CSpriteAnimationUI::m_iCount = {};
const string CSpriteAnimationUI::m_strTypeTag = "SpriteAnimation";

CSpriteAnimationUI::CSpriteAnimationUI()
{
}

CSpriteAnimationUI::CSpriteAnimationUI(const CSpriteAnimationUI& rhs)
	: CUIObject_Tool(rhs)
{
}

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
    Get_Component<CSprite2D>()->Set_Param("FrameIndex", { &m_iCurrentFrameIndex,"uint",sizeof(_uint) });
    Get_Component<CSprite2D>()->Set_Param("Col", { &m_iFrameCountX,"uint",sizeof(_uint) });
    Get_Component<CSprite2D>()->Set_Param("Row", { &m_iFrameCountY,"uint",sizeof(_uint) });

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "empty.png");

    m_iCount++;

	return S_OK;
}

void CSpriteAnimationUI::Priority_Update(_float dt)
{
}

void CSpriteAnimationUI::Update(_float dt)
{
    if (!m_isAlive)
        return;

    Play_Animation(dt);

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
            Get_Component<CSprite2D>()->Set_Param("FrameIndex", { &m_iCurrentFrameIndex,"uint",sizeof(_uint) });             
        }
    } 
}

void CSpriteAnimationUI::Late_Update(_float dt)
{
}

void CSpriteAnimationUI::Render_GUI()
{
    __super::Render_GUI();
     
    // 텍스쳐
    ImGui::SeparatorText(u8"이미지");
    if (ImGui::Button(u8"선택"))
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
    Get_Component<CSprite2D>()->Render_GUI();

    // 스프라이트 애니메이션
    ImGui::SeparatorText(u8"스프라이트 애니메이션");
    if (ImGui::Button(m_isPlaying ? u8"정지" : u8"재생"))
    {
        m_isPlaying = !m_isPlaying;
        if (!m_isPlaying)
        {
            m_fFrameAccTime = 0.f;
            m_iCurrentFrameIndex = 0;
        }
    }

    ImGui::Checkbox(u8"루프", &m_isLoop);
    static _bool isCustomCount = { false };
    if (ImGui::Checkbox(u8"총 개수 바꾸기", &isCustomCount))
    {
        if(!isCustomCount)
            m_iFrameCountTotal = m_iFrameCountX * m_iFrameCountY;
    }
    ImGui::BeginDisabled(!isCustomCount);
    static _uint iTotalMin = m_iFrameCountX * (m_iFrameCountY -1);
    ImGui::DragScalar(u8"총 개수", ImGuiDataType_U32, &m_iFrameCountTotal, 1.f, &iTotalMin, NULL, "%u", ImGuiSliderFlags_AlwaysClamp);
    ImGui::EndDisabled();

    _bool isChanged = {};
    static _uint iMin = 1;
    isChanged |= ImGui::DragScalar(u8"가로 개수", ImGuiDataType_U32, &m_iFrameCountX, 1.f, &iMin, NULL, "%u", ImGuiSliderFlags_AlwaysClamp);
    isChanged |= ImGui::DragScalar(u8"세로 개수", ImGuiDataType_U32, &m_iFrameCountY, 1.f, &iMin, NULL, "%u", ImGuiSliderFlags_AlwaysClamp);
    if (isChanged)
    {
        Get_Component<CSprite2D>()->Set_Param("Col", { &m_iFrameCountX,"uint",sizeof(_uint) });
        Get_Component<CSprite2D>()->Set_Param("Row", { &m_iFrameCountY,"uint",sizeof(_uint) });
        iTotalMin = m_iFrameCountX * (m_iFrameCountY - 1);
        if (!isCustomCount)
            m_iFrameCountTotal = m_iFrameCountX * m_iFrameCountY;
    }     

    ImGui::DragFloat(u8"재생 속도", &m_fFrameSpeed, 1.f, 1.f, 120.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
}

void CSpriteAnimationUI::FillElementData(UI_ELEMENT_DATA& data)
{
    __super::FillElementData(data);

    data.strTypeTag = m_strTypeTag;

    data.strTextureTag = m_strTextureKey;

    data.isLoop = m_isLoop;
    data.iFrameCountX = m_iFrameCountX;
    data.iFrameCountY = m_iFrameCountY;
    data.iFrameCountTotal = m_iFrameCountTotal; 
    data.fFrameSpeed = m_fFrameSpeed;
}

void CSpriteAnimationUI::ReadElementData(const UI_ELEMENT_DATA& data)
{
    __super::ReadElementData(data);

    m_strTextureKey = data.strTextureTag;
    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, m_strTextureKey);

    m_isLoop = data.isLoop;
    m_iFrameCountX = data.iFrameCountX;
    m_iFrameCountY = data.iFrameCountY;
    m_iFrameCountTotal = data.iFrameCountTotal;
    m_fFrameSpeed = data.fFrameSpeed;
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

void CSpriteAnimationUI::Free()
{
    __super::Free();
}