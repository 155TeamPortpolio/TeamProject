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

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "Bangboo.jpg");

    CUI_Object* pImage = Builder::Create_UIObject({ "UITool_Level" ,"Proto_GameObject_ImageUI" })
        .Scale({ 1.f, 1.f })
        .Build("UI_ImageUI");

    Get_Component<CObjectContainer>()->Add_Child(pImage);

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
    ImGui::SeparatorText("Create Child");

    if (ImGui::Button("Image"))
    {
        CUI_Object* pImage = Builder::Create_UIObject({ "UITool_Level" ,"Proto_GameObject_ImageUI" })
            .Scale({ 1.f, 1.f })
            .Build("UI_ImageUI");

        if (!pImage)
            return;

        IUI_Service* pUIService = CGameInstance::GetInstance()->Get_UIMgr();
        pUIService->Add_UIObject(pImage, "UITool_Level");
        Get_Component<CObjectContainer>()->Add_Child(pImage);
    }

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
