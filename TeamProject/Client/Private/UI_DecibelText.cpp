#include "pch.h"
#include "UI_DecibelText.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "TextSlot.h"

HRESULT CUI_DecibelText::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_DecibelText::Initialize(INIT_DESC* pArg)
{
    TEXT_DESC* pDesc = static_cast<TEXT_DESC*>(pArg);
    m_pState = pDesc->pState;
    m_pColor = pDesc->pColor;

    __super::Initialize(pArg);

    Ready_PartObjects();

    return S_OK;
}

void CUI_DecibelText::Update(_float dt)
{
    if (m_handle.isValid())
        m_handle.Get()->Set_Color(*m_pColor);
}

void CUI_DecibelText::Ready_PartObjects()
{
    CUI_Object* pBg = Builder::Create_UIObject({ CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(), "Proto_GameObject_Image" })
        .Build("textBg");

    CUI_Object* pPts = Builder::Create_UIObject({ CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(), "Proto_GameObject_Text" })
        .Build("text");

    if (pPts)
    {
        if (auto pText = pPts->Get_Component<CTextSlot>())
        {
            pText->Set_Text(L"ÄÞº¸");
        }

        pPts->Set_Color(Helper::HexToColor("#FFFF3E"));
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

    if (pBg)
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

CGameObject* CUI_DecibelText::Create()
{
    CUI_DecibelText* pInstance = new CUI_DecibelText();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_DecibelText");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_DecibelText::Clone(INIT_DESC* pArg)
{
    CUI_DecibelText* pInstance = new CUI_DecibelText(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_DecibelText");
        Safe_Release(pInstance);
    }
    return pInstance;
}