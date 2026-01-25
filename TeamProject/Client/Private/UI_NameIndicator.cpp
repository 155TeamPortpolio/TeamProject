#include "pch.h"
#include "UI_NameIndicator.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "CharacterController.h"

#include "TextSlot.h"
#include "Player.h"

HRESULT CUI_NameIndicator::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();

    return S_OK;
}

HRESULT CUI_NameIndicator::Initialize(INIT_DESC* pArg)
{
    INDICATOR_DESC* pDesc = static_cast<INDICATOR_DESC*>(pArg);
    m_strName = pDesc->strName;
    m_pCCT = pDesc->pCCT;

    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("name_indicator.json")));
    Cache_Children();

    Get_Component<CEventListener>()->Add_Listener<UI_INTERACTABLE_DESC>([&](const UI_INTERACTABLE_DESC& desc)
        {
            if (desc.strName != m_strName)
                return;

            Update_Interact((desc.isInteractable)? STATE_INTERACT::INTERACTABLE : STATE_INTERACT::NOTINTERACTABLE);
        });

    return S_OK;
}

void CUI_NameIndicator::Awake()
{
    Set_Name(m_strName);

    Set_Alpha(0.f);
    Set_ChildAlpha(CHILD::ARROWL, 0.f);
    Set_ChildAlpha(CHILD::ARROWR, 0.f);
}

void CUI_NameIndicator::Update(_float dt)
{
    if(m_pCCT)
        XMStoreFloat3(&m_vPosition, m_pCCT->Get_FootPosition() + XMVectorSet(0.f, m_pCCT->Get_HalfSize() * 2.f, 0.f, 0.f));

    Update_WorldToScreen(m_vPosition);

    Update_Visible(CalcState_ByDistance());

    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (m_eInteract == STATE_INTERACT::INTERACTABLE)
    {
        if (Is_ChildAnimFinished(CHILD::ARROWL))
            Set_ChildAnimation(CHILD::ARROWL, 1);
        if (Is_ChildAnimFinished(CHILD::ARROWR))
            Set_ChildAnimation(CHILD::ARROWR, 1);
    }
}

void CUI_NameIndicator::Cache_Children()
{
    auto pContainer = Get_Component<CObjectContainer>();
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        const string& strInstanceName = INSTANCENAMES[i];
        if (strInstanceName.empty())
            continue;

        auto pObj = pContainer->Find_Descendant(strInstanceName);
        if (!pObj)
            continue;

        auto pUI = dynamic_cast<CUI_Object*>(pObj);
        m_pChildren[i] = pUI;
    }

    m_pName = m_pChildren[ENUM(CHILD::NAME)]->Get_Component<CTextSlot>();
}

void CUI_NameIndicator::Set_Name(const wstring& strName)
{
    if (!m_pName)
        return;

    m_pName->Set_Text(strName);
    m_vSize.x = m_pName->Get_TextSize().x * m_pName->Get_Scale() + Get_ChildSize(CHILD::ARROWL).x + Get_ChildSize(CHILD::ARROWR).x + m_fPadding * 2.f;
}

void CUI_NameIndicator::Update_Visible(STATE_VISIBLE eNewState)
{
    if (m_eVisible == eNewState)
        return;

    m_eVisible = eNewState;

    if (m_eVisible == STATE_VISIBLE::VISIBLE)
        Set_Animation(0);
    else
        Set_Animation(1);
}

void CUI_NameIndicator::Update_Interact(STATE_INTERACT eNewState)
{
    if (m_eInteract == eNewState)
        return;

    m_eInteract = eNewState; 

    if (m_eInteract == STATE_INTERACT::INTERACTABLE)
    {
        Set_ChildAnimation(CHILD::ARROWL, 0);
        Set_ChildAnimation(CHILD::ARROWR, 0);
    }
    else if (m_eInteract == STATE_INTERACT::NOTINTERACTABLE)
    {
        Set_ChildStopAnimation(CHILD::ARROWL);
        Set_ChildStopAnimation(CHILD::ARROWR);
        Set_ChildAlpha(CHILD::ARROWL, 0.f);
        Set_ChildAlpha(CHILD::ARROWR, 0.f);
    }
}

CUI_NameIndicator::STATE_VISIBLE CUI_NameIndicator::CalcState_ByDistance()
{
    auto pPlayer = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player));
    Vector4 vPlayerPos = static_cast<CPlayer*>(pPlayer)->Get_CurCharacterHandle().Get()->Get_Position();

    Vector3 vDiff = (Vector3(m_vPosition) - Vector3(vPlayerPos));

    return (max(1.f, m_fRadius) >= vDiff.Length()) ? STATE_VISIBLE::VISIBLE : STATE_VISIBLE::HIDDEN;
}

void CUI_NameIndicator::Set_ChildAlpha(CHILD child, _float fAlpha)
{
    auto pChild = m_pChildren[child];
    if (!pChild)
        return;

    pChild->Set_Alpha(fAlpha);
}

void CUI_NameIndicator::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[child];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

void CUI_NameIndicator::Set_ChildStopAnimation(CHILD child)
{
    auto pChild = m_pChildren[child];
    if (!pChild)
        return;

    pChild->Stop_Animation();
}

_bool CUI_NameIndicator::Is_ChildAnimFinished(CHILD child)
{
    auto pChild = m_pChildren[child];
    if (!pChild)
        return false;

    return pChild->Is_AnimFinished();
}

_float2 CUI_NameIndicator::Get_ChildSize(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return {};

    return pChild->Get_PxSize();
}

CGameObject* CUI_NameIndicator::Create()
{
    CUI_NameIndicator* pInstance = new CUI_NameIndicator();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_NameIndicator");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_NameIndicator::Clone(INIT_DESC* pArg)
{
    CUI_NameIndicator* pInstance = new CUI_NameIndicator(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_NameIndicator");
        Safe_Release(pInstance);
    }
    return pInstance;
}