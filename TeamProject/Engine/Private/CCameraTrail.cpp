#include "Engine_Defines.h"
#include "CCameraTrail.h"

HRESULT CCameraTrail::Initialize_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CCameraTrail::Initialize(COMPONENT_DESC* pArg)
{
    return E_NOTIMPL;
}

CCameraTrail* CCameraTrail::Create()
{
    return nullptr;
}

void CCameraTrail::Free()
{
    __super::Free();

}

CComponent* CCameraTrail::Clone()
{
    return new CCameraTrail(*this);
}