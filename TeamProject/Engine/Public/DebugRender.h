#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CDebugRender :
    public CComponent
{
private:
    CDebugRender();
    CDebugRender(const CDebugRender& rhs);
    virtual ~CDebugRender() DEFAULT;
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
    HRESULT Render_DebugBox(ID3D11DeviceContext* pContext, _uint DrawIndex );
    void Add_DebugBounding(const MINMAX_BOX& box);
    _uint Get_DebugBoxCount() {
        if (m_Vertices.empty())return 0;
        return m_Vertices.size(); 
    }

private:
    vector<ID3D11Buffer*> m_Vertices;
    ID3D11Buffer* m_pIB = { nullptr };

public:
    static CDebugRender* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END