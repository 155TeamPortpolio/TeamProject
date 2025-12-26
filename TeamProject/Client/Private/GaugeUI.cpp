#include "pch.h"
#include "GaugeUI.h"

#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"

CGaugeUI::CGaugeUI()
{
}

CGaugeUI::CGaugeUI(const CGaugeUI& rhs)
    : CUI_Object(rhs)
{
}

HRESULT CGaugeUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CGaugeUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    return S_OK;
}

void CGaugeUI::Priority_Update(_float dt)
{
}

void CGaugeUI::Update(_float dt)
{
    if (!m_isAlive)
        return;

    Play_Animation(dt);
}

void CGaugeUI::Late_Update(_float dt)
{
}

void CGaugeUI::Render_GUI()
{
    __super::Render_GUI();
}

void CGaugeUI::ReadElementData(const UI_ELEMENT_DATA& data)
{
    // 공통 데이터 읽기
    m_eAnchor = static_cast<ANCHOR>(data.transform.iAnchor);
    m_vAnchorOffset = _float2(data.transform.vAnchorOffset[0], data.transform.vAnchorOffset[1]);
    m_vSize = _float2(data.transform.vSize[0], data.transform.vSize[1]);
    m_vScale = _float2(data.transform.vScale[0], data.transform.vScale[1]);
    m_vPivot = _float2(data.transform.vPivot[0], data.transform.vPivot[1]);
    m_fRadian = data.transform.fRadian;
    m_vColor = _float4(data.vColor[0], data.vColor[1], data.vColor[2], data.vColor[3]);

    // 애니메이션 데이터 읽기
    for (auto& clipData : data.animClips)
    {
        UI_ANIM_CLIP clip = { clipData.strName };
        clip.fDuration = clipData.fDuration;
        clip.isLoop = clipData.isLoop;

        for (auto& keyframeData : clipData.keyframes)
        {
            UI_KEYFRAME keyframe = {};

            keyframe.fTime = keyframeData.fTime;
            keyframe.vScale = { keyframeData.vScale[0], keyframeData.vScale[1] };
            keyframe.fAngle = keyframeData.fAngle;
            keyframe.vPosition = { keyframeData.vPosition[0], keyframeData.vPosition[1] };
            keyframe.vColor = { keyframeData.vColor[0], keyframeData.vColor[1], keyframeData.vColor[2], keyframeData.vColor[3] };
            keyframe.easeType = static_cast<EaseType>(keyframeData.uEaseType);

            clip.keyframes.push_back(keyframe);
        }
        m_AnimClips.push_back(clip);
    }

    // 자식 데이터 읽기
    CObjectContainer* pContainer = Get_Component<CObjectContainer>();
    if (pContainer)
    {
        for (auto& childElementData : data.children)
        {
            const string& strCurrentLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
            const string& strTypeTag = childElementData.strTypeTag;
            CUI_Object* pChildObj = Builder::Create_UIObject({ strCurrentLevelKey , "Proto_GameObject_" + strTypeTag })
                .Build(strTypeTag);

            if (!pChildObj)
                continue;

            pChildObj->ReadElementData(childElementData);

            pContainer->Add_Child(pChildObj);
        }
    }

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, data.strTextureTag);
}

CGameObject* CGaugeUI::Create()
{
    CGaugeUI* pInstance = new CGaugeUI();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CGaugeUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGaugeUI::Clone(INIT_DESC* pArg)
{
    CGaugeUI* pInstance = new CGaugeUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CGaugeUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGaugeUI::Free()
{
    __super::Free();
}