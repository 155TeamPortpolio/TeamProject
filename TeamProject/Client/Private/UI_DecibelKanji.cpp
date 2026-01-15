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
    Set_Color();

    if (*m_pState != m_iPrevState)
    {
        m_iPrevState = *m_pState;

        Set_Kanji(static_cast<CUI_Decibel::State>(*m_pState));
    } 
}

void CUI_DecibelKanji::Ready_PartObjects()
{
    auto pContainer = Get_Component<CObjectContainer>();
    const string& strLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

    for (_int i = 0; i < ENUM(Child::END); ++i)
    {
        CUI_Object* pObj = Builder::Create_UIObject({ strLevelKey, "Proto_GameObject_Image" })
            .Build("decibelKanji" + to_string(i));

        if (i == ENUM(Child::BG))
        {
            pObj->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "CombatBg00.png");
            pObj->Set_Color(Helper::HexToColor("#000000"));
        }

        pContainer->Add_Child(pObj);
        m_handles[i] = pObj->Get_Handle();
    }
}

void CUI_DecibelKanji::Set_Color()
{
    if (m_handles[ENUM(Child::KANJI)].isValid())
        m_handles[ENUM(Child::KANJI)].Get()->Set_Color(*m_pColor);

    m_vColor.w = (*m_pState == ENUM(CUI_Decibel::State::NONE)) ? 0.f : 1.f ;
}

void CUI_DecibelKanji::Set_Kanji(CUI_Decibel::State texture)
{
    Set_KanjiTexture(KANJI_TEXTURES[ENUM(texture)]);
    Set_Layout();
}

void CUI_DecibelKanji::Set_KanjiTexture(string textureKey)
{
    if (!m_handles[ENUM(Child::KANJI)].isValid())
        return;

    auto pObj = m_handles[ENUM(Child::KANJI)].Get();
    pObj->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, textureKey);
    pObj->Set_Size(_float2(m_fHeight * pObj->Get_Component<CSprite2D>()->Get_AspectRatio(), m_fHeight));
    pObj->Set_AnchorOffset(m_vPadding);
}

void CUI_DecibelKanji::Set_Layout()
{
    if (!m_handles[ENUM(Child::BG)].isValid())
        return;

    auto pObj = m_handles[ENUM(Child::BG)].Get();
    const _float2 vSize = m_handles[ENUM(Child::KANJI)].isValid() ? m_handles[ENUM(Child::KANJI)].Get()->Get_PxSize() : _float2();
    pObj->Set_Size({ vSize.x + m_vPadding.x * 2.f, vSize.y + m_vPadding.y * 2.f});
    pObj->Set_AnchorOffset({ 0.f, 0.f });
    m_vSize = pObj->Get_PxSize();
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