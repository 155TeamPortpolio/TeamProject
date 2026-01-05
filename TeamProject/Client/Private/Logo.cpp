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

    CUI_Object* pUI = Builder::Create_UIObject({ "Logo_Level", "Proto_GameObject_CanvasPanel" })
        .Asset("loading.json")
        .Build("loading");
    
    CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pUI, "Logo_Level");

    m_handle = pUI->Get_Handle();

	return S_OK;
}

void CLogo::Priority_Update(_float dt)
{
}

void CLogo::Update(_float dt)
{
    if (GetAsyncKeyState('P'))
    {
        if (m_handle.isValid())
            m_handle.Get()->Set_Animation(1);
    }

    if (GetAsyncKeyState('O'))
    {
        if (m_handle.isValid())
            m_handle.Get()->Set_Animation(0);
    }
}

void CLogo::Late_Update(_float dt)
{
}

void CLogo::Render_GUI()
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