#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CHiZ_Culling :
    public CBase
{
private:
    CHiZ_Culling();
    ~CHiZ_Culling() DEFAULT;
public:
    HRESULT Initialize();
    void Update_HiZ(ID3D11DeviceContext* pContext);

public:
    void Check_Resource();

#ifdef _USING_GUI
    void Render_GUI();
#endif // _USING_GUI
private:
    _bool isReady = { false };
    ID3D11Texture2D* m_pHiZTex = { nullptr };
    ID3D11UnorderedAccessView* m_pHiZ0Uav = { nullptr };
    ID3D11ShaderResourceView* m_phiZ0Srv = { nullptr };
    ID3D11ShaderResourceView* m_pDepthSrv = { nullptr };
    class CComputeShader* m_pComputeShader = { nullptr };

private:
    _uint3 m_iGroup = {};

public:
    static CHiZ_Culling* Create();
    void Free() override;
};
NS_END
