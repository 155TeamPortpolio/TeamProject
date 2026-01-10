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
    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (m_hText.isValid())
        m_hText.Get()->Set_Color(*m_pColor);
}

void CUI_DecibelText::Ready_PartObjects()
{
    auto pGameInstance = CGameInstance::GetInstance();
    const auto& strLevelKey = pGameInstance->Get_LevelMgr()->Get_NowLevelKey();

    CUI_Object* pText = Builder::Create_UIObject({ strLevelKey, "Proto_GameObject_Text" }).Build("text");
    CUI_Object* pBg = Builder::Create_UIObject({ strLevelKey, "Proto_GameObject_Image" }).Build("textBg"); 

    if (!pText || !pBg)
        return;

    // 텍스트 먼저 띄우고나서 배경 사이즈 정해야해서 순서 이렇게
    Init_TextObject(pText);
    Init_BgObject(pBg, pText);

    auto pContainer = Get_Component<CObjectContainer>();
    pContainer->Add_Child(pBg);
    pContainer->Add_Child(pText);
}

void CUI_DecibelText::Init_TextObject(CUI_Object* pText)
{
    if (auto pTextSlot = pText->Get_Component<CTextSlot>())
    {
        pTextSlot->Set_Font("NanumSquareNeo.spritefont");
        pTextSlot->Set_Text(L"abcdefg");
        pTextSlot->Set_Size(1.f);
        //pTextSlot->Set_Color(Helper::HexToColor("#FFFFFF"));
        pTextSlot->Set_Shear(_float2(-0.4f, 0.f));

        pText->Set_Size(pTextSlot->Get_TextSize());
    }

    pText->Set_Color(Helper::HexToColor("#FFFF3E"));
    pText->Set_AnchorOffset(m_vPadding);
    m_hText = pText->Get_Handle();
}

void CUI_DecibelText::Init_BgObject(CUI_Object* pBg, CUI_Object* pText)
{
    if (auto pSprite = pBg->Get_Component<CSprite2D>())
    {
        pSprite->Change_Texture(0, G_GlobalLevelKey, "CombatBg00.png");
    } 
    const _float2 pSize = pText->Get_PxSize();
    pBg->Set_Size({ pSize.x + m_vPadding.x * 2.f, pSize.y + m_vPadding.y * 2.f });
    pBg->Set_Color(Helper::HexToColor("#000000"));
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