#include "pch.h"
#include "UI_DecibelDigits.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "Texture.h"

const string CUI_DecibelDigits::DIGIT_TEXTURES[ENUM(DigitTexture::END)] = { "00.png", "01.png", "02.png", "03.png", "04.png", "05.png", "06.png", "07.png", "08.png", "09.png" };
CUI_DecibelDigits::DigitSlot CUI_DecibelDigits::digitOrder[] = { DigitSlot::DIGIT_1000, DigitSlot::DIGIT_100, DigitSlot::DIGIT_10, DigitSlot::DIGIT_1 };

void CUI_DecibelDigits::Set_Digits(_int iDecibel)
{
    Set_Digit(ChildSlot::DIGIT_1000, static_cast<DigitTexture>(Helper::Get_Digit(iDecibel, 3)));
    Set_Digit(ChildSlot::DIGIT_100, static_cast<DigitTexture>(Helper::Get_Digit(iDecibel, 2)));
    Set_Digit(ChildSlot::DIGIT_10, static_cast<DigitTexture>(Helper::Get_Digit(iDecibel, 1)));
    Set_Digit(ChildSlot::DIGIT_1, static_cast<DigitTexture>(Helper::Get_Digit(iDecibel, 0)));

    Set_LayoutDigits();
    Set_LayoutBg();
}

HRESULT CUI_DecibelDigits::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CUI_DecibelDigits::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Add_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(ChildSlot::END); ++i)
    {
        CUI_Object* pSlot = Builder::Create_UIObject({ CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(), "Proto_GameObject_Image" })
            .Build("decibelNumber" + to_string(i));

        Get_Component<CObjectContainer>()->Add_Child(pSlot);
    }

    auto pBg = Get_Slot(ChildSlot::BG);
    if (pBg)
    {
        pBg->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "CombatBg00.png");
        pBg->Set_Color(_float4(0.f, 0.f, 0.f, 1.f));
    }

    return S_OK;
}

void CUI_DecibelDigits::Update(_float dt)
{
}

void CUI_DecibelDigits::Set_Digit(ChildSlot slot, DigitTexture texture)
{
    auto pSlot = Get_Slot(slot);
    if (!pSlot)
        return;

    pSlot->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, DIGIT_TEXTURES[ENUM(texture)]);

    pSlot->Set_Size(_float2(m_fHeight * pSlot->Get_Component<CSprite2D>()->Get_AspectRatio(), m_fHeight));
}

void CUI_DecibelDigits::Set_LayoutBg()
{
    auto pContainer = Get_Component<CObjectContainer>();

    auto pSlot = dynamic_cast<CUI_Object*>(pContainer->Get_ChildByOrder(ENUM(ChildSlot::BG)));
    if (!pSlot)
        return;

    pSlot->Set_Size({ m_fDigitTotalWidth + m_vPadding.x * 2.f, m_fHeight + m_vPadding.y * 2.f });
}

void CUI_DecibelDigits::Set_LayoutDigits()
{
    _vector2 vPos = m_vPadding;
    m_fDigitTotalWidth = 0.f;

    for (_int i = 0; i < _countof(digitOrder); ++i)
    {
        auto pSlot = Get_DigitSlot(digitOrder[i]);
        if (!pSlot)
            continue;
         
        pSlot->Set_AnchorOffset(vPos);
        _float fOffsetX = pSlot->Get_PxSize().x * 0.9f;
        vPos.x += fOffsetX;
        m_fDigitTotalWidth += fOffsetX;
    }
}

CUI_Object* CUI_DecibelDigits::Get_Slot(ChildSlot slot)
{
    auto pContainer = Get_Component<CObjectContainer>();

    return (pContainer) ? dynamic_cast<CUI_Object*>(pContainer->Get_ChildByOrder(ENUM(slot))) : nullptr;
}

CUI_Object* CUI_DecibelDigits::Get_DigitSlot(DigitSlot slot)
{
    switch (slot)
    {
    case DigitSlot::DIGIT_1000: return Get_Slot(ChildSlot::DIGIT_1000);
    case DigitSlot::DIGIT_100:  return Get_Slot(ChildSlot::DIGIT_100);
    case DigitSlot::DIGIT_10:   return Get_Slot(ChildSlot::DIGIT_10);
    case DigitSlot::DIGIT_1:    return Get_Slot(ChildSlot::DIGIT_1);
    }

    return nullptr;
}

CGameObject* CUI_DecibelDigits::Create()
{
    CUI_DecibelDigits* pInstance = new CUI_DecibelDigits();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_DecibelDigits");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_DecibelDigits::Clone(INIT_DESC* pArg)
{
    CUI_DecibelDigits* pInstance = new CUI_DecibelDigits(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_DecibelDigits");
        Safe_Release(pInstance);
    }
    return pInstance;
}