#include "pch.h"
#include "Logo.h"

#include "GameInstance.h"

HRESULT CLogo::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CLogo::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    return S_OK;
}

void CLogo::Awake()
{
    string strCurrentLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();// m_LevelTag;
    CUI_Object* uiObj = Builder::Create_UIObject({ strCurrentLevel, "Proto_GameObject_CanvasPanel" })
        .Asset("shaderTest1.json")
        .Build("prefabLogo");
    
    if (uiObj)
    {
        CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(uiObj, strCurrentLevel);

        m_handle = uiObj->Get_Handle();
    }

    if (m_handle.isValid())
        m_handle.Get()->Set_Animation(0);
}

void CLogo::Update(_float dt)
{ 
}

CGameObject* CLogo::Create()
{
    CLogo* pInstance = new CLogo();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CLogo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CLogo::Clone(INIT_DESC* pArg)
{
    CLogo* pInstance = new CLogo(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CLogo");
        Safe_Release(pInstance);
    }
    return pInstance;
}