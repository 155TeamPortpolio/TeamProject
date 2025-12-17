#pragma once
#include "GameObject.h"

NS_BEGIN(EffectTool)
class CToolGrid :
    public CGameObject
{
private:
    CToolGrid();
    CToolGrid(const CToolGrid& rhs);
    virtual ~CToolGrid() DEFAULT;

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
    static CToolGrid* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();

private:
    _float m_fGridSize{ 20.f };
};
NS_END
