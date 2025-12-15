#pragma once
#include "GameObject.h"

NS_BEGIN(AnimTool)
class CGrid :
    public CGameObject
{
    struct HittedArea {
        _float4 vEdgeMin = {};
        _float4 vEdgeMax = {};
    };

protected:
    CGrid();
    CGrid(const CGrid& rhs);
    virtual ~CGrid() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

public:
    static CGrid* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};

NS_END

