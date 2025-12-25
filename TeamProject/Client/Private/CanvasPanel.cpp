#include "pch.h"
#include "CanvasPanel.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

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
    Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "BangbooRunActivityIcon01.png");
#endif

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

void CCanvasPanel::ReadElementData(const UI_ELEMENT_DATA& data)
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