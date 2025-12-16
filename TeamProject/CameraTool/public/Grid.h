#pragma once
#include "GameObject.h"

NS_BEGIN(CameraTool)
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
    _uint                   m_iRayReceiverID = {};

public:
    static CGrid* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};

NS_END

