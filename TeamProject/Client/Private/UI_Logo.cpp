#include "pch.h"
#include "UI_Logo.h"

#include "GameInstance.h"

HRESULT CUI_Logo::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CUI_Logo::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    return S_OK;
}

void CUI_Logo::Awake()
{
    string strCurrentLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
    CUI_Object* pPrefab = Builder::Create_UIObject({ strCurrentLevel, "Proto_GameObject_CanvasPanel" })
        .Asset("logo.json")
        .Build("prefab");
    
    if (!pPrefab)
        return;

    CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pPrefab, strCurrentLevel);
    m_hRoot = pPrefab->Get_Handle();

    if (m_hRoot.isValid())
        m_hRoot.Get()->Set_Animation(0);
}

void CUI_Logo::Update(_float dt)
{ 
}

CGameObject* CUI_Logo::Create()
{
    CUI_Logo* pInstance = new CUI_Logo();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Logo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Logo::Clone(INIT_DESC* pArg)
{
    CUI_Logo* pInstance = new CUI_Logo(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Logo");
        Safe_Release(pInstance);
    }
    return pInstance;
}