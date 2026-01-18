#pragma once

#include "CamObject.h"

NS_BEGIN(Client)

class CAnimCam final : public CCamObject
{
private:
    CAnimCam() = default;
    CAnimCam(const CAnimCam& rhs) : CCamObject(rhs) {}
    virtual ~CAnimCam() = default;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Priority_Update(_float dt)  override;
    void    Update(_float dt)           override;
    void    Late_Update(_float dt)      override;
    void    Render_GUI()                override { __super::Render_GUI(); }

    void    SetTarget(OBJECT_HANDLE handle) { this->handle = handle; }

private:
    OBJECT_HANDLE handle{};

public:
    static  CAnimCam* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    void Free() override { __super::Free(); }
};

NS_END