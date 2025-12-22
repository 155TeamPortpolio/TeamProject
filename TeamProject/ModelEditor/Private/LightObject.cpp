#include "pch.h"
#include "LightObject.h"
#include "Light.h"

CLightObject::CLightObject()
{
}

CLightObject::CLightObject(const CLightObject& rhs)
    :CGameObject(rhs)
{
}

HRESULT CLightObject::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CLight>();
    return S_OK;
}

HRESULT CLightObject::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    LIGHT_DESC desc = {};
    desc.vOffsetPosition = { 0,0,0,0 };
    desc.fLightRange = 300;
    desc.fLightIntensity = 10.f;
    desc.eType = LIGHT_TYPE::POINT;
    desc.vLightDiffuse = { 1.f,1.f,1.f,1.f };
    Get_Component<CLight>()->Set_Desc(desc, LIGHT_TYPE::POINT);
    return S_OK;
}

void CLightObject::Priority_Update(_float dt)
{
}

void CLightObject::Update(_float dt)
{
}

void CLightObject::Late_Update(_float dt)
{
}

void CLightObject::Render_GUI()
{
    __super::Render_GUI();
}

CLightObject* CLightObject::Create()
{
    CLightObject* instance = new CLightObject();
    if (FAILED(instance->Initialize_Prototype()))
    {
        MSG_BOX("Object Create Failed : CLightObject");
        Safe_Release(instance);
    }

    return instance;
}

void CLightObject::Free()
{
    __super::Free();
}

CGameObject* CLightObject::Clone(INIT_DESC* pArg)
{
    CLightObject* instance = new CLightObject(*this);

    if (FAILED(instance->Initialize(pArg)))
    {
        MSG_BOX("Object Clone Failed : CLightObject");
        Safe_Release(instance);
    }

    return instance;
}
