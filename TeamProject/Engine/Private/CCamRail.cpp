#include "Engine_Defines.h"
#include "CCamRail.h"

HRESULT CCamRail::Initialize_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CCamRail::Initialize(COMPONENT_DESC* pArg)
{
    return E_NOTIMPL;
}

CCamRail* CCamRail::Create()
{
    return nullptr;
}

void CCamRail::Free()
{
    __super::Free();

}

CComponent* CCamRail::Clone()
{
    return new CCamRail(*this);
}