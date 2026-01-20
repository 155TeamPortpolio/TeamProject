#include "pch.h"
#include "UI_DecibelDigits.h"
#include "UI_Decibel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "Texture.h"

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

    Change_SpriteTexture(CHILD::BG, "CombatBgDigits.png");
    Set_ChildColor(CHILD::BG, _float4(0.f, 0.f, 0.f, 1.f)); 

    return S_OK;
}

void CUI_DecibelDigits::Update(_float dt)
{
    for (_int i = 0; i < _countof(digitOrder); ++i)
    {
        auto pDigit = Get_Digit(digitOrder[i]);
        if (!pDigit)
            continue;

        pDigit->Set_Color(*m_pColor);
    }

    _int iDecibel = static_cast<_int>(*m_pDecibel);

    if (iDecibel != m_iPrevDecibel)
    {
        m_iPrevDecibel = iDecibel;

        Update_Digits(iDecibel);
        Update_Layout();
    }
}

void CUI_DecibelDigits::Ready_PartObjects()
{
    auto pContainer = Get_Component<CObjectContainer>();
    const string& strLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        CUI_Object* pObj = Builder::Create_UIObject({ strLevelKey, "Proto_GameObject_Image" })
            .Build("decibelDigits" + to_string(i));

        if (!pObj)
            continue;

        pContainer->Add_Child(pObj);
        m_pChildren[i] = pObj;

        auto pSprite = m_pChildren[i]->Get_Component<CSprite2D>();
        if (!pSprite)
            continue;

        m_pSprites[i] = pSprite;
    }
}

void CUI_DecibelDigits::Update_Digits(_int iDecibel)
{
    Set_Digit(CHILD::DIGIT_1000, static_cast<DigitTexture>(Helper::Get_Digit(iDecibel, 3)));
    Set_Digit(CHILD::DIGIT_100, static_cast<DigitTexture>(Helper::Get_Digit(iDecibel, 2)));
    Set_Digit(CHILD::DIGIT_10, static_cast<DigitTexture>(Helper::Get_Digit(iDecibel, 1)));
    Set_Digit(CHILD::DIGIT_1, static_cast<DigitTexture>(Helper::Get_Digit(iDecibel, 0)));
}

void CUI_DecibelDigits::Update_Layout()
{
    Set_LayoutDigits();
    Set_LayoutBg();
}

void CUI_DecibelDigits::Set_Digit(CHILD child, DigitTexture texture)
{
    auto pSprite = m_pSprites[ENUM(child)];
    auto pChild = m_pChildren[ENUM(child)]; 
    if(!pSprite || !pChild)
        return;

    pSprite->Change_Texture(0, G_GlobalLevelKey, DIGIT_TEXTURES[ENUM(texture)]);
    pChild->Set_Size(_float2(m_fHeight * pSprite->Get_AspectRatio(), m_fHeight));
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
    auto pChild = m_pChildren[ENUM(CHILD::BG)];
    if (!pChild)
        return;

    pChild->Set_Size({ m_fDigitTotalWidth + m_vPadding.x * 2.f, m_fHeight + m_vPadding.y * 2.f });
    Set_Size(pChild->Get_PxSize());
}

void CUI_DecibelDigits::Set_ChildColor(CHILD child, _float4 vColor)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Color(vColor);
}

void CUI_DecibelDigits::Change_SpriteTexture(CHILD child, const string& strTextureKey)
{
    auto pSprite = m_pSprites[ENUM(child)];
    if (!pSprite)
        return;

    pSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

CUI_Object* CUI_DecibelDigits::Get_Digit(Digit digit) const
{
    switch (digit)
    {
    case Digit::DIGIT_1000: return m_pChildren[ENUM(CHILD::DIGIT_1000)];
    case Digit::DIGIT_100:  return m_pChildren[ENUM(CHILD::DIGIT_100)]; 
    case Digit::DIGIT_10:   return m_pChildren[ENUM(CHILD::DIGIT_10)]; 
    case Digit::DIGIT_1:    return m_pChildren[ENUM(CHILD::DIGIT_1)]; 
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