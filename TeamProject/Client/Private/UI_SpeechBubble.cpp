#include "pch.h"
#include "UI_SpeechBubble.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "CharacterController.h"

#include "TextSlot.h"
#include "Player.h"

HRESULT CUI_SpeechBubble::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();

    return S_OK;
}

HRESULT CUI_SpeechBubble::Initialize(INIT_DESC* pArg)
{
    SPEECHBUBBLE_DESC* pDesc = static_cast<SPEECHBUBBLE_DESC*>(pArg);
    m_vPosition = pDesc->vPosition + m_vOffset;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("speechBubble.json")));
    Cache();
     
    Set_SpeechBubble(pDesc->strSpeech);

    return S_OK;
}

void CUI_SpeechBubble::Awake()
{
    Set_Alpha(0.f);
}

void CUI_SpeechBubble::Update(_float dt)
{
    Update_WorldToScreen(m_vPosition);
   
    Update_Visible(CalcState_ByDistance());

    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_SpeechBubble::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    auto pBg = pContainer->Find_Descendant("bg");
    if (pBg)
        m_pBg = dynamic_cast<CUI_Object*>(pBg);

    auto pText = pContainer->Find_Descendant("text");
    if (pText)
        m_pTextSlot = dynamic_cast<CTextSlot*>(pText->Get_Component<CTextSlot>());
}

void CUI_SpeechBubble::Set_SpeechBubble(wstring strSpeech)
{
    if (!m_pTextSlot)
        return;

    m_pTextSlot->Set_Text(strSpeech);

    if (!m_pBg)
        return;

    _float2 vTextSize = m_pTextSlot->Get_TextSize() * m_pTextSlot->Get_Scale();
    m_pBg->Set_Size(vTextSize + m_vPadding);
}

void CUI_SpeechBubble::Update_Visible(STATE_VISIBLE eNewState)
{
    if (m_eVisible == eNewState)
        return;

    m_eVisible = eNewState;

    if (m_eVisible == STATE_VISIBLE::VISIBLE)
        Set_Animation(0);
    else
        Set_Animation(1);
}

CUI_SpeechBubble::STATE_VISIBLE CUI_SpeechBubble::CalcState_ByDistance()
{
    auto pPlayer = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player));
    Vector4 vPlayerPos = static_cast<CPlayer*>(pPlayer)->Get_CurCharacterHandle().Get()->Get_Position();

    Vector3 vDiff = (Vector3(m_vPosition) - Vector3(vPlayerPos));

    return (max(1.f, m_fRadius) >= vDiff.Length()) ? STATE_VISIBLE::VISIBLE : STATE_VISIBLE::INVISIBLE;
}

CGameObject* CUI_SpeechBubble::Create()
{
    CUI_SpeechBubble* pInstance = new CUI_SpeechBubble();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_SpeechBubble");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_SpeechBubble::Clone(INIT_DESC* pArg)
{
    CUI_SpeechBubble* pInstance = new CUI_SpeechBubble(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_SpeechBubble");
        Safe_Release(pInstance);
    }
    return pInstance;
}