#include "pch.h"
#include "CanvasPanel.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

#include "ImageUI.h"
#include "TextUI.h"
#include "ButtonUI.h"
#include "SpriteAnimationUI.h"
#include "UVAnimationUI.h"
#include "GaugeUI.h"

_uint CCanvasPanel::m_iCount = {};
const string CCanvasPanel::m_strTypeTag = "CanvasPanel";

CCanvasPanel::CCanvasPanel()
{
}

CCanvasPanel::CCanvasPanel(const CCanvasPanel& rhs)
    : CUIObject_Tool(rhs)
{
}

HRESULT CCanvasPanel::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CCanvasPanel::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

#ifdef _DEBUG
    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "PanelBox.dds");
#endif

    m_iCount++;
    
    return S_OK;
}

void CCanvasPanel::Priority_Update(_float dt)
{
    Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CCanvasPanel::Update(_float dt)
{
    if (!m_isAlive)
        return;

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    Play_Animation(dt);
}

void CCanvasPanel::Late_Update(_float dt)
{
    Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

void CCanvasPanel::Render_GUI()
{
    Render_GUI_Create();

    Render_GUI_SavePrefab();

    __super::Render_GUI();
}

void CCanvasPanel::SavePrefab(json& data)
{ 
    __super::SavePrefab(data);

    data["typeTag"] = m_strTypeTag;
}

void CCanvasPanel::LoadPrefab(const json& data)
{
    __super::LoadPrefab(data);
}

void CCanvasPanel::Render_GUI_Create()
{
    ImGui::SeparatorText("Create");

    _bool isCreateChild = {};
    string strType = {};

    if (ImGui::Button("Create Canvas"))
    {
        isCreateChild = true;
        strType = CCanvasPanel::m_strTypeTag;
    }

    if (ImGui::Button("Create Image"))
    {
        isCreateChild = true;
        strType = CImageUI::m_strTypeTag;
    }

    if (ImGui::Button("Create Text"))
    {
        isCreateChild = true;
        strType = CTextUI::m_strTypeTag;
    }

    if (ImGui::Button("Create Button"))
    {
        isCreateChild = true;
        strType = CButtonUI::m_strTypeTag;
    }

    if (ImGui::Button("Create SpriteAnimation"))
    {
        isCreateChild = true;
        strType = CSpriteAnimationUI::m_strTypeTag;
    }

    if (ImGui::Button("Create UVAnimationUI"))
    {
        isCreateChild = true;
        strType = CUVAnimationUI::m_strTypeTag;
    }

    if (ImGui::Button("Create Gauge"))
    {
        isCreateChild = true;
        strType = CGaugeUI::m_strTypeTag;
    }

    // 자식 생성
    if (isCreateChild)
    {
        string strCurrentLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

        CUI_Object* pChild = Builder::Create_UIObject({ strCurrentLevelKey, "Proto_GameObject_" + strType})       // UI Object 생성
            .Size({ m_fChildCreateSize.x, m_fChildCreateSize.y })
            .Build(strType);

        if (!pChild)
            return;

        Get_Component<CObjectContainer>()->Add_Child(pChild);                             // 컨테이너에 자식 추가
    }
}

void CCanvasPanel::Render_GUI_SavePrefab()
{
    ImGui::SeparatorText("Save as Prefab");

    if (ImGui::Button("Save"))
    {
        json data;

        SavePrefab(data["parent"]);

        std::ofstream outputFile(Helper::SaveFileDialogByWinAPI("prefab", "json"));

        if (outputFile.is_open())
        {
            outputFile << data.dump(4);
            outputFile << std::endl;
            outputFile.close();
        }
    }
}

CGameObject* CCanvasPanel::Create()
{
    CCanvasPanel* pInstance = new CCanvasPanel();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CCanvasPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCanvasPanel::Clone(INIT_DESC* pArg)
{
    CCanvasPanel* pInstance = new CCanvasPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CCanvasPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCanvasPanel::Free()
{
    __super::Free();
}