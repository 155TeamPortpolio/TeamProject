#pragma once

#include "GameObject.h"

NS_BEGIN(CameraTool)

class CGrid : public CGameObject
{
protected:
    CGrid() {}
    CGrid(const CGrid& rhs) : CGameObject(rhs) {}
    virtual ~CGrid() = default;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt)  override{}
    virtual void    Update(_float dt)           override{}
    virtual void    Late_Update(_float dt)      override{}
    virtual void    Render_GUI()                override { __super::Render_GUI(); }

public:
    static CGrid* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() { __super::Free(); }
};

NS_END

