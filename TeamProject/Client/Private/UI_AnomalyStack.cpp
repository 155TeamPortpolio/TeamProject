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
    __super::Awake();
}

void CUI_AnomalyStack::Update(_float dt)
{
    if (InputDevice()->Key_Tap('Z'))
        Increase(2);

    if (InputDevice()->Key_Tap('X'))
        Increase(4);

    if (InputDevice()->Key_Tap('C'))
        Increase(6);

    if (InputDevice()->Key_Tap('V'))
        Decrease(4);

    if (InputDevice()->Key_Tap('B'))
        Decrease(2);

    if (InputDevice()->Key_Tap('N'))
        Decrease(0);

	__super::Update(dt);
}

HRESULT CUI_AnomalyStack::Ready_Slots()
{
    _float fSlotWidth = 15.f;
    _float fSpacing = 2.f;

    _float fTotalWidth = fSlotWidth * m_iSlotCount + fSpacing * (m_iSlotCount - 1);
    _float fStartX = -fTotalWidth * 0.5f + fSlotWidth * 0.5f;

    for (_int i = 0; i < m_iSlotCount; ++i)
    {
        auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_AnomalyStackSlot" }).Build("slot");
        if (!pObj)
            continue;

        _float fX = fStartX + i * (fSlotWidth + fSpacing);

        pObj->Set_AnchorOffset({ fX, 0.f});

        Get_Component<CObjectContainer>()->Add_Child(pObj);
        m_pSlots.push_back(dynamic_cast<CUI_AnomalyStackSlot*>(pObj));
    }

    return S_OK;
}

void CUI_AnomalyStack::Increase(_uint iCount)
{
    for (_int i = 0; i < m_iSlotCount; ++i)
    {
        if (!m_pSlots[i])
            continue;

        if (iCount == m_iSlotCount)
            m_pSlots[i]->PlayEffect();

        if (iCount > i)
            m_pSlots[i]->Activate(true);
    }
}

void CUI_AnomalyStack::Decrease(_uint iCount)
{
    for (_int i = 0; i < m_iSlotCount; ++i)
    {
        if (!m_pSlots[i])
            continue;

        m_pSlots[i]->StopEffect();
        if (iCount <= i)
            m_pSlots[i]->Activate(false);
    }
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

void CUI_AnomalyStack::Free()
{
    __super::Free();

    m_pSlots.clear();
}