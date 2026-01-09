#include "pch.h"
#include "UI_Decibel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "UI_DecibelKanji.h"
#include "UI_DecibelDigits.h"

void CUI_Decibel::Set_Decibel(_int iDecibel)
{
    if (iDecibel < 0 || iDecibel > 9999)
        return;

    m_iDecibel = iDecibel;

    auto pKanji = dynamic_cast<CUI_DecibelKanji*>(Get_Slot(ChildSlot::KANJI));
    auto pDigits = dynamic_cast<CUI_DecibelDigits*>(Get_Slot(ChildSlot::DIGITS));

    if (!pKanji || !pDigits)
        return;

    // 간지 텍스쳐 셋 하고 (간지에서 분기하는게 나은가?)
    CUI_DecibelKanji::State kanjiState = { CUI_DecibelKanji::State::NONE };

    if (m_iDecibel >= 3000)
    {
        kanjiState = CUI_DecibelKanji::State::COMBAT_MAXIMUM;
        m_vTargetColor = Helper::HexToColor("#FF9A22");
    }
    else if (m_iDecibel >= 2000)
    {
        kanjiState = CUI_DecibelKanji::State::COMBAT_BLASTING;
        m_vTargetColor = Helper::HexToColor("#FFFF3E");
    }
    else if (m_iDecibel >= 2000)
    {
        kanjiState = CUI_DecibelKanji::State::COMBAT_UPROAR;
        m_vTargetColor = Helper::HexToColor("#41FDFE");
    }
    else
    {
        kanjiState = CUI_DecibelKanji::State::COMBAT_MAXIMUM;   //None 처리해야
        m_vTargetColor = Helper::HexToColor("#FFFFFF");
    }
    
    pKanji->Set_Kanji(kanjiState);      // 데시벨 보내고 안에서 분길하는게 나은가
    pDigits->Set_Digits(m_iDecibel);

    pDigits->Set_AnchorOffset(_float2(Get_Slot(ChildSlot::KANJI)->Get_PxSize().x, 0.f));
}

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

    Set_Decibel(m_iDecibel);

	return S_OK;
}

void CUI_Decibel::Update(_float dt)
{
    //Get_Component<CObjectContainer>()->UpdateChild(dt);
    //
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('P'))
    //    Set_Decibel(m_iDecibel + 12);
    //
    //if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('Q'))
    //    Set_Decibel(m_iDecibel - 12);

    _float fLerpAmount = min(1.f, dt * m_fColorLerpSpeed);
    XMStoreFloat4(&m_vCurrentColor, XMVectorLerp(XMLoadFloat4(&m_vCurrentColor), XMLoadFloat4(&m_vTargetColor), fLerpAmount));
}

void CUI_Decibel::Ready_PartObjects()
{
    auto pContainer = Get_Component<CObjectContainer>();

    pContainer->Add_Child(
        Builder::Create_UIObject({ CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(), "Proto_GameObject_DecibelKanji" })
        .Build("decibelKanji"));

    pContainer->Add_Child(
        Builder::Create_UIObject({ CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(), "Proto_GameObject_DecibelDigits" })
        .Build("decibelDigits"));
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