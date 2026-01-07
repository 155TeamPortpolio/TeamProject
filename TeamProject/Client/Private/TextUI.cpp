#include "pch.h"
#include "TextUI.h"

#include "TextSlot.h"
#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CTextUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CTextSlot>();

    return S_OK;
}

HRESULT CTextUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    const string strObjectID = to_string(m_ObjectID).c_str();
    Get_Component<CSprite2D>()->Set_TextKey(strObjectID);
    Get_Component<CTextSlot>()->Set_TextKey(strObjectID);

#ifdef _DEBUG
    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "canvas.dds");
#endif

    return S_OK;
}

void CTextUI::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CTextSlot>()->Set_Position(m_vLeftTop);   
    Get_Component<CTextSlot>()->Push_Text();
}

void CTextUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    if (data.contains("text"))
    {
        auto pSprite   = Get_Component<CSprite2D>();
        auto pTextSlot = Get_Component<CTextSlot>();

        const auto& textJson = data["text"];
        
        pTextSlot->Set_Font(textJson.value("fontTag", "Asap.spritefont"));
        string strText = textJson.value("content", "content");
        pTextSlot->Set_Text(Helper::ConvertToWideString(strText));
        pTextSlot->Set_Size(textJson.value("fontScale", 1.f));
        pTextSlot->Set_Color(m_vColor);
         
        if (textJson.value("outlined", false))
        {
            auto color = textJson.value("outlineColor", json::array({ 1.0f, 1.0f, 1.0f, 1.0f }));
            pTextSlot->Set_OutLine(textJson.value("outlineThickness", 1.f), { color[0], color[1], color[2], color[3] });
        } 
    }
}

CGameObject* CTextUI::Create()
{
    CTextUI* pInstance = new CTextUI();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CTextUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTextUI::Clone(INIT_DESC* pArg)
{
    CTextUI* pInstance = new CTextUI(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CTextUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}