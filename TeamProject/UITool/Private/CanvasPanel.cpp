#include "pch.h"
#include "CanvasPanel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

#include "TestUI.h"

CCanvasPanel::CCanvasPanel()
{
}

CCanvasPanel::CCanvasPanel(const CCanvasPanel& rhs)
    : CUI_Object(rhs)
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
    Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "CanvasPanel.png");
#endif

    return S_OK;
}

void CCanvasPanel::Priority_Update(_float dt)
{
    Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CCanvasPanel::Update(_float dt)
{
    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CCanvasPanel::Late_Update(_float dt)
{
    Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

void CCanvasPanel::Render_GUI()
{
    ImGui::SeparatorText("Create UI");
    _bool isCreateChild = {};
    string strProtoTag;
    string strInstanceKey;
    if (ImGui::Button("Create Image"))
    {
        isCreateChild = true;
        strProtoTag = "Proto_GameObject_ImageUI";
        strInstanceKey = "UI_ImageUI";       
    }
    if (ImGui::Button("Create Text"))
    {
        isCreateChild = true;
        strProtoTag = "Proto_GameObject_TextUI";
        strInstanceKey = "UI_TextUI";
    }

    if(isCreateChild)
    {
        CUI_Object* pChild = Builder::Create_UIObject({ "UITool_Level" , strProtoTag })
            .Scale({ m_fChildCreateSize.x, m_fChildCreateSize.y })
            .Build(strInstanceKey);

        if (!pChild)
            return;

        IUI_Service* pUIService = CGameInstance::GetInstance()->Get_UIMgr();
        pUIService->Add_UIObject(pChild, "UITool_Level");
        Get_Component<CObjectContainer>()->Add_Child(pChild);
    }

    ImGui::SeparatorText("Layout");
    ImGui::DragFloat("X Position", &m_fLocalX, 1.f);    // x, y 움직이면 자식 중에 이미지는 움직이는데 텍스트는 안 움직임 (Post~ 함수는 호출하는데 fontposition은 업데이트 안 해줘서)
    ImGui::DragFloat("Y Position", &m_fLocalY, 1.f);
    ImGui::DragFloat("X Size", &m_fSizeX, 1.f);
    ImGui::DragFloat("Y Size", &m_fSizeY, 1.f);

    //CGameObject::Render_GUI();
    __super::Render_GUI();
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
