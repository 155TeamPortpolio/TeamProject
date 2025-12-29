#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CHiZ_Culling :
    public CBase
{
    struct CB_CopyData
    {
        _uint2 dstSize;
        _uint2 padding;
    };

    struct CB_ReduceData
    {
        _uint2 srcSize;
        _uint2 dstSize;
        _uint srcMip;
        _uint3 padding;
    };

    struct OcclusionInput
    {
        _uint       minX, minY, maxX, maxY;     // »≠∏È «»ºø rect
        _float      objMinDepth01;                       // min(viewZ / zFar) in [0,1]
        _uint       indexInList;                                // frustums ∫§≈Õ ¿Œµ¶Ω∫
        _uint       padding;
    };

private:
    CHiZ_Culling();
    ~CHiZ_Culling() DEFAULT;
public:
    HRESULT Initialize();
    void Update_HiZ(ID3D11DeviceContext* pContext);

public:
    vector<OPAQUE_PACKET> OcculsionCulling(const vector<OPAQUE_PACKET>& frustums);

#ifdef _USING_GUI
    void Render_GUI();
#endif // _USING_GUI

private:
    void Check_Resource();
    _uint CalcMipCount(_uint width, _uint height);
    ID3D11Buffer* CreateDynamicCB(ID3D11Device* device, _uint byteSize);
    void Update_CBuffer(ID3D11DeviceContext* ctx, ID3D11Buffer* cb, const void* data, UINT size);
    _float Clamp01(_float value);
    _bool BuildOcclusionInput(const MINMAX_BOX& localAabbMinMax,_fmatrix worldMatrix, _fmatrix viewMatrix,_uint viewportW,_uint viewportH,_float zFar, _uint indexInList,
        OcclusionInput& outInput);
private:
    _bool m_isReady = { false };
    _int m_DebugMip = { 0 };
    _float2 m_viewport = {};
    _uint m_mipCount = {};
    _uint2 m_texSize = {};
    _uint3 m_threadSize = {};
    _uint3 m_groupCount = {};

    ID3D11Texture2D* m_pHiZTex = { nullptr };
    vector<ID3D11UnorderedAccessView*> m_HiZUav;    /*π”∫∞ UAVµÈ*/
    vector<ID3D11ShaderResourceView*> m_HiZSrvMip; /*π”∏  SRVµÈ*/

    ID3D11ShaderResourceView* m_pDepthSrv = { nullptr };
    class CComputeShader* m_pCopyShader = { nullptr };
    class CComputeShader* m_pReduceShader = { nullptr };

    ID3D11Buffer* m_pCopyBuffer = { nullptr };
    ID3D11Buffer* m_pReduceBuffer = { nullptr };
private:
    _uint3 m_iGroup = {};

public:
    static CHiZ_Culling* Create();
    void Free() override;
};
NS_END
