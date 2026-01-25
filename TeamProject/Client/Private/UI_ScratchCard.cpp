#include "pch.h"
#include "UI_ScratchCard.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_ScratchCard::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_ScratchCard::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("scratchCard.json")));

	return S_OK;
}

void CUI_ScratchCard::Awake()
{
}

void CUI_ScratchCard::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

CGameObject* CUI_ScratchCard::Create()
{
    CUI_ScratchCard* pInstance = new CUI_ScratchCard();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_ScratchCard");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_ScratchCard::Clone(INIT_DESC* pArg)
{
    CUI_ScratchCard* pInstance = new CUI_ScratchCard(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_ScratchCard");
        Safe_Release(pInstance);
    }
    return pInstance;
}