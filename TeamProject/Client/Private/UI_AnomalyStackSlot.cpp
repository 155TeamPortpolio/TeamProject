#include "pch.h"
#include "UI_AnomalyStackSlot.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

void CUI_AnomalyStackSlot::Change_State(STATE eState)
{
    if (m_eState == eState)
        return;

    m_eState = eState;
    switch (m_eState)
    {
    case STATE::EMPTY:
        break;
    case STATE::FULL:
        break;
    }
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

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("anomalyStack_miyabi.json")));

	return S_OK;
}

void CUI_AnomalyStackSlot::Awake()
{
}

void CUI_AnomalyStackSlot::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_AnomalyStackSlot::UI_Active(void* pArg)
{
}

void CUI_AnomalyStackSlot::UI_DeActive(void* pArg)
{
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