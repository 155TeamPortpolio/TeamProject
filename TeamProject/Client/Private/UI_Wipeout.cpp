#include "pch.h"
#include "UI_Wipeout.h"

#include "GameInstance.h"
#include "ObjectContainer.h" 

HRESULT CUI_Wipeout::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Wipeout::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("wipeout.json")));
    Cache();
    
    Change_State(STATE::INACTIVE);

    return S_OK;
}

void CUI_Wipeout::Awake()
{
    __super::Awake();
}

void CUI_Wipeout::Update(_float dt)
{
    if (m_eState == STATE::DEACTIVATING)
        Change_State(STATE::INACTIVE);

    if (m_eState == STATE::ACTIVE && Is_GroupAnimationFinished(m_eCurrentGroup))
    {
        switch (m_eCurrentGroup)
        {
        case GROUP::GROUP1:
            Change_Group(GROUP::GROUP2);
            break;
        case GROUP::GROUP2:
            Change_Group(GROUP::GROUP3);
            break;
        case GROUP::GROUP3:
            Change_Group(GROUP::GROUP4);
            break;
        case GROUP::GROUP4:
            Change_Group(GROUP::GROUP5, m_iBlinkCount);
            break;
        case GROUP::GROUP5:
            if (m_iBlinkCount == 2)
            {
                Change_State(STATE::DEACTIVATING);
                return;
            } 

            Change_Group(GROUP::GROUP4, m_iBlinkCount);
            m_iBlinkCount++;
            break;
        }
    }

    if(m_eState == STATE::ACTIVE)
        __super::Update(dt);

    Update_RTV("renderTargetScreen", true);
}

void CUI_Wipeout::UI_Active(void* pArg)
{
    Change_State(STATE::ACTIVE);
}

void CUI_Wipeout::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();
    for (_int i = 0; i < ENUM(GROUP::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pGroups[i] = dynamic_cast<CUI_Object*>(pObj);
    }
}

void CUI_Wipeout::Change_State(STATE eState)
{
    if (m_eState == eState)
        return;

    m_eState = eState;

    switch (m_eState)
    {
    case STATE::ACTIVE:
        Set_Alpha(1.f);
        Set_Scale(Vector2(1.f, 1.f));
        Set_Animation(0);
        Set_Alive(true);
        Change_Group(GROUP::GROUP1);
        m_iBlinkCount = 0;
        break;
    case STATE::DEACTIVATING:
        Set_Alpha(0.f);
        m_iCurrentClipIndex = -1;
        //Change_State(STATE::INACTIVE);
        break;
    case STATE::INACTIVE:
        Set_Alive(false);
        for(_int i = 0; i < ENUM(GROUP::END); ++i)
            Set_GroupAlive(static_cast<GROUP>(i), false);
        break;
    }
}

_bool CUI_Wipeout::Change_Group(GROUP eGroup, _int iIndex)
{
    if (m_eCurrentGroup == eGroup)
        return false;

    Set_GroupAlive(m_eCurrentGroup, false);

    m_eCurrentGroup = eGroup;
    Set_GroupAlive(m_eCurrentGroup, true);
    return Set_GroupAnimation(m_eCurrentGroup, iIndex);
}

void CUI_Wipeout::Set_GroupAlive(GROUP group, _bool isAlive)
{
    if (group < static_cast<GROUP>(0) || group >= GROUP::END)
        return;

    auto pGroup = m_pGroups[ENUM(group)];
    if (!pGroup)
        return;

    pGroup->Set_Alpha(isAlive ? 1 : 0);
    pGroup->Set_Alive(isAlive);
}

_bool CUI_Wipeout::Set_GroupAnimation(GROUP group, _int iIndex)
{
    auto pGroup = m_pGroups[ENUM(group)];
    if (!pGroup)
        return false;
 
    _bool isCheck = { false };

    isCheck |= pGroup->Set_Animation(iIndex);

    auto pContainer = pGroup->Get_Component<CObjectContainer>();
    if (!pContainer)
        return isCheck;

    for (auto& pChild : pContainer->Get_Children())
    {
        if (auto pUI = dynamic_cast<CUI_Object*>(pChild))
            isCheck |= pUI->Set_Animation(iIndex);
    }

    return isCheck;
}

_bool CUI_Wipeout::Is_GroupAnimationFinished(GROUP group)
{
    if (group == GROUP::END)
        return false;

    auto pGroup = m_pGroups[ENUM(group)];
    if (!pGroup)
        return false;

    if (false == pGroup->Is_AnimFinished())
        return false;

    auto pContainer = pGroup->Get_Component<CObjectContainer>();
    if (!pContainer)
        return true;

    for (auto& pChild : pContainer->Get_Children())
    {
        if (auto pUI = dynamic_cast<CUI_Object*>(pChild))
        {
            _bool isfinished = pUI->Is_AnimFinished();

            if (false == isfinished)
                return false;
        } 
    }

    return true;
}

CGameObject* CUI_Wipeout::Create()
{
    CUI_Wipeout* pInstance = new CUI_Wipeout();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Wipeout");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Wipeout::Clone(INIT_DESC* pArg)
{
    CUI_Wipeout* pInstance = new CUI_Wipeout(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Wipeout");
        Safe_Release(pInstance);
    }
    return pInstance;
}