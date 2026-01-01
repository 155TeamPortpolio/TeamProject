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
    struct CB_OcclusionData
    {
        _uint2 viewportSize; 
        _uint  mipCount;       
        _float epsilon;        

        _uint  inputCount;   
        _float3 padding0;     
    };

    struct OcclusionInput
    {
        _uint       minX, minY, maxX, maxY;     // 화면 픽셀 rect
        _float      objMinDepth01;                       // min(viewZ / zFar) in [0,1]
        _uint       indexInList;                                // frustums 벡터 인덱스
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
    ID3D11Buffer* CreateDynamicCB(ID3D11Device* device, _uint byteSize);
    void Update_CBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const void* data, _uint size);
    _float Clamp01(_float value);
    _uint CalcMipCount(_uint width, _uint height);
    
    _bool BuildOcclusionInput(const MINMAX_BOX& localAabbMinMax,_fmatrix worldMatrix, _fmatrix viewMatrix,_uint viewportW,_uint viewportH,_float zFar, _uint indexInList,
        OcclusionInput& outInput);

    void EnsureOcclusionResources(ID3D11Device* pDevice, _uint requiredCount);

private:
    _bool m_isReady = { false };
    _int m_DebugMip = { 0 };
    _uint m_mipCount = {};
    _uint3 m_iGroup = {};
    _float2 m_viewport = {};
    _uint2 m_texSize = {};
    _uint3 m_threadSize = {};
    _uint3 m_groupCount = {};

    ID3D11Texture2D* m_pHiZTex = { nullptr };
    vector<ID3D11UnorderedAccessView*> m_HiZUav;    /*밉별 UAV들*/
    vector<ID3D11ShaderResourceView*> m_HiZSrvMip; /*밉맵 SRV들*/

    ID3D11ShaderResourceView* m_pDepthSrv = { nullptr };
    class CComputeShader* m_pCopyShader = { nullptr };
    class CComputeShader* m_pReduceShader = { nullptr };
    class CComputeShader* m_pOcclusionShader = { nullptr };

    ID3D11Buffer* m_pCopyBuffer = { nullptr };
    ID3D11Buffer* m_pReduceBuffer = { nullptr };
    ID3D11Buffer* m_pOcculsionBuffer = { nullptr };

private:
    ID3D11Buffer* m_inputBuffer = { nullptr };       
    ID3D11ShaderResourceView* m_inputSrv = { nullptr };           // Inputs 버퍼를 읽는 SRV (t1)

    ID3D11Buffer* m_visibleBuffer = { nullptr };                                    // VisibleFlags용 GPU 버퍼
    ID3D11UnorderedAccessView* m_visibleUav = { nullptr };         // VisibleFlags에 쓰는 UAV (u0)

    ID3D11Buffer* m_visibleStaging = { nullptr };                   // CPU readback용 staging
    _uint                   m_capacity = 0;                                             // 현재 버퍼가 수용 가능한 최대 element 개수

private:
    ID3D11ShaderResourceView* m_pHiZSrv = { nullptr };

public:
    static CHiZ_Culling* Create();
    void Free() override;
};
NS_END
