#include "pch.h"
#include "GachaResult.h"

#include "StaticModel.h"
#include "Material.h"

#include "Helper_Func.h"

CGachaResult::CGachaResult()
    :CGameObject()
{
}

CGachaResult::CGachaResult(const CGachaResult& rhs)
    :CGameObject(rhs)
{
}

void CGachaResult::SetRenderState(_bool Render)
{
    if (Render) SetRenderLayer(RENDER_LAYER::Default);
    else SetRenderLayer(RENDER_LAYER::None);
}

HRESULT CGachaResult::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGachaResult::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CGachaResult::Awake()
{
}

void CGachaResult::Priority_Update(_float dt)
{
}

void CGachaResult::Update(_float dt)
{
 
}

void CGachaResult::Late_Update(_float dt)
{
}

void CGachaResult::Free()
{
    __super::Free();
}
