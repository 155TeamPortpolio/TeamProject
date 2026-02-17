#include "pch.h"
#include "UI_AnomalyStackSlot.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "SpriteAnimationUI.h"

void CUI_AnomalyStackSlot::Activate(_bool isActive)
{
    if(isActive)
        Change_State(STATE::ACTIVATING);
    else
        Change_State(STATE::EMPTY);
}

void CUI_AnomalyStackSlot::StopEffect()
{
    if (m_eState != STATE::ACTIVE)
        return;

    Change_State(STATE::DEACTIVATING);
}

HRESULT CUI_AnomalyStackSlot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_AnomalyStackSlot::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("anomalyStackSlot_miyabi.json")));
    Cache();

	return S_OK;
}

void CUI_AnomalyStackSlot::Awake()
{
    SetFill(false);
    Set_ChildAlive(CHILD::START, false);
    Set_ChildAlive(CHILD::LOOP, false); 
    Set_ChildAlive(CHILD::END, false);
}

void CUI_AnomalyStackSlot::Update(_float dt)
{
    switch (m_eState)
    {
    case STATE::ACTIVATING:
        if (Is_AnimationFinished(CHILD::START))
            Change_State(STATE::ACTIVE);
        break;
    }

    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_AnomalyStackSlot::UI_Active(void* pArg)
{
}

void CUI_AnomalyStackSlot::UI_DeActive(void* pArg)
{
}

void CUI_AnomalyStackSlot::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::COUNT); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
    }
}

void CUI_AnomalyStackSlot::Change_State(STATE eState)
{
    if (m_eState == eState)
        return;

    m_eState = eState;
    switch (eState)
    {
    case STATE::EMPTY:
        SetFill(false);
        break;
    case STATE::ACTIVATING:
        SetFill(true);
        PlayFireStart();
        break;
    case STATE::ACTIVE:
        PlayFireLoop();
        break;
    case STATE::DEACTIVATING:
        PlayFireEnd();
        break;
    }
}

void CUI_AnomalyStackSlot::SetFill(_bool isFill)
{
    Set_ChildAlive(CHILD::FILL, isFill);

    if (!isFill)
        Set_ChildAnimation(CHILD::OVERLAY, 0);
}

void CUI_AnomalyStackSlot::PlayFireStart()
{
    Set_ChildAlive(CHILD::START, true);
    Set_ChildAlive(CHILD::LOOP, false);
    Set_ChildAlive(CHILD::END, false);

    Set_ChildSpriteAnimation(CHILD::START, true);
}

void CUI_AnomalyStackSlot::PlayFireLoop()
{
    Set_ChildAlive(CHILD::START, false);
    Set_ChildAlive(CHILD::LOOP, true);
    Set_ChildAlive(CHILD::END, false);

    Set_ChildSpriteAnimation(CHILD::LOOP, true);
}

void CUI_AnomalyStackSlot::PlayFireEnd()
{
    Set_ChildAlive(CHILD::START, false);
    Set_ChildAlive(CHILD::LOOP, false);
    Set_ChildAlive(CHILD::END, true);

    Set_ChildSpriteAnimation(CHILD::END, true);
}

void CUI_AnomalyStackSlot::Set_ChildAlive(CHILD child, _bool isAlive)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Alive(isAlive);
}

void CUI_AnomalyStackSlot::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

void CUI_AnomalyStackSlot::Set_ChildSpriteAnimation(CHILD child, _bool isActive)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;
    
    auto pContainer = pChild->Get_Component<CObjectContainer>();
    if (!pContainer)
        return;

    for (auto& pChild : pContainer->Get_Children())
    {
        if (!pChild || !pChild->Is_Alive())
            continue;

        if (auto pSpriteAnimation = static_cast<CSpriteAnimationUI*>(pChild))
        {
            if (isActive)
                pSpriteAnimation->UI_Active();
            else
                pSpriteAnimation->UI_DeActive();
        } 
    }
}

_bool CUI_AnomalyStackSlot::Is_AnimationFinished(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild || !pChild->Is_Alive())
        return true;

    if (!pChild->Is_AnimFinished())
        return false;

    auto pContainer = pChild->Get_Component<CObjectContainer>();
    if (!pContainer)
        return true;

    for (auto& pChild : pContainer->Get_Children())
    {
        if (!pChild || !pChild->Is_Alive())
            continue;

        if (!dynamic_cast<CUI_Object*>(pChild)->Is_AnimFinished())
            return false;
    }

    return true;
}

CGameObject* CUI_AnomalyStackSlot::Create()
{
    CUI_AnomalyStackSlot* pInstance = new CUI_AnomalyStackSlot();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_AnomalyStackSlot");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_AnomalyStackSlot::Clone(INIT_DESC* pArg)
{
    CUI_AnomalyStackSlot* pInstance = new CUI_AnomalyStackSlot(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_AnomalyStackSlot");
        Safe_Release(pInstance);
    }
    return pInstance;
}