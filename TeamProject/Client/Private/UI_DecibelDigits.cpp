#include "pch.h"
#include "UI_DecibelDigits.h"
#include "UI_Decibel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "Texture.h"

const string CUI_DecibelDigits::DIGIT_TEXTURES[ENUM(DigitTexture::END)] = { "00.png", "01.png", "02.png", "03.png", "04.png", "05.png", "06.png", "07.png", "08.png", "09.png" };
CUI_DecibelDigits::Digit CUI_DecibelDigits::digitOrder[] = { Digit::DIGIT_1000, Digit::DIGIT_100, Digit::DIGIT_10, Digit::DIGIT_1 };

HRESULT CUI_DecibelDigits::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_DecibelDigits::Initialize(INIT_DESC* pArg)
{
    DIGITS_DESC* pDesc = static_cast<DIGITS_DESC*>(pArg);
    m_pDecibel = pDesc->pDecibel;
    m_pColor = pDesc->pColor;

    __super::Initialize(pArg);

    Ready_PartObjects();

    return S_OK;
}

void CUI_DecibelDigits::Update(_float dt)
{
    _int iDecibel = static_cast<_int>(*m_pDecibel);

    if (iDecibel == m_iPrevDecibel)
        return;

    m_iPrevDecibel = iDecibel;

    Update_Digits(iDecibel);
    Update_Layout();
}

void CUI_DecibelDigits::Ready_PartObjects()
{
    auto pContainer = Get_Component<CObjectContainer>();
    const string& strLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

    for (_int i = 0; i < ENUM(Child::END); ++i)
    {
        CUI_Object* pObj = Builder::Create_UIObject({ strLevelKey, "Proto_GameObject_Image" })
            .Build("decibelDigits" + to_string(i));

        if (i == ENUM(Child::BG))
        {
            pObj->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "CombatBg00.png");
            pObj->Set_Color(Helper::HexToColor("#000000"));
        }
        
        pContainer->Add_Child(pObj);
        m_handles[i] = pObj->Get_Handle();
    }
}

void CUI_DecibelDigits::Update_Digits(_int iDecibel)
{
    Set_Digit(Child::DIGIT_1000, static_cast<DigitTexture>(Helper::Get_Digit(iDecibel, 3)));
    Set_Digit(Child::DIGIT_100, static_cast<DigitTexture>(Helper::Get_Digit(iDecibel, 2)));
    Set_Digit(Child::DIGIT_10, static_cast<DigitTexture>(Helper::Get_Digit(iDecibel, 1)));
    Set_Digit(Child::DIGIT_1, static_cast<DigitTexture>(Helper::Get_Digit(iDecibel, 0)));

    for (_int i = 0; i < _countof(digitOrder); ++i)
    {
        auto pDigit = Get_Digit(digitOrder[i]);
        if (!pDigit)
            continue;
        
        pDigit->Set_Color(*m_pColor);
    }
}

void CUI_DecibelDigits::Update_Layout()
{
    Set_LayoutDigits();
    Set_LayoutBg();
}

void CUI_DecibelDigits::Set_Digit(Child child, DigitTexture texture)
{
    if (!m_handles[ENUM(child)].isValid())
        return;

    auto pObj = m_handles[ENUM(child)].Get();
    if (!pObj)
        return;

    auto pSprite = pObj->Get_Component<CSprite2D>();
    pSprite->Change_Texture(0, G_GlobalLevelKey, DIGIT_TEXTURES[ENUM(texture)]);
    pObj->Set_Size(_float2(m_fHeight * pSprite->Get_AspectRatio(), m_fHeight));
}

void CUI_DecibelDigits::Set_LayoutDigits()
{
    _vector2 vPos = m_vPadding;
    m_fDigitTotalWidth = 0.f;

    for (_int i = 0; i < _countof(digitOrder); ++i)
    {
        auto pDigit = Get_Digit(digitOrder[i]);
        if (!pDigit)
            continue;

        pDigit->Set_AnchorOffset(vPos);
        _float fOffsetX = pDigit->Get_PxSize().x * 0.9f;
        vPos.x += fOffsetX;
        m_fDigitTotalWidth += fOffsetX;
    }
}

void CUI_DecibelDigits::Set_LayoutBg()
{
    if (!m_handles[ENUM(Child::BG)].isValid())
        return;

    auto pBg = m_handles[ENUM(Child::BG)].Get();
    pBg->Set_Size({ m_fDigitTotalWidth + m_vPadding.x * 2.f, m_fHeight + m_vPadding.y * 2.f });
    Set_Size(pBg->Get_PxSize());
}

CUI_Object* CUI_DecibelDigits::Get_Digit(Digit digit)
{
    UI_HANDLE handle = {};

    switch (digit)
    {
    case Digit::DIGIT_1000: handle = m_handles[ENUM(Child::DIGIT_1000)]; break;
    case Digit::DIGIT_100:  handle = m_handles[ENUM(Child::DIGIT_100)]; break;
    case Digit::DIGIT_10:   handle = m_handles[ENUM(Child::DIGIT_10)]; break;
    case Digit::DIGIT_1:    handle = m_handles[ENUM(Child::DIGIT_1)]; break;
    }

    return (handle.isValid()) ? handle.Get() : nullptr ;
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