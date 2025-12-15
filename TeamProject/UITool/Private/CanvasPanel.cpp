#include "pch.h"
#include "CanvasPanel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

#include "TestUI.h"
#include "ImageUI.h"
#include "TextUI.h"

_uint CCanvasPanel::m_iCount = {};

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
    Render_GUI_Layout();

    Render_GUI_Transform();

    ImGui::SeparatorText("Create");

    _bool isCreateChild = {};
    string strProtoTag;
    string strInstanceKey;

    if (ImGui::Button("Create Image"))
    {
        isCreateChild = true;
        strProtoTag = "Proto_GameObject_ImageUI";
        strInstanceKey = "UI_ImageUI" + to_string(CImageUI::m_iCount++);
    }

    if (ImGui::Button("Create Text"))
    {
        isCreateChild = true;
        strProtoTag = "Proto_GameObject_TextUI";
        strInstanceKey = "UI_TextUI" + to_string(CTextUI::m_iCount++);
    }

    if(isCreateChild)
    {
        CUI_Object* pChild = Builder::Create_UIObject({ "UITool_Level" , strProtoTag })
            .Size({ m_fChildCreateSize.x, m_fChildCreateSize.y })
            .Build(strInstanceKey);

        if (!pChild)
            return;

        IUI_Service* pUIService = CGameInstance::GetInstance()->Get_UIMgr();
        pUIService->Add_UIObject(pChild, "UITool_Level");
        Get_Component<CObjectContainer>()->Add_Child(pChild);
    }
}

json CCanvasPanel::ToJson()
{
    json objData;

    objData["typeTag"] = "CanvasPanel";

    ToJson_Common(objData);

    return objData;
}

void CCanvasPanel::FromJson(const json& data)
{
    if (data.contains("children"))
    {
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
