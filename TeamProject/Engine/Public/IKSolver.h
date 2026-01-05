#pragma once
#include "Engine_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL IIKSolver abstract : public CBase
{
protected:
    IIKSolver() {}
    IIKSolver(const IIKSolver& rhs) = delete;
    virtual ~IIKSolver() DEFAULT;

public:
    virtual IK_TYPE Get_Type() const PURE;

public:
    virtual HRESULT Initialize(void* pArg) PURE;
    virtual void    Solve(IK_CONTEXT& context) PURE;
    virtual void    Reset() PURE;
    virtual void    Render_GUI() {}

public:
    virtual void Free() override { __super::Free(); }
};

NS_END