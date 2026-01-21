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
    auto pSprite = Get_Component<CSprite2D>();
    Get_Component<CTextSlot>()->Set_TextKey(strObjectID);
    pSprite->Set_TextKey(strObjectID);

    pSprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

#ifdef _DEBUG
    pSprite->Add_Texture(G_GlobalLevelKey, "canvas.png");
#else
    pSprite->Add_Texture(G_GlobalLevelKey, "transparent.png");
#endif

    return S_OK;
}

void CTextUI::Update(_float dt)
{
    __super::Update(dt);

    auto pTextSlot = Get_Component<CTextSlot>();

    if (pTextSlot->Is_AutoPos())
        pTextSlot->Update_Pivot(m_vLeftTop + m_vTextPivotOffset);
    else
        pTextSlot->Set_Position(m_vLeftTop);

    pTextSlot->Push_Text();
    pTextSlot->Set_Color(_float4(m_vColor.x, m_vColor.y, m_vColor.z, m_vCombinedAlpha));
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
        auto vShear = textJson.value("shear", json::array({ 0.0f, 0.0f }));
        Get_Component<CTextSlot>()->Set_Shear({vShear[0], vShear[1]});
        if (textJson.value("autoPos", true))    //지금 오토포즈에 쓸 피봇 오프셋 계산을 로드에서 하는데, 만약에 클라이언트에서 텍스트 만들면 이거 처리 해줘야
        {
            auto eAnchor = textJson.value("anchor", 0);
            pTextSlot->Enable_AutoPos(static_cast<ANCHOR>(eAnchor));

            if (eAnchor & static_cast<_uint>(ANCHOR::Left))
            {
            }
            else if (eAnchor & static_cast<_uint>(ANCHOR::Right))
                m_vTextPivotOffset.x += m_vSize.x;
            else
                m_vTextPivotOffset.x += m_vSize.x * 0.5f;

            if (eAnchor & static_cast<_uint>(ANCHOR::Top))
            {
            }
            else if (eAnchor & static_cast<_uint>(ANCHOR::Bottom))
                m_vTextPivotOffset.y += m_vSize.y;
            else
                m_vTextPivotOffset.y += m_vSize.y * 0.5f;
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