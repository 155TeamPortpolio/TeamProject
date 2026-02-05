#include "pch.h"
#include "UI_GachaResult.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

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

    Create_Items();

    UI_Active();

	return S_OK;
}

void CUI_GachaResult::Awake()
{
}

void CUI_GachaResult::Update(_float dt)
{
    if (InputDevice()->Key_Down('P'))
        UI_Active();

	__super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_GachaResult::UI_Active(void* pArg)
{
    Set_Animation(0);
    if (m_pTitle)
        m_pTitle->Set_Animation(0);
}

void CUI_GachaResult::UI_DeActive(void* pArg)
{
}

void CUI_GachaResult::Cache()
{
    m_pTitle = dynamic_cast<CUI_Object*>(Get_Component<CObjectContainer>()->Find_Descendant("title"));
}

void CUI_GachaResult::Create_Items()
{
    auto pContainer = Get_Component<CObjectContainer>();

    _float2 vCenter = { m_WinSize.x * 0.5f, m_WinSize.y * 0.7f };

    for (_int i = 0; i < COL; ++i)
    {
        for (_int j = 0; j < ROW; ++j)
        {
            auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_GachaResultItem" }).Build("resultItem");
            if (!pObj)
                continue;

            float offsetX = (i - (COL - 1) * 0.5f) * (WIDTH + SPACING);
            float offsetY = (j - (ROW - 1) * 0.5f) * (HEIGHT + SPACING);

            float x = vCenter.x + offsetX;
            float y = vCenter.y + offsetY;
            pObj->Set_AnchorOffset({ x, y });

            pContainer->Add_Child(pObj);
        } 
    } 
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