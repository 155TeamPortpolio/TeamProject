#include "pch.h"
#include "UI_AnomalyStack.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "UI_AnomalyStackSlot.h"

HRESULT CUI_AnomalyStack::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

    PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_AnomalyStackSlot", CUI_AnomalyStackSlot::Create());

	return S_OK;
}

HRESULT CUI_AnomalyStack::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    Ready_Slots();

	return S_OK;
}

void CUI_AnomalyStack::Awake()
{
}

void CUI_AnomalyStack::Update(_float dt)
{
	__super::Update(dt);
}

HRESULT CUI_AnomalyStack::Ready_Slots()
{
    _float fSlotWidth = 15.f;   // ¿¹½Ã
    _float fSpacing = 2.f;
    _int iCount = 6;

    _float fTotalWidth = fSlotWidth * iCount + fSpacing * (iCount - 1);
    _float fStartX = -fTotalWidth * 0.5f + fSlotWidth * 0.5f;

    for (_int i = 0; i < iCount; ++i)
    {
        auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_AnomalyStackSlot" }).Build("slot");
        if (!pObj)
            continue;

        _float fX = fStartX + i * (fSlotWidth + fSpacing);

        pObj->Set_AnchorOffset({ fX, 0.f});

        Get_Component<CObjectContainer>()->Add_Child(pObj);
    }

    return S_OK;
}

CGameObject* CUI_AnomalyStack::Create()
{
    CUI_AnomalyStack* pInstance = new CUI_AnomalyStack();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_AnomalyStack");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_AnomalyStack::Clone(INIT_DESC* pArg)
{
    CUI_AnomalyStack* pInstance = new CUI_AnomalyStack(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_AnomalyStack");
        Safe_Release(pInstance);
    }
    return pInstance;
}