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
    _uint CalcMipCount(_uint width, _uint height);

private:
    _bool m_isReady = { false };

    _uint m_mipCount = {};
    _uint2 m_texSize = {};
    _uint3 m_threadSize = {};
    _uint3 m_groupCount = {};

    ID3D11Texture2D* m_pHiZTex = { nullptr };
    vector<ID3D11UnorderedAccessView*> m_HiZUav;    /*¹Óº° UAVµé*/
    vector<ID3D11ShaderResourceView*> m_HiZSrvMip; /*¹Ó¸Ê SRVµé*/

    ID3D11ShaderResourceView* m_pDepthSrv = { nullptr };
    class CComputeShader* m_pComputeShader = { nullptr };

private:
    _uint3 m_iGroup = {};

public:
    static CHiZ_Culling* Create();
    void Free() override;
};
NS_END
