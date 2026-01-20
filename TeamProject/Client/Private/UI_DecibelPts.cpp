#include "pch.h"
#include "UI_DecibelPts.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

HRESULT CUI_DecibelPts::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_DecibelPts::Initialize(INIT_DESC* pArg)
{
    PTS_DESC* pDesc = static_cast<PTS_DESC*>(pArg);
    m_pColor = pDesc->pColor;

    __super::Initialize(pArg);

    Ready_PartObjects();

    Init_PtsObject();

    return S_OK;
}

void CUI_DecibelPts::Update(_float dt)
{
    Set_ChildColor(CHILD::PTS, *m_pColor);
}

void CUI_DecibelPts::Ready_PartObjects()
{
    auto pContainer = Get_Component<CObjectContainer>();
    const string& strLevelKey = LevelManager()->Get_NowLevelKey();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        CUI_Object* pObj = Builder::Create_UIObject({ strLevelKey, "Proto_GameObject_Image" })
            .Build("decibelKanji" + to_string(i));
        if (!pObj)
            continue;

        pContainer->Add_Child(pObj);
        m_pChildren[i] = pObj;

        if (auto pSprite = m_pChildren[i]->Get_Component<CSprite2D>())
            m_pSprites[i] = pSprite;
    }
}

void CUI_DecibelPts::Init_PtsObject()
{
    _int iIndex = ENUM(CHILD::PTS);

    auto pChild = m_pChildren[iIndex];
    auto pSprite = m_pSprites[iIndex];

    if (!pChild || !pSprite)
        return;

    pSprite->Change_Texture(0, G_GlobalLevelKey, "CombatPTS.png");
    pChild->Set_Size(_float2(m_fHeight * pSprite->Get_AspectRatio(), m_fHeight));
    pChild->Set_AnchorOffset(m_vPadding);

    // 포인트 이미지 먼저 띄우고나서 배경 사이즈 정해야해서 순서 이렇게
    Init_BgObject(pChild->Get_PxSize());
}

void CUI_DecibelPts::Init_BgObject(_float2 vSize)
{
    _int iIndex = ENUM(CHILD::BG);

    auto pChild = m_pChildren[iIndex];
    auto pSprite = m_pSprites[iIndex];

    if (!pChild || !pSprite)
        return;

    pSprite->Change_Texture(0, G_GlobalLevelKey, "CombatBgPts.png");
    pChild->Set_Size({ vSize.x + m_vPadding.x * 2.f, vSize.y + m_vPadding.y * 2.f });
    pChild->Set_Color(Helper::HexToColor("#000000"));
    Set_Size(pChild->Get_PxSize());
}

void CUI_DecibelPts::Set_ChildColor(CHILD child, _float4 vColor)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Color(vColor);
}

CGameObject* CUI_DecibelPts::Create()
{
    CUI_DecibelPts* pInstance = new CUI_DecibelPts();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_DecibelPts");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_DecibelPts::Clone(INIT_DESC* pArg)
{
    CUI_DecibelPts* pInstance = new CUI_DecibelPts(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_DecibelPts");
        Safe_Release(pInstance);
    }
    return pInstance;
}