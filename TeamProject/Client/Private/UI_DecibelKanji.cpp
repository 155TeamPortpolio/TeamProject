#include "pch.h"
#include "UI_DecibelKanji.h"
#include "UI_Decibel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "Texture.h" 

HRESULT CUI_DecibelKanji::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_DecibelKanji::Initialize(INIT_DESC* pArg)
{
    KANJI_DESC* pDesc = static_cast<KANJI_DESC*>(pArg);
    m_pState = pDesc->pState;
    m_pColor = pDesc->pColor;

    __super::Initialize(pArg);

    Ready_PartObjects();

    Set_ChildColor(CHILD::BG, _float4(0.f, 0.f, 0.f, 1.f));
    Change_SpriteTexture(CHILD::BG, "CombatBgKanji.png");

	return S_OK;
}

void CUI_DecibelKanji::Update(_float dt)
{
    Set_Alpha((*m_pState == ENUM(CUI_Decibel::State::NONE)) ? 0.f : 1.f);
    Set_ChildColor(CHILD::KANJI, *m_pColor);

    if (*m_pState != m_iPrevState)
    {
        m_iPrevState = *m_pState;

        Set_KanjiTexture(KANJI_TEXTURES[ENUM(static_cast<CUI_Decibel::State>(*m_pState))]);
    } 
}

void CUI_DecibelKanji::Ready_PartObjects()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        CUI_Object* pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_Image" })
            .Build("decibelKanji" + to_string(i));
        if (!pObj)
            continue;

        pContainer->Add_Child(pObj);
        m_pChildren[i] = pObj;

        if (auto pSprite = m_pChildren[i]->Get_Component<CSprite2D>())
            m_pSprites[i] = pSprite;
    }
}

void CUI_DecibelKanji::Set_ChildColor(CHILD child, _float4 vColor)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Color(vColor);
}

void CUI_DecibelKanji::Set_ChildAlpha(CHILD child, _float fAlpha)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Alpha(fAlpha);
}

void CUI_DecibelKanji::Change_SpriteTexture(CHILD child, const string& strTextureKey)
{
    auto pSprite = m_pSprites[ENUM(child)];
    if (!pSprite)
        return;

    pSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

void CUI_DecibelKanji::Set_KanjiTexture(string textureKey)
{
    auto pChild = m_pChildren[ENUM(CHILD::KANJI)];
    if (!pChild)
        return;

    pChild->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, textureKey);
    pChild->Set_Size(_float2(m_fHeight * pChild->Get_Component<CSprite2D>()->Get_AspectRatio(), m_fHeight));
    pChild->Set_AnchorOffset(_float2(m_vPadding.x * 0.9f, m_vPadding.y));

    Set_Layout();
}

void CUI_DecibelKanji::Set_Layout()
{
    auto pChild = m_pChildren[ENUM(CHILD::BG)];
    if (!pChild)
        return;

    const _float2 vSize = m_pChildren[ENUM(CHILD::KANJI)] ? m_pChildren[ENUM(CHILD::KANJI)]->Get_PxSize() : _float2();
    pChild->Set_Size({ vSize.x + m_vPadding.x * 2.f, vSize.y + m_vPadding.y * 2.f});
    pChild->Set_AnchorOffset({ 0.f, 0.f });
    m_vSize = pChild->Get_PxSize();
}

CGameObject* CUI_DecibelKanji::Create()
{
    CUI_DecibelKanji* pInstance = new CUI_DecibelKanji();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_DecibelKanji");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_DecibelKanji::Clone(INIT_DESC* pArg)
{
    CUI_DecibelKanji* pInstance = new CUI_DecibelKanji(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_DecibelKanji");
        Safe_Release(pInstance);
    }
    return pInstance;
}