#include "pch.h"
#include "CanvasPanel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

#include "ImageUI.h"
#include "TextUI.h"
#include "ButtonUI.h"

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

    m_iCount++;
    
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
        strInstanceKey = "UI_ImageUI" + to_string(CImageUI::m_iCount);
    }

    if (ImGui::Button("Create Text"))
    {
        isCreateChild = true;
        strProtoTag = "Proto_GameObject_TextUI";
        strInstanceKey = "UI_TextUI" + to_string(CTextUI::m_iCount);
    }

    if (ImGui::Button("Create Button"))
    {
        isCreateChild = true;
        strProtoTag = "Proto_GameObject_ButtonUI";
        strInstanceKey = "UI_ButtonUI" + to_string(CButtonUI::m_iCount);
    }

    // 자식 생성
    if(isCreateChild)
    {
        string strCurrentLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

        CUI_Object* pChild = Builder::Create_UIObject({ strCurrentLevelKey, strProtoTag})       // UI Object 생성
            .Size({ m_fChildCreateSize.x, m_fChildCreateSize.y })
            .Build(strInstanceKey);

        if (!pChild)
            return;

        CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pChild, strCurrentLevelKey);    // UI Manager에 추가

        CUIObject_Tool* pUIChild = dynamic_cast<CUIObject_Tool*>(pChild);
        if (!pUIChild)
            return;
       
        Add_Child(pUIChild);    // 부모의 오브젝트 컨테이너에 자식을 추가하고, 자식에 부모 포인터와 자식 인덱스 저장
    }
}

void CCanvasPanel::ToJson(json& data)
{
    __super::ToJson(data);

    data["typeTag"] = "CanvasPanel";
}

void CCanvasPanel::FromJson(const json& data)
{
    __super::FromJson(data);
    FromJson_RefreshCount(m_iCount);    // json에서 불러올 때 카운트 새로고침
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