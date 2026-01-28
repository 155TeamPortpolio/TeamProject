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

    m_pBg = m_pChildren[ENUM(CHILD::BG)]->Get_Component<CSprite2D>();
    m_pText = m_pChildren[ENUM(CHILD::TEXTS)]->Get_Component<CTextSlot>();

    Change_BgTexture("CombatBgText.png");
    Set_ChildColor(CHILD::BG, _float4(0.f, 0.f, 0.f, 1.f));
    auto pChild = m_pChildren[ENUM(CHILD::TEXTS)];
    if (pChild)
        pChild->Set_AnchorOffset(m_vPadding);

    Init_TextSlot();

    return S_OK;
}

void CUI_DecibelText::Update(_float dt)
{
    Set_Alpha((*m_pState == ENUM(CUI_Decibel::State::NONE)) ? 0.f : 1.f);
    Set_ChildColor(CHILD::TEXTS, *m_pColor); 

    if (*m_pState != m_iPrevState)
    {
        m_iPrevState = *m_pState;

        Set_Text(TEXT_CONTENTS[*m_pState]);
    }

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_DecibelText::Ready_PartObjects()
{
    auto pGameInstance = CGameInstance::GetInstance();
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        string strPrototypeTag = "Proto_GameObject_Image";
        if (i == ENUM(CHILD::TEXTS))
            strPrototypeTag = "Proto_GameObject_Text";

        CUI_Object* pObj = Builder::Create_UIObject({ G_GlobalLevelKey, strPrototypeTag })
            .Build("decibelText" + to_string(i));

        if (!pObj)
            continue;

        pContainer->Add_Child(pObj);
        m_pChildren[i] = pObj;
    }
}

void CUI_DecibelText::Init_TextSlot()
{
    if (!m_pText)
        return;

    m_pText->Set_Font("NanumSquareNeo.spritefont");
    m_pText->Set_Size(m_fTextScale);
    m_pText->Set_Shear(_float2(-0.4f, 0.f));
    //m_pText->Enable_AutoPos(ANCHOR::Center);
}

void CUI_DecibelText::Set_Text(const wstring& wstrText)
{
    auto pTextChild = m_pChildren[ENUM(CHILD::TEXTS)];
    auto pBgChild = m_pChildren[ENUM(CHILD::BG)];

    if (!m_pText || !pTextChild || !pBgChild)
        return;

    m_pText->Set_Text(wstrText);

    const _float2 textSize = Calc_TextPxSize();
    pTextChild->Set_Size(textSize);

    Update_Layout();
}

void CUI_DecibelText::Update_Layout()
{
    auto pTextChild = m_pChildren[ENUM(CHILD::TEXTS)];
    auto pBgChild = m_pChildren[ENUM(CHILD::BG)];

    if (!pTextChild || !pBgChild)
        return;

    const _float2 textPx = pTextChild->Get_PxSize();

    pBgChild->Set_Size({
        textPx.x + m_vPadding.x * 2.f,
        textPx.y + m_vPadding.y * 2.f
        });

    Set_Size(pBgChild->Get_PxSize());
}

_float2 CUI_DecibelText::Calc_TextPxSize() const
{
    if (!m_pText)
        return _float2();

    _float2 size = m_pText->Get_TextSize() * m_fTextScale;
    const _float shearX = fabs(m_pText->Get_Shear().x);
    size.x += shearX * size.y * 2.f;
    return size;
}

void CUI_DecibelText::Set_ChildColor(CHILD child, _float4 vColor)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Color(vColor);
}

void CUI_DecibelText::Set_ChildAlpha(CHILD child, _float fAlpha)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Alpha(fAlpha);
}

void CUI_DecibelText::Change_BgTexture(const string& strTextureKey)
{
    if (!m_pBg)
        return;

    m_pBg->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
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