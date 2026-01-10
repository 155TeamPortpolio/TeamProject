#include "pch.h"
#include "UI_DecibelKanji.h"
#include "UI_Decibel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "Texture.h" 

const string CUI_DecibelKanji::KANJI_TEXTURES[ENUM(CUI_Decibel::State::END)] = { "CombatMaximum.png", "CombatUproar.png", "CombatBlasting.png", "CombatMaximum.png" };

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

	return S_OK;
}

void CUI_DecibelKanji::Update(_float dt)
{
    ////상태 바뀌었을 때만 실행하고 싶은데, 컬러가 바로 안 바뀜
    //if (*m_pState == m_iPrevState)
    //    return;
    //
    //m_iPrevState = *m_pState;

    Set_Kanji(static_cast<CUI_Decibel::State>(*m_pState));
}

void CUI_DecibelKanji::Ready_PartObjects()
{
    for (_int i = 0; i < ENUM(ChildSlot::END); ++i)
    {
        CUI_Object* pSlot = Builder::Create_UIObject({ CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(), "Proto_GameObject_Image" })
            .Build("kanji" + to_string(i));
        Get_Component<CObjectContainer>()->Add_Child(pSlot);
    }

    auto pBg = Get_Slot(ChildSlot::BG);

    if (pBg)
    {
        pBg->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "CombatBg00.png");
        pBg->Set_Color(Helper::HexToColor("#000000"));
    }
}

void CUI_DecibelKanji::Set_Kanji(CUI_Decibel::State texture)
{
    auto pKanji = Get_Slot(ChildSlot::KANJI);
    auto pBg = Get_Slot(ChildSlot::BG);

    if (!pKanji || !pBg)
        return;

    Set_KanjiTexture(pKanji, KANJI_TEXTURES[ENUM(texture)]);
    pKanji->Set_Color(*m_pColor);
    Set_Layout(pKanji, pBg);
}

void CUI_DecibelKanji::Set_KanjiTexture(CUI_Object* pKanji, string textureKey)
{
    pKanji->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, textureKey);
    pKanji->Set_Size(_float2(m_fHeight * pKanji->Get_Component<CSprite2D>()->Get_AspectRatio(), m_fHeight));
}

void CUI_DecibelKanji::Set_Layout(CUI_Object* pKanji, CUI_Object* pBg)
{
    const _float2 vSize = pKanji->Get_PxSize();
    pBg->Set_Size({ vSize.x + m_vPadding.x * 2.f, vSize.y + m_vPadding.y * 2.f});

    pBg->Set_AnchorOffset({ 0.f, 0.f });
    pKanji->Set_AnchorOffset(m_vPadding);
    m_vSize = pBg->Get_PxSize();
}

CUI_Object* CUI_DecibelKanji::Get_Slot(ChildSlot slot)
{
    auto pContainer = Get_Component<CObjectContainer>();
    return (pContainer) ? dynamic_cast<CUI_Object*>(pContainer->Get_ChildByOrder(ENUM(slot))) : nullptr;
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