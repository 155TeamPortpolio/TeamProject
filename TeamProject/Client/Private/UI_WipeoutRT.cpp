#include "pch.h"
#include "UI_WipeoutRT.h"

#include "GameInstance.h"
#include "ObjectContainer.h" 

HRESULT CUI_WipeoutRT::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_WipeoutRT::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("wipeoutRT.json")));
    Cache();
    
    Change_State(STATE::INACTIVE);

    return S_OK;
}

void CUI_WipeoutRT::Awake()
{
    __super::Awake();
}

void CUI_WipeoutRT::Update(_float dt)
{
     __super::Update(dt);

    m_fTimer += dt;
    if (m_fTimer >= m_fDuration)
        Change_State(STATE::INACTIVE);

    if (Is_GroupAnimationFinished(m_eCurrentGroup))
        Update_GroupState();

    Update_RTV("renderTargetScreen", true);
}

void CUI_WipeoutRT::UI_Active(void* pArg)
{
    Set_RenderTargetScreenRenderLayer(RENDER_LAYER::Default);
    Change_State(STATE::ACTIVE);
}

void CUI_WipeoutRT::Cache()
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

void CUI_WipeoutRT::Update_GroupState()
{
    switch (m_eCurrentGroup)
    {
    case GROUP::GROUP1:        Change_Group(GROUP::GROUP2);        break;
    case GROUP::GROUP2:        Change_Group(GROUP::GROUP3);        break;
    case GROUP::GROUP3:        Change_Group(GROUP::GROUP4);        break;
    case GROUP::GROUP4:        Change_Group(GROUP::GROUP5, m_iBlinkCount);        break;
    case GROUP::GROUP5:
        if (m_iBlinkCount == 2)
        {
            Change_State(STATE::INACTIVE);
            return;
        }

        Change_Group(GROUP::GROUP4, m_iBlinkCount);
        m_iBlinkCount++;
        break;
    }
}

void CUI_WipeoutRT::Change_State(STATE eState)
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
        m_fTimer = 0;
        break;
    case STATE::INACTIVE:
        Set_RenderTargetScreenRenderLayer(RENDER_LAYER::None);
        Set_Alive(false);
        for(_int i = 0; i < ENUM(GROUP::END); ++i)
            Set_GroupAlive(static_cast<GROUP>(i), false);
        m_iCurrentClipIndex = -1;
        break;
    }
}

_bool CUI_WipeoutRT::Change_Group(GROUP eGroup, _int iIndex)
{
    if (m_eCurrentGroup == eGroup)
        return false;

    Set_GroupAlive(m_eCurrentGroup, false);

    m_eCurrentGroup = eGroup;
    Set_GroupAlive(m_eCurrentGroup, true);
    return Set_GroupAnimation(m_eCurrentGroup, iIndex);
}

void CUI_WipeoutRT::Set_GroupAlive(GROUP group, _bool isAlive)
{
    if (group < static_cast<GROUP>(0) || group >= GROUP::END)
        return;

    auto pGroup = m_pGroups[ENUM(group)];
    if (!pGroup)
        return;

    pGroup->Set_Alpha(isAlive ? 1 : 0);
    pGroup->Set_Alive(isAlive);
}

_bool CUI_WipeoutRT::Set_GroupAnimation(GROUP group, _int iIndex)
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

_bool CUI_WipeoutRT::Is_GroupAnimationFinished(GROUP group)
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

CGameObject* CUI_WipeoutRT::Create()
{
    CUI_WipeoutRT* pInstance = new CUI_WipeoutRT();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_WipeoutRT");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_WipeoutRT::Clone(INIT_DESC* pArg)
{
    CUI_WipeoutRT* pInstance = new CUI_WipeoutRT(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_WipeoutRT");
        Safe_Release(pInstance);
    }
    return pInstance;
}