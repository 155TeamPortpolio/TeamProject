#include "pch.h"
#include "UI_Loading.h"

#include "GameInstance.h"

HRESULT CUI_Loading::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CUI_Loading::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    return S_OK;
}

void CUI_Loading::Awake()
{
    string strCurrentLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();// m_LevelTag;
    CUI_Object* uiObj = Builder::Create_UIObject({ strCurrentLevel, "Proto_GameObject_CanvasPanel" })
        .Asset("loading_default.json")
        .Build("prefabLogo");

    if (uiObj)
    {
        CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(uiObj, strCurrentLevel);

        m_handle = uiObj->Get_Handle();
    }

    if (m_handle.isValid())
        m_handle.Get()->Set_Animation(0);
}

void CUI_Loading::Update(_float dt)
{
}

CGameObject* CUI_Loading::Create()
{
    CUI_Loading* pInstance = new CUI_Loading();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Loading");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Loading::Clone(INIT_DESC* pArg)
{
    CUI_Loading* pInstance = new CUI_Loading(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Loading");
        Safe_Release(pInstance);
    }
    return pInstance;
}