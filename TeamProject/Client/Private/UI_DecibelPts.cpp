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

    return S_OK;
}

void CUI_DecibelPts::Update(_float dt)
{
    Set_Color();
}

void CUI_DecibelPts::Ready_PartObjects()
{
    auto pGameInstance = CGameInstance::GetInstance();
    const auto& strLevelKey = pGameInstance->Get_LevelMgr()->Get_NowLevelKey();

    CUI_Object* pPts = Builder::Create_UIObject({ strLevelKey, "Proto_GameObject_Image" }).Build("pts");
    CUI_Object* pBg = Builder::Create_UIObject({ strLevelKey, "Proto_GameObject_Image" }).Build("ptsBg"); 

    if (!pPts || !pBg)
        return;
     
    // 포인트 이미지 먼저 띄우고나서 배경 사이즈 정해야해서 순서 이렇게
    Init_PtsObject(pPts);
    Init_BgObject(pBg, pPts);

    auto pContainer = Get_Component<CObjectContainer>();
    pContainer->Add_Child(pBg);
    pContainer->Add_Child(pPts);
}

void CUI_DecibelPts::Init_PtsObject(CUI_Object* pPts)
{
    if (auto pSprite = pPts->Get_Component<CSprite2D>())
    {
        pSprite->Change_Texture(0, G_GlobalLevelKey, "CombatPTS.png");
        pPts->Set_Size(_float2(m_fHeight * pSprite->Get_AspectRatio(), m_fHeight));
    }
    pPts->Set_AnchorOffset(m_vPadding);
    m_hPts = pPts->Get_Handle();
}

void CUI_DecibelPts::Init_BgObject(CUI_Object* pBg, CUI_Object* pPts)
{
    if (auto pSprite = pBg->Get_Component<CSprite2D>())
    {
        pSprite->Change_Texture(0, G_GlobalLevelKey, "CombatBgPts.png");
    }
    const _float2 pSize = pPts->Get_PxSize();
    pBg->Set_Size({ pSize.x + m_vPadding.x * 2.f, pSize.y + m_vPadding.y * 2.f });
    pBg->Set_Color(Helper::HexToColor("#000000"));
    Set_Size(pBg->Get_PxSize());
}

void CUI_DecibelPts::Set_Color()
{
    if (m_hPts.isValid())
        m_hPts.Get()->Set_Color(*m_pColor);
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