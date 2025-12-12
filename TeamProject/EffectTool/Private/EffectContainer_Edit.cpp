#include "pch.h"
#include "EffectContainer_Edit.h"

CEffectContainer_Edit::CEffectContainer_Edit()
    :CEffectContainer()
{
}

CEffectContainer_Edit::CEffectContainer_Edit(const CEffectContainer_Edit& rhs)
    :CEffectContainer(rhs)
{
}

HRESULT CEffectContainer_Edit::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CEffectContainer_Edit::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    return S_OK;
}

void CEffectContainer_Edit::Awake()
{
}

void CEffectContainer_Edit::Priority_Update(_float dt)
{
}

void CEffectContainer_Edit::Update(_float dt)
{
}

void CEffectContainer_Edit::Late_Update(_float dt)
{
}

void CEffectContainer_Edit::Render_GUI()
{

}

CEffectContainer_Edit* CEffectContainer_Edit::Create()
{
    CEffectContainer_Edit* instance = new CEffectContainer_Edit();

    if (FAILED(instance->Initialize_Prototype()))
    {
        MSG_BOX("Object Clone Failed : CEffectContainer_Edit");
        Safe_Release(instance);
    }

    return instance;
}

CGameObject* CEffectContainer_Edit::Clone(INIT_DESC* pArg)
{
    CEffectContainer_Edit* instance = new CEffectContainer_Edit(*this);

    if (FAILED(instance->Initialize(pArg)))
    {
        MSG_BOX("Object Clone Failed : CEffectContainer_Edit");
        Safe_Release(instance);
    }

    return instance;
}

void CEffectContainer_Edit::Free()
{
    __super::Free();
}
