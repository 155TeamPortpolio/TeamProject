#include "pch.h"
#include "UI_GachaResult.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "UI_GachaResultItem.h"
#include "UI_TextButton.h"

HRESULT CUI_GachaResult::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaResult::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("gacha_result.json")));
    Cache();
    Create_ConfirmButton();

    m_InstanceName = "gachaResult";

    Set_Alive(false);

	return S_OK;
}

void CUI_GachaResult::Awake()
{
}

void CUI_GachaResult::Update(_float dt)
{
    Update_ItemAppear(dt);

    __super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_GachaResult::UI_Active(void* pArg)
{
    if (!pArg)
        return;
     
    Set_Alive(true);
    Set_Animation(0);
    if (m_pTitle)
        m_pTitle->Set_Animation(0);

    m_isAlltemsAppeared = false;
    if (m_pConfirmButton)
        m_pConfirmButton->Set_Alive(false);

    // 아이템 처리 
    m_iItemAppearIndex = 0;
    m_fItemAppearTimer = 0;

    RESULT_DESC* pDesc = static_cast<RESULT_DESC*>(pArg);
    m_pResultDesc = pDesc->pResultDesc;

    if (!Ensure_ItemCount(m_pResultDesc->size()))
        return;

    for (_int i = 0; i < m_pResultDesc->size(); ++i)
        m_pItems[i]->Set_ResultDesc((*m_pResultDesc)[i]);

    Update_ItemLayout();
}

void CUI_GachaResult::UI_DeActive(void* pArg)
{
}

void CUI_GachaResult::Cache()
{
    m_pTitle = dynamic_cast<CUI_Object*>(Get_Component<CObjectContainer>()->Find_Descendant("title"));
}

void CUI_GachaResult::Create_ConfirmButton()
{
    CUI_TextButton::BUTTON_DESC* pDesc = new CUI_TextButton::BUTTON_DESC;
    pDesc->strLabel = L"확인";
    pDesc->onClick = []() {};

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_TextButton" })
        .Add_UIDesc(pDesc)
        .Build("confrimButton");

    if (!pObj)
        return;

    pObj->Set_AnchorOffset({ m_WinSize.x * 0.5f, m_WinSize.y * 0.8f});
    pObj->Set_Alive(false);

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pConfirmButton = pObj;
}

void CUI_GachaResult::Update_ItemAppear(_float dt)
{
    const _int iTargetCount = m_pResultDesc ? m_pResultDesc->size() : 0;

    if (iTargetCount == 0)
        return;

    if (m_iItemAppearIndex >= iTargetCount)
    {
        if (!m_isAlltemsAppeared)
        {
            m_isAlltemsAppeared = true;
            if (m_pConfirmButton)
                m_pConfirmButton->Set_Alive(true);
        }
        return;
    }

    m_fItemAppearTimer += dt;

    if (m_fItemAppearTimer < m_fItemAppearDuration)
        return;

    m_pItems[m_iItemAppearIndex]->UI_Active();
    ++m_iItemAppearIndex;
    m_fItemAppearTimer = 0.f;
}

bool CUI_GachaResult::Ensure_ItemCount(size_t count)
{
    while (m_pItems.size() < count)
    {
        auto pItem = Create_Item();
        if (!pItem)
            return false;

        m_pItems.push_back(pItem);
    }
    return true;
}

void CUI_GachaResult::Update_ItemLayout()
{
    _int iCount = m_pResultDesc->size();
    if (iCount == 0)
        return;

    _int iCol = min((int)iCount, MAX_COL);
    _int iRow = (int)((iCount + MAX_COL - 1) / MAX_COL);

    _float2 vCenter = { m_WinSize.x * 0.5f, m_WinSize.y * 0.52f };

    for (size_t i = 0; i < iCount; ++i)
    {
        _int x = i % iCol;
        _int y = i / iCol;

        _float vOffsetX = (x - (iCol - 1) * 0.5f) * (WIDTH + SPACING);
        _float vOffsetY = (y - (iRow - 1) * 0.5f) * (HEIGHT + SPACING);

        m_pItems[i]->Set_AnchorOffset({ vCenter.x + vOffsetX, vCenter.y + vOffsetY });
    }
}

CUI_GachaResultItem* CUI_GachaResult::Create_Item()
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_GachaResultItem" }).Build("resultItem");
    if (!pObj)
        return nullptr;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    return dynamic_cast<CUI_GachaResultItem*>(pObj);
}

CGameObject* CUI_GachaResult::Create()
{
    CUI_GachaResult* pInstance = new CUI_GachaResult();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_GachaResult");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_GachaResult::Clone(INIT_DESC* pArg)
{
    CUI_GachaResult* pInstance = new CUI_GachaResult(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_GachaResult");
        Safe_Release(pInstance);
    }
    return pInstance;
}