#pragma once
#include "GameObject.h"
NS_BEGIN(Demo)
class CDemoGrid :
    public CGameObject
{
    struct HittedArea {
        _float4 vEdgeMin = {};
        _float4 vEdgeMax = {};
    };

protected:
    CDemoGrid();
    CDemoGrid(const CDemoGrid& rhs);
    virtual ~CDemoGrid() DEFAULT;

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
    static CDemoGrid* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};

NS_END

