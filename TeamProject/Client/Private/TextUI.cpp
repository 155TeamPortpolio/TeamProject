#include "pch.h"
#include "TextUI.h"

#include "TextSlot.h"
#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"

CTextUI::CTextUI()
{
}

CTextUI::CTextUI(const CTextUI& rhs)
    : CUI_Object(rhs)
{
}

HRESULT CTextUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CTextSlot>();

    return S_OK;
}

HRESULT CTextUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

#ifdef _DEBUG
    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "PanelBox.dds");
#endif

    return S_OK;
}

void CTextUI::Priority_Update(_float dt)
{
}

void CTextUI::Update(_float dt)
{
    if (!m_isAlive)
        return;

    Get_Component<CTextSlot>()->Set_Position(m_vLeftTop);   //

    Get_Component<CTextSlot>()->Push_Text();

    Play_Animation(dt);
}

void CTextUI::Late_Update(_float dt)
{
}

void CTextUI::Render_GUI()
{
    __super::Render_GUI();
}

void CTextUI::Load(const nlohmann::ordered_json& data)
{
    if (data.empty())
        return;

    // 공통 데이터 읽기
    m_InstanceName = data.value("instanceName", "");

    // Transform 데이터 읽기
    if (data.contains("transform"))
    {
        const auto& transformJson = data["transform"];

        m_eAnchor = static_cast<ANCHOR>(transformJson.value("anchor", 0));
        auto anchorOffset = transformJson.value("anchorOffset", json::array({ 0.0f, 0.0f }));
        m_vAnchorOffset = { anchorOffset[0], anchorOffset[1] };
        auto size = transformJson.value("size", json::array({ 100.0f, 100.0f }));
        m_vSize = { size[0], size[1] };
        auto scale = transformJson.value("scale", json::array({ 1.0f, 1.0f }));
        m_vScale = { scale[0], scale[1] };
        auto pivot = transformJson.value("pivot", json::array({ 0.5f, 0.5f }));
        m_vPivot = { pivot[0], pivot[1] };
        m_fRadian = transformJson.value("radian", 0.0f);
    }

    // Color 데이터 읽기
    auto color = data.value("color", json::array({ 1.0f, 1.0f, 1.0f, 1.0f }));
    m_vColor = { color[0], color[1], color[2], color[3] };

    // 애니메이션 클립 읽기
    if (data.contains("animClips"))
    {
        const auto& animClipsJson = data["animClips"];
        m_AnimClips.clear();

        for (const auto& clipJson : animClipsJson)
        {
            UI_ANIM_CLIP clip = {};
            clip.strName = clipJson.value("name", "");
            clip.fDuration = clipJson.value("duration", 1.0f);
            clip.isLoop = clipJson.value("loop", false);

            // 키프레임 읽기
            if (clipJson.contains("keyframes"))
            {
                const auto& keyframesJson = clipJson["keyframes"];
                for (const auto& keyframeJson : keyframesJson)
                {
                    UI_KEYFRAME keyframe;
                    keyframe.fTime = keyframeJson.value("time", 0.0f);

                    auto vScale = keyframeJson.value("scale", json::array({ 1.0f, 1.0f }));
                    keyframe.vScale = { vScale[0], vScale[1] };

                    keyframe.fAngle = keyframeJson.value("angle", 0.0f);

                    auto vPosition = keyframeJson.value("position", json::array({ 0.0f, 0.0f }));
                    keyframe.vPosition = { vPosition[0], vPosition[1] };

                    auto vColor = keyframeJson.value("color", json::array({ 1.0f, 1.0f, 1.0f, 1.0f }));
                    keyframe.vColor = { vColor[0], vColor[1], vColor[2], vColor[3] };

                    keyframe.easeType = static_cast<EaseType>(keyframeJson.value("easeType", 0u));

                    clip.keyframes.push_back(keyframe);
                }
            }

            m_AnimClips.push_back(clip);
        }
    }

    // 자식 데이터 읽기
    if (data.contains("children"))
    {
        const auto& childrenJson = data["children"];
        CObjectContainer* pContainer = Get_Component<CObjectContainer>();

        for (const auto& childJson : childrenJson)
        {
            // 자식 UI 객체 생성
            string strTypeTag = childJson.value("typeTag", "");
            if (strTypeTag.empty())
                continue;

            const string& strCurrentLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
            CUI_Object* pChildObj = Builder::Create_UIObject({ strCurrentLevelKey, "Proto_GameObject_" + strTypeTag })
                .Build(strTypeTag);

            if (!pChildObj)
                continue;

            pChildObj->Load(childJson);

            if (pContainer)
                pContainer->Add_Child(pChildObj);
        }
    }

    if (data.contains("text"))
    {
        auto pSprite = Get_Component<CSprite2D>();
        auto pTextSlot = Get_Component<CTextSlot>();

        const auto& textJson = data["text"];
        
        pTextSlot->Set_Font(textJson.value("fontTag", "Asap.spritefont"));
        string strText = textJson.value("content", "content");
        pTextSlot->Set_Text(Helper::ConvertToWideString(strText));
        pTextSlot->Set_TextKey(strText);
        pSprite->Set_TextKey(strText);
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

void CTextUI::Free()
{
    __super::Free();
}