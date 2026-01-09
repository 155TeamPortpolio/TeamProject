#include "pch.h"
#include "UI_DecibelKanji.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "Texture.h"

const string CUI_DecibelKanji::KANJI_TEXTURES[ENUM(State::END)] = { "CombatMaximum.png", "CombatUproar.png", "CombatBlasting.png", "CombatMaximum.png" };

void CUI_DecibelKanji::Set_Kanji(State texture)
{
    auto pKanji = Get_Slot(ChildSlot::KANJI);
    auto pBg = Get_Slot(ChildSlot::BG);

    if (!pKanji || !pBg)
        return;

    Set_KanjiTexture(pKanji, KANJI_TEXTURES[ENUM(texture)]);
    Set_Layout(pKanji, pBg);
}

HRESULT CUI_DecibelKanji::Initialize_Prototype()
{
    __super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUI_DecibelKanji::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Add_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(ChildSlot::END); ++i)
    {
        CUI_Object* pSlot = Builder::Create_UIObject({ CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(), "Proto_GameObject_Image" })
            .Build("dd");
        Get_Component<CObjectContainer>()->Add_Child(pSlot);
    }

    auto pKanji = Get_Slot(ChildSlot::KANJI);
    auto pBg = Get_Slot(ChildSlot::BG);

    if (pKanji)
    {
        pKanji->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "CombatMaximum.png");
        pKanji->Set_Color(Helper::HexToColor("#FFFFFF"));
    }

    if (pBg)
    {
        pBg->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "CombatBg00.png");
        pBg->Set_Color(Helper::HexToColor("#000000"));
    } 

	return S_OK;
}

void CUI_DecibelKanji::Update(_float dt)
{
}

void CUI_DecibelKanji::Set_KanjiTexture(CUI_Object* pKanji, string textureKey)
{
    pKanji->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, textureKey);
    pKanji->Set_Size(_float2(m_fHeight * pKanji->Get_Component<CSprite2D>()->Get_CurTexture()->Get_AspectRatio(), m_fHeight));
}

void CUI_DecibelKanji::Set_Layout(CUI_Object* pKanji, CUI_Object* pBg)
{
    const _float2 vKanjiSize = pKanji->Get_PxSize();
    pBg->Set_Size({vKanjiSize.x + m_vPadding.x * 2.f, vKanjiSize.y + m_vPadding.y * 2.f});

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