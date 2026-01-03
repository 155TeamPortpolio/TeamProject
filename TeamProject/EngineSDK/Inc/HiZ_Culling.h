#pragma once
#include "Base.h"

NS_BEGIN(Engine)
static constexpr _uint kFrameBuffered =8;
static constexpr _uint readLatency =7;

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
        _uint       minX, minY, maxX, maxY;     // ȭ�� �ȼ� rect
        _float      objMinDepth01;                       // min(viewZ / zFar) in [0,1]
        _uint       indexInList;                                // frustums ���� �ε���
        _uint       padding;
    };

    struct OcclusionKey
    {
        const void* worldPtr; 
        _uint drawIndex;      
    };

    struct OcclusionKeyHash
    {
        size_t operator()(const OcclusionKey& key) const noexcept
        {
            size_t h1 = hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(key.worldPtr));
            size_t h2 =hash<_uint>{}(key.drawIndex);
            return h1 ^ (h2 + 0x9e3779b97f4a7c15ull + (h1 << 6) + (h1 >> 2));
        }
    };

    struct OcclusionKeyEq
    {
        bool operator()(const OcclusionKey& a, const OcclusionKey& b) const noexcept
        {
            return a.worldPtr == b.worldPtr && a.drawIndex == b.drawIndex;
        }
    };
    struct OcclusionReadbackFrame
    {
        ID3D11Buffer* visibleBuffer = nullptr;              // UAV target
        ID3D11UnorderedAccessView* visibleUav = nullptr;    // UAV
        ID3D11Buffer* visibleStaging = nullptr;             // staging readback
        ID3D11Query* copyDoneQuery = nullptr;               // event query
        vector<OcclusionKey> keys;
        _bool hasIssued = false;
    };
    struct OcclusionHysteresisState
    {
        _uint hiddenStreak = 0;
    };
    enum OcclusionFlags : _uint
    {
        OCCL_FLAG_RISK_FLAT_OR_HUGE = 1u << 0, // ����/�Ŵ�(����/��/ū ������ ���ɼ�)
        OCCL_FLAG_RISK_GROUNDCONTACT = 1u << 1 // ���� ���� ����(����)
    };

#ifdef _USING_GUI
  
    struct HiZStats
    {
        _uint frustumIn = 0;       // frustum pass ���� ���� ��Ŷ ��
        _uint tested = 0;          // occlusion �׽�Ʈ�� ���� ��(inputs.size)
        _uint visibleByOcc = 0;    // occlusion ����� visible�� ������ ��
        _uint culledByOcc = 0;     // occlusion ����� occluded�� ������ ��
        _uint notTested = 0;       // BuildOcclusionInput ����/���� ������ �׽�Ʈ �� �� ��
        _uint outResult = 0;       // ���� result ��
        _uint canRead = 0;         // �̹� ������ readback ����(1/0)
    };
    _bool isTabOpen = { false };
    HiZStats m_stats; 
#endif

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

    _bool isQueryComplete(ID3D11DeviceContext* pContext, ID3D11Query* pQuery);
    void TryReadback(ID3D11DeviceContext* pContext, OcclusionReadbackFrame& readSlot, _uint capacity);
 
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
    vector<ID3D11UnorderedAccessView*> m_HiZUav;    /*�Ӻ� UAV��*/
    vector<ID3D11ShaderResourceView*> m_HiZSrvMip; /*�Ӹ� SRV��*/

    ID3D11ShaderResourceView* m_pDepthSrv = { nullptr };
    class CComputeShader* m_pCopyShader = { nullptr };
    class CComputeShader* m_pReduceShader = { nullptr };
    class CComputeShader* m_pOcclusionShader = { nullptr };

    ID3D11Buffer* m_pCopyBuffer = { nullptr };
    ID3D11Buffer* m_pReduceBuffer = { nullptr };
    ID3D11Buffer* m_pOcculsionBuffer = { nullptr };

private:
    ID3D11Buffer* m_inputBuffer = { nullptr };       
    ID3D11ShaderResourceView* m_inputSrv = { nullptr };           // Inputs ���۸� �д� SRV (t1)
    _uint                   m_capacity = 0;                                             // ���� ���۰� ���� ������ �ִ� element ����

private:
    ID3D11ShaderResourceView* m_pHiZSrv = { nullptr };

 private :
    OcclusionReadbackFrame m_readbackFrames[kFrameBuffered];
    //ID3D11Buffer* m_visibleBuffer = { nullptr };                                    // VisibleFlags�� GPU ����
   //ID3D11UnorderedAccessView* m_visibleUav = { nullptr };         // VisibleFlags�� ���� UAV (u0)
   //ID3D11Buffer* m_visibleStaging = { nullptr };                   // CPU readback�� staging

    _uint m_frameCursor = 0;                               // �� ������ ����
    vector<_uint> m_cachedVisibleFlags;         // ���������� ������ ���(���� ȸ�ǿ�)
    vector<OcclusionKey> m_cachedKeys;    // last read keys

    unordered_map<OcclusionKey, OcclusionHysteresisState, OcclusionKeyHash, OcclusionKeyEq> m_hysteresis;
public:
    static CHiZ_Culling* Create();
    void Free() override;
};
NS_END
