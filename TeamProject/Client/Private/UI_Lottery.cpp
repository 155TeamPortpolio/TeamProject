#include "pch.h"
#include "UI_Lottery.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "ButtonUI.h"

HRESULT CUI_Lottery::Initialize_Prototype()
{
	__super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_Lottery::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    // JSON ·Îµå
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("lottery.json")));

    auto pObj = Builder::Create_UIObject({LevelManager()->Get_NowLevelKey(), "Proto_GameObject_ScratchCard"}).Build("scratchCard");
    if (pObj)
    {
        Get_Component<CObjectContainer>()->Add_Child(pObj);
        m_pChildren[ENUM(CHILD::SCRATCH)] = pObj;
    }

    Cache();

    if(m_pButtons[ENUM(BTN::BTN_BACK)])
        m_pButtons[ENUM(BTN::BTN_BACK)]->Set_OnClick([this]() { OnClick_Back(); });

    if (m_pButtons[ENUM(BTN::BTN_REFRESH)])
        m_pButtons[ENUM(BTN::BTN_REFRESH)]->Set_OnClick([this]() { OnClick_RefreshNews(); });

    if (m_pButtons[ENUM(BTN::BTN_SCRATCH)])
        m_pButtons[ENUM(BTN::BTN_SCRATCH)]->Set_OnClick([this]() { OnClick_OpenScratch(); });

	return S_OK;
}

void CUI_Lottery::Awake()
{
}

void CUI_Lottery::Update(_float dt)
{
	__super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_Lottery::Late_Update(_float dt)
{
}

void CUI_Lottery::UI_Active(void* pArg)
{
}

void CUI_Lottery::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();
     
    for (_int i = 0; i < ENUM(BTN::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(BTN_NAMES[i]);
        if (!pObj)
            continue;

        m_pButtons[i] = dynamic_cast<CButtonUI*>(pObj);
    }
}

void CUI_Lottery::OnClick_Back()
{
    if (Is_ChildAlive(CHILD::SCRATCH))
        Set_ChildUIDeActive(CHILD::SCRATCH);
}

void CUI_Lottery::OnClick_RefreshNews()
{
}

void CUI_Lottery::OnClick_OpenScratch()
{
    Set_ChildUIActive(CHILD::SCRATCH);
}

void CUI_Lottery::Set_ChildUIActive(CHILD child, void* pArg)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->UI_Active(pArg);
}

void CUI_Lottery::Set_ChildUIDeActive(CHILD child, void* pArg)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->UI_DeActive(pArg);
}

_bool CUI_Lottery::Is_ChildAlive(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return false;

    return pChild->Is_Alive();
}

CGameObject* CUI_Lottery::Create()
{
    CUI_Lottery* pInstance = new CUI_Lottery();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Lottery");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Lottery::Clone(INIT_DESC* pArg)
{
    CUI_Lottery* pInstance = new CUI_Lottery(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Lottery");
        Safe_Release(pInstance);
    }
    return pInstance;
}