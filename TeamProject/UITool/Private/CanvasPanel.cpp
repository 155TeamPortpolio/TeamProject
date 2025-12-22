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
    Render_GUI_Layout();
    
    Render_GUI_Transform();

    Render_GUI_Create();

    Render_GUI_SavePrefab();

    __super::Render_GUI();
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
    string strProtoTag = "Proto_GameObject_";
    string strInstanceKey;

    if (ImGui::Button("Create Image"))
    {
        isCreateChild = true;
        strProtoTag += CImageUI::m_strTypeTag;
        strInstanceKey = CImageUI::m_strTypeTag + to_string(CImageUI::m_iCount);
    }

    if (ImGui::Button("Create Text"))
    {
        isCreateChild = true;
        strProtoTag += CTextUI::m_strTypeTag;
        strInstanceKey = CTextUI::m_strTypeTag + to_string(CTextUI::m_iCount);
    }

    if (ImGui::Button("Create Button"))
    {
        isCreateChild = true;
        strProtoTag += CButtonUI::m_strTypeTag;
        strInstanceKey = CButtonUI::m_strTypeTag + to_string(CButtonUI::m_iCount);
    }

    if (ImGui::Button("Create SpriteAnimation"))
    {
        isCreateChild = true;
        strProtoTag += CSpriteAnimationUI::m_strTypeTag;
        strInstanceKey = CSpriteAnimationUI::m_strTypeTag + to_string(CSpriteAnimationUI::m_iCount);
    }

    if (ImGui::Button("Create UVAnimationUI"))
    {
        isCreateChild = true;
        strProtoTag += CUVAnimationUI::m_strTypeTag;
        strInstanceKey = CUVAnimationUI::m_strTypeTag + to_string(CUVAnimationUI::m_iCount);
    }

    if (ImGui::Button("Create Gauge"))
    {
        isCreateChild = true;
        strProtoTag += CGaugeUI::m_strTypeTag;
        strInstanceKey = CGaugeUI::m_strTypeTag + to_string(CGaugeUI::m_iCount);
    }

    // 자식 생성
    if (isCreateChild)
    {
        string strCurrentLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

        CUI_Object* pChild = Builder::Create_UIObject({ strCurrentLevelKey, strProtoTag })       // UI Object 생성
            .Size({ m_fChildCreateSize.x, m_fChildCreateSize.y })
            .Build(strInstanceKey);

        if (!pChild)
            return;

        CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pChild, strCurrentLevelKey);    // UI Manager에 추가
        this->Get_Component<CObjectContainer>()->Add_Child(pChild);                             // 컨테이너에 자식 추가
    }
}

void CCanvasPanel::Render_GUI_SavePrefab()
{
    ImGui::SeparatorText("Save as Prefab");

    if (ImGui::Button("Save"))
    {
        json data;

        // 부모 정보 저장
        SavePrefab(data["parent"]);

        //// 자식 정보 저장
        //const vector<CGameObject*> Childeren = Get_Component<CObjectContainer>()->Get_Children();
        //
        //for (auto& pChild : Childeren)
        //{
        //    if (!pChild)
        //        continue;
        //
        //    CUIObject_Tool* pUI = dynamic_cast<CUIObject_Tool*>(pChild);
        //
        //    if (!pUI)
        //        continue;
        //
        //    json objData;
        //    pUI->SavePrefab(objData);
        //    data["parent"]["children"].push_back(objData);
        //}

        // 프리팹 이름으로 파일명 바꾸고
        std::ofstream outputFile(Helper::SaveFileDialogByWinAPI(CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(), "json"));

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