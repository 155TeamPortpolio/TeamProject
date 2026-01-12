#include "pch.h"
#include "Npc.h"

CNpc::CNpc()
{
}

CNpc::CNpc(const CNpc& rhs)
    :CGameObject(rhs)
{
}

HRESULT CNpc::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CNpc::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    return S_OK;
}

void CNpc::Update(_float dt)
{
}

void CNpc::Free()
{
    __super::Free();
}
