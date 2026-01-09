#include "pch.h"
#include "UI_Decibel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "UI_DecibelKanji.h"
#include "UI_DecibelDigits.h"

HRESULT CUI_Decibel::Initialize_Prototype()
{
    __super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUI_Decibel::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Add_Component<CObjectContainer>();

    Ready_PartObjects();

	return S_OK;
}

void CUI_Decibel::Update(_float dt)
{
    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (!m_initLayout)
    {
        Set_Decibel(0);
        m_initLayout = true;
    }

    _float fLerpAmount = min(1.f, dt * m_fColorLerpSpeed);
    XMStoreFloat4(&m_vColor, XMVectorLerp(XMLoadFloat4(&m_vColor), XMLoadFloat4(&m_vTargetColor), fLerpAmount));

    /*테스트 코드*/
    if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('P'))
        Set_Decibel(m_iDecibel + 12);

    if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('Q'))
        Set_Decibel(m_iDecibel - 12);
}

void CUI_Decibel::Ready_PartObjects()
{
    auto pContainer = Get_Component<CObjectContainer>();;

    CUI_DecibelKanji::KANJI_DESC* pKanji = new CUI_DecibelKanji::KANJI_DESC;
    pKanji->pState = &m_iState;
    pKanji->pColor = &m_vColor; 
    pContainer->Add_Child(
        Builder::Create_UIObject({ CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(), "Proto_GameObject_DecibelKanji" })
        .Add_UIDesc(pKanji)
        .Build("decibelKanji"));

    CUI_DecibelDigits::DIGITS_DESC* pDigits = new CUI_DecibelDigits::DIGITS_DESC;
    pDigits->pDecibel = &m_iDecibel;
    pDigits->pColor = &m_vColor;
    pDigits->pOffsetX = &m_fDigitsOffsetX;
    pContainer->Add_Child(
        Builder::Create_UIObject({ CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(), "Proto_GameObject_DecibelDigits" })
        .Add_UIDesc(pDigits)
        .Build("decibelDigits"));
}

void CUI_Decibel::Set_Decibel(_int iDecibel)
{
    if (iDecibel < 0 || iDecibel > 9999)
        return;

    m_iDecibel = iDecibel;

    if (m_iDecibel >= 3000)
    {
        m_iState = State::COMBAT_MAXIMUM;
        m_vTargetColor = Helper::HexToColor("#FF9A22");
    }
    else if (m_iDecibel >= 2000)
    {
        m_iState = State::COMBAT_BLASTING;
        m_vTargetColor = Helper::HexToColor("#FFFF3E");
    }
    else if (m_iDecibel >= 1000)
    {
        m_iState = State::COMBAT_UPROAR;
        m_vTargetColor = Helper::HexToColor("#41FDFE");
    }
    else
    {
        m_iState = State::NONE;
        m_vTargetColor = Helper::HexToColor("#FFFFFF");
    }

    m_fDigitsOffsetX = (Get_Slot(ChildSlot::KANJI)) ? Get_Slot(ChildSlot::KANJI)->Get_PxSize().x * 0.9f : 100.f;
}

CUI_Object* CUI_Decibel::Get_Slot(ChildSlot slot)
{
    auto pContainer = Get_Component<CObjectContainer>();

    return (pContainer) ? dynamic_cast<CUI_Object*>(pContainer->Get_ChildByOrder(ENUM(slot))) : nullptr;
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