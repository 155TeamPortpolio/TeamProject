#include "pch.h"
#include "UI_DecibelText.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "TextSlot.h"

const wstring CUI_DecibelText::TEXT_CONTENTS[ENUM(CUI_Decibel::State::END)] = { L"", L"UPROAR", L"Blasting", L"Maximum" };

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
    Set_Color();

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
    const auto& strLevelKey = pGameInstance->Get_LevelMgr()->Get_NowLevelKey();
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(Child::END); ++i)
    {
        string strPrototypeTag = "Proto_GameObject_Image";
        if (i == ENUM(Child::TEXTS)) 
            strPrototypeTag = "Proto_GameObject_Text";

        CUI_Object* pObj = Builder::Create_UIObject({ strLevelKey, strPrototypeTag })
            .Build("decibelText" + to_string(i));

        if (i == ENUM(Child::BG))
        {
            pObj->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, "CombatBg00.png");
            pObj->Set_Color(Helper::HexToColor("#000000"));
        }
        else if (i == ENUM(Child::TEXTS))
        {
            if (auto pTextSlot = pObj->Get_Component<CTextSlot>())
            {
                pTextSlot->Set_Font("NanumSquareNeo.spritefont");
                pTextSlot->Set_Size(m_fTextScale);
                pTextSlot->Set_Shear(_float2(-0.4f, 0.f));

                pObj->Set_AnchorOffset(m_vPadding);
            }
        }

        pContainer->Add_Child(pObj);
        m_handles[i] = pObj->Get_Handle();
    }
}

void CUI_DecibelText::Set_Color()
{
    if (m_handles[ENUM(Child::TEXTS)].isValid())
        m_handles[ENUM(Child::TEXTS)].Get()->Set_Color(*m_pColor);

    m_vColor.w = (*m_pState == ENUM(CUI_Decibel::State::NONE)) ? 0.f : 1.f;
}

void CUI_DecibelText::Set_Text(const wstring& wstrText)
{
    if (!m_handles[ENUM(Child::TEXTS)].isValid() || !m_handles[ENUM(Child::BG)].isValid())
        return;
    
    auto pText = m_handles[ENUM(Child::TEXTS)].Get();
    if (auto pTextSlot = pText->Get_Component<CTextSlot>())
    {
        pTextSlot->Set_Text(wstrText);
        auto textSize = pTextSlot->Get_TextSize() * m_fTextScale;   //  ??
        _float fShearX = fabs(pTextSlot->Get_Shear().x);
        textSize.x += fShearX * textSize.y * 2.f;
        pText->Set_Size(textSize);
    }

    auto pBg = m_handles[ENUM(Child::BG)].Get();
    const _float2 pSize = pText->Get_PxSize();
    pBg->Set_Size({ pSize.x + m_vPadding.x * 2.f, pSize.y + m_vPadding.y * 2.f });
    Set_Size(pBg->Get_PxSize());
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