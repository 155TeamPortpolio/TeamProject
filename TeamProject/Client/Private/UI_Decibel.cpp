#include "pch.h"
#include "UI_Decibel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"

#include "UI_DecibelKanji.h"
#include "UI_DecibelDigits.h"
#include "UI_DecibelPts.h"
#include "UI_DecibelText.h"

HRESULT CUI_Decibel::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();

	return S_OK;
}

HRESULT CUI_Decibel::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
     
    Get_Component<CEventListener>()->Add_Listner<UI_STATUS_DESC>([&](const UI_STATUS_DESC& desc)
        {
            if (desc.eOwner == m_eOwner &&
                desc.eType == m_eType)
                Set_Decibel(desc.value.fCurValue);
        });

    Ready_PartObjects();

	return S_OK;
}

void CUI_Decibel::Update(_float dt)
{
    if (!m_initLayout)
    {
        Set_Decibel(0);
        m_initLayout = true;
    }

    _float fLerpAmount = min(1.f, dt * m_fColorLerpSpeed);
    XMStoreFloat4(&m_vColor, XMVectorLerp(XMLoadFloat4(&m_vColor), XMLoadFloat4(&m_vTargetColor), fLerpAmount));

    Get_Component<CObjectContainer>()->UpdateChild(dt); 
}

void CUI_Decibel::Ready_PartObjects()
{
    const string& strLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

    CUI_DecibelKanji::KANJI_DESC* pKanji = new CUI_DecibelKanji::KANJI_DESC;
    pKanji->pState = &m_iState;
    pKanji->pColor = &m_vColor;
    Attach_Child(strLevelKey, "Proto_GameObject_DecibelKanji", "decibelKanji", pKanji, &m_handles[ENUM(Child::KANJI)]);

    CUI_DecibelDigits::DIGITS_DESC* pDigits = new CUI_DecibelDigits::DIGITS_DESC;
    pDigits->pDecibel = &m_fDecibel;
    pDigits->pColor = &m_vColor;
    Attach_Child(strLevelKey, "Proto_GameObject_DecibelDigits", "decibelDigits", pDigits, &m_handles[ENUM(Child::DIGITS)]);

    CUI_DecibelPts::PTS_DESC* pPts = new CUI_DecibelPts::PTS_DESC;
    pPts->pColor = &m_vColor;
    Attach_Child(strLevelKey, "Proto_GameObject_DecibelPts", "decibelPts", pPts, &m_handles[ENUM(Child::PTS)]);

    CUI_DecibelText::TEXT_DESC* pText = new CUI_DecibelText::TEXT_DESC;
    pText->pState = & m_iState;
    pText->pColor = &m_vColor;
    Attach_Child(strLevelKey, "Proto_GameObject_DecibelText", "decibelText", pText, &m_handles[ENUM(Child::TEXTS)]);
}

void CUI_Decibel::Set_Decibel(_float fDecibel)
{
    if (fDecibel < 0.f || fDecibel > 9999.f)
        return;

    m_fDecibel = fDecibel;

    if (m_fDecibel >= 3000)
    {
        m_iState = State::COMBAT_MAXIMUM;
        m_vTargetColor = Helper::HexToColor("#FF9A22");
    }
    else if (m_fDecibel >= 2000)
    {
        m_iState = State::COMBAT_BLASTING;
        m_vTargetColor = Helper::HexToColor("#FFFF3E");
    }
    else if (m_fDecibel >= 1000)
    {
        m_iState = State::COMBAT_UPROAR;
        m_vTargetColor = Helper::HexToColor("#41FDFE");
    }
    else
    {
        m_iState = State::NONE;
        m_vTargetColor = Helper::HexToColor("#D8D1D5");
    }

    Layout();
}

void CUI_Decibel::Layout()
{
    if (!m_handles[ENUM(Child::KANJI)].isValid() ||
        !m_handles[ENUM(Child::DIGITS)].isValid() ||
        !m_handles[ENUM(Child::PTS)].isValid() ||
        !m_handles[ENUM(Child::TEXTS)].isValid())
        return;

    _float2 vKanjiSize = m_handles[ENUM(Child::KANJI)].Get()->Get_PxSize();
    _float2 vDigitsSize = m_handles[ENUM(Child::DIGITS)].Get()->Get_PxSize();
    _float2 vPtsSize = m_handles[ENUM(Child::PTS)].Get()->Get_PxSize();
    _float2 vTextsSize = m_handles[ENUM(Child::TEXTS)].Get()->Get_PxSize();

    m_handles[ENUM(Child::DIGITS)].Get()->Set_AnchorOffset(_float2(vKanjiSize.x * 0.9f, 0.f));
    m_handles[ENUM(Child::PTS)].Get()->Set_AnchorOffset(_float2((vKanjiSize.x + vDigitsSize.x) * 0.9f, vDigitsSize.y - vPtsSize.y));
    m_handles[ENUM(Child::TEXTS)].Get()->Set_AnchorOffset(_float2(vKanjiSize.x * 0.9f, vKanjiSize.y - vTextsSize.y));
}

void CUI_Decibel::Attach_Child(const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, UI_DESC* pDesc, UI_HANDLE* pHandleOut)
{
    auto builder = Builder::Create_UIObject({ strLevelKey, strPrototypeTag });

    if (pDesc)
        builder.Add_UIDesc(pDesc);

    CUI_Object* pObj = builder.Build(strInstanceName);
    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);

    if (pHandleOut)
        *pHandleOut = pObj->Get_Handle();
}

CGameObject* CUI_Decibel::Create()
{
    CUI_Decibel* pInstance = new CUI_Decibel();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_DecibelGauge");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Decibel::Clone(INIT_DESC* pArg)
{
    CUI_Decibel* pInstance = new CUI_Decibel(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Decibel");
        Safe_Release(pInstance);
    }
    return pInstance;
}