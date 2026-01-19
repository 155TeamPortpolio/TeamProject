#pragma once

#include "UI_MeshPyramidData.h"

NS_BEGIN(Client)

class CUI_MeshPyramid final : public CGameObject
{
private:
    CUI_MeshPyramid() {}
    CUI_MeshPyramid(const CUI_MeshPyramid& rhs) : CGameObject(rhs) {}
    virtual ~CUI_MeshPyramid() DEFAULT;

public:
    HRESULT Initialize_Prototype()     override;
    HRESULT Initialize(INIT_DESC* arg) override;
    void    Priority_Update(_float dt) override {}
    void    Update(_float dt)          override;
    void    Late_Update(_float dt)     override {}

private:
    _bool   IsOnScreen(_float marginPx = 0.f);

private:
    UI_MeshPyramidConfig  cfg{}; 
    UI_MeshPyramidRuntime rt{};

private:
    Vector3 color = {1.f, 1.f, 1.f};
    _float  alpha = 0.f;

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};

NS_END