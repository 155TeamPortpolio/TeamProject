#include "pch.h"
#include "UI_WipeoutOverlay.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "SpriteAnimationUI.h"

HRESULT CUI_WipeoutOverlay::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_WipeoutOverlay::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("wipeout_overlay.json")));

    auto pContainer = Get_Component<CObjectContainer>();
    m_pSpriteAnimation = dynamic_cast<CSpriteAnimationUI*>(pContainer->Find_Descendant("SpriteAnimation"));

	return S_OK;
}

void CUI_WipeoutOverlay::Awake()
{
    Change_State(STATE::INVISIBLE);
}

void CUI_WipeoutOverlay::Update(_float dt)
{
	__super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);

    switch (m_eState)
    {
    case STATE::VISIBLE:
        if (m_pSpriteAnimation && m_pSpriteAnimation->Is_AnimFinished())
            Change_State(STATE::FINISHED);
        break;
    case STATE::FINISHED:
        Change_State(STATE::INVISIBLE);
        break;
    }
}

void CUI_WipeoutOverlay::UI_Active(void* pArg)
{
    Change_State(STATE::VISIBLE); 
}

void CUI_WipeoutOverlay::UI_DeActive(void* pArg)
{
    Change_State(STATE::INVISIBLE); 
}

_bool CUI_WipeoutOverlay::Is_AnimFinished()
{
    return m_eState == STATE::FINISHED;
}

void CUI_WipeoutOverlay::Change_State(STATE eState)
{
    if (m_eState == eState)
        return;

    m_eState = eState;
    switch (eState)
    {
    case STATE::VISIBLE:
        Set_Alive(true);
        if (m_pSpriteAnimation)
            m_pSpriteAnimation->UI_Active();
        break;
    case STATE::INVISIBLE:
        Set_Alive(false);
        break;
    }
}

CGameObject* CUI_WipeoutOverlay::Create()
{
    CUI_WipeoutOverlay* pInstance = new CUI_WipeoutOverlay();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_WipeoutOverlay");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_WipeoutOverlay::Clone(INIT_DESC* pArg)
{
    CUI_WipeoutOverlay* pInstance = new CUI_WipeoutOverlay(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_WipeoutOverlay");
        Safe_Release(pInstance);
    }
    return pInstance;
}