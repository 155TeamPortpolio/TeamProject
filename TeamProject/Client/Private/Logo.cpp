#include "pch.h"
#include "Logo.h"

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