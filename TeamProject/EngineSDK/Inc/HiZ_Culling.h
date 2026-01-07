#pragma once
#include "Base.h"

NS_BEGIN(Engine)
static constexpr _uint kFrameBuffered =8;
static constexpr _uint readLatency =7;
static constexpr _uint hideAfter = 3;
static constexpr _uint showAfter = 2;

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
        _uint ObjID = 0;
        _uint drawIndex = 0;
    };

    struct OcclusionKeyHash
    {
        size_t operator()(const OcclusionKey& key) const noexcept
        {
            uint64_t packed = (uint64_t(key.ObjID) << 32) | uint64_t(key.drawIndex);
            return hash<uint64_t>{}(packed);
        }
    };

    struct OcclusionKeyEq
    {
        bool operator()(const OcclusionKey& lhs, const OcclusionKey& rhs) const noexcept
        {
            return lhs.ObjID == rhs.ObjID && lhs.drawIndex == rhs.drawIndex;
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
        _uint issuedFrame = 0;
    };
    struct OcclusionHysteresisState
    {
        _uint hiddenStreak = 0;
        uint8_t visibleStreak = 0;
        bool isHidden = false;
    };
    enum OcclusionFlags : _uint
    {
        OCCL_FLAG_RISK_FLAT_OR_HUGE = 1u << 0, // ����/�Ŵ�(����/��/ū ������ ���ɼ�)
        OCCL_FLAG_RISK_GROUNDCONTACT = 1u << 1 // ���� ���� ����(����)
    };

#ifdef _USING_GUI
    struct OcclDebugRect
    {
        float minX, minY, maxX, maxY;   // screen pixels
        float depth01;                  // objMinDepth01
        _uint flags;                 // outInput.padding
        _uint drawIndex;
    };
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
    vector<OcclDebugRect> m_dbgRects;
    bool m_dbgDrawRects = true;
    int  m_dbgRectLimit = 2000;
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
    _bool TryReadbackOne(ID3D11DeviceContext* pContext, OcclusionReadbackFrame& slot);
 
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
    _uint m_frameCounter = {};
    _uint m_cachedFrame = {};

    vector<_uint> m_cachedVisibleFlags;         // ���������� ������ ���(���� ȸ�ǿ�)
    vector<OcclusionKey> m_cachedKeys;    // last read keys

    unordered_map<OcclusionKey, OcclusionHysteresisState, OcclusionKeyHash, OcclusionKeyEq> m_hysteresis;
public:
    static CHiZ_Culling* Create();
    void Free() override;
};
NS_END
