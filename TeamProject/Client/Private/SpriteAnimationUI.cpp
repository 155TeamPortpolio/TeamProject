#include "pch.h"
#include "SpriteAnimationUI.h"

#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"

CSpriteAnimationUI::CSpriteAnimationUI()
{
}

CSpriteAnimationUI::CSpriteAnimationUI(const CSpriteAnimationUI& rhs)
    : CUI_Object(rhs)
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

    auto pSprite = Get_Component<CSprite2D>();

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
            pSprite->Set_Param("FrameIndex", { &m_iCurrentFrameIndex,"uint",sizeof(_uint) });
        }

        pSprite->Set_Param("Col", { &m_iFrameCountX, "uint", sizeof(_uint) });
        pSprite->Set_Param("Row", { &m_iFrameCountY, "uint", sizeof(_uint) });
    }
}

void CSpriteAnimationUI::Late_Update(_float dt)
{
}

void CSpriteAnimationUI::Render_GUI()
{
    __super::Render_GUI();
}

void CSpriteAnimationUI::Load(const nlohmann::ordered_json& data)
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

    auto pSprite = Get_Component<CSprite2D>();
    pSprite->Change_Texture(0, G_GlobalLevelKey, data.value("textureTag", ""));

    if (data.contains("spriteAnimation"))
    {
        const auto& spriteAnimationJson = data["spriteAnimation"];

        m_isLoop = spriteAnimationJson.value("loop", true);
        m_iFrameCountX = spriteAnimationJson.value("frameCountX", 1);
        m_iFrameCountY = spriteAnimationJson.value("frameCountY", 1);
        m_iFrameCountTotal = spriteAnimationJson.value("frameCountTotal", 1);
        m_fFrameSpeed = spriteAnimationJson.value("frameSpeed", 30.0f); 
    }
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