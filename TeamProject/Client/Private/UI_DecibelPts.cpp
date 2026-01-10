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
    if (m_handle.isValid())
        m_handle.Get()->Set_Color(*m_pColor);
}

void CUI_DecibelPts::Ready_PartObjects()
{
    CUI_Object* pBg = Builder::Create_UIObject({ CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(), "Proto_GameObject_Image" })
        .Build("ptsBg");

    CUI_Object* pPts = Builder::Create_UIObject({ CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(), "Proto_GameObject_Image" })
        .Build("pts");

    if (pPts)
    {
        auto pSprite = pPts->Get_Component<CSprite2D>();
        pSprite->Change_Texture(0, G_GlobalLevelKey, "CombatPTS.png");
        pPts->Set_Size(_float2(m_fHeight * pSprite->Get_AspectRatio(), m_fHeight));
        pPts->Set_AnchorOffset(m_vPadding);
        m_handle = pPts->Get_Handle();
    }

    if (pBg)
    {
        pBg->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "CombatBg00.png");
        pBg->Set_Color(Helper::HexToColor("#000000"));
        const _float2 pSize = pPts->Get_PxSize();
        pBg->Set_Size({ pSize.x + m_vPadding.x * 2.f, pSize.y + m_vPadding.y * 2.f });
    }

    if(pBg)
        Get_Component<CObjectContainer>()->Add_Child(pBg);

    if (pPts)
    {
        Get_Component<CObjectContainer>()->Add_Child(pPts);
        UI_HANDLE handle = pPts->Get_Handle();
        if (!handle.isValid())
            return;

        m_handle = handle;
    } 
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