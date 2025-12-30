cbuffer CB_Occlusion : register(b0)
{
    uint2 viewportSize;
    uint mipCount;
    float epsilon;

    uint inputCount;
    float padding0;
    float padding1;
    float padding2;
};

struct OcclusionInput /*개별 오브젝트 */
{
    uint minX, minY, maxX, maxY;
    float objMinDepth01; // LinearZ(0..1)에서 "가장 가까운" 값(작은 값)
    uint indexInList;
    uint padding;
};

Texture2D<float> HiZTex : register(t0);
StructuredBuffer<OcclusionInput> Inputs : register(t1);
RWStructuredBuffer<uint> VisibleFlags : register(u0);

uint ClampMip(uint mip)
{
    return (mip < mipCount) ? mip : (mipCount - 1);
}
[numthreads(64, 1, 1)]
void CS_MAIN(uint3 tid : SV_DispatchThreadID)
{
    uint inputIndex = tid.x;
    if (inputIndex >= inputCount)
        return;

    OcclusionInput inputData = Inputs[inputIndex];

    uint rectW = (inputData.maxX > inputData.minX) ? (inputData.maxX - inputData.minX) : 1;
    uint rectH = (inputData.maxY > inputData.minY) ? (inputData.maxY - inputData.minY) : 1;
    uint rectMax = (rectW > rectH) ? rectW : rectH;

    // 너무 큰 건 아예 보이게(안전장치)
    if (rectMax >= 1024)
    {
        VisibleFlags[inputIndex] = 1;
        return;
    }

    // mip 선택: rect가 클수록 더 거칠게(=max 놓칠 확률 ↓)
    int mipSel = (int) floor(log2((float) rectMax));
    if (rectMax >= 512)
        mipSel += 1;
    mipSel = max(mipSel, 0);

    uint mip = ClampMip((uint) mipSel);
    uint mipScale = 1u << mip;

    uint maxXInclusive = (inputData.maxX > 0) ? (inputData.maxX - 1) : 0;
    uint maxYInclusive = (inputData.maxY > 0) ? (inputData.maxY - 1) : 0;

    // mip 좌표계의 rect 최소/최대
    uint2 pMin = uint2(inputData.minX, inputData.minY) / mipScale;
    uint2 pMax = uint2(maxXInclusive, maxYInclusive) / mipScale;

    // mip 크기 클램프
    uint mipW, mipH, mipLevels;
    HiZTex.GetDimensions(mip, mipW, mipH, mipLevels);

    uint2 maxCoord = uint2(max(1u, mipW) - 1, max(1u, mipH) - 1);
    pMin = clamp(pMin, uint2(0, 0), maxCoord);
    pMax = clamp(pMax, uint2(0, 0), maxCoord);

    // 3x3 샘플 좌표(좌/중/우, 상/중/하)
    uint2 pMid = (pMin + pMax) >> 1;

    uint x0 = pMin.x, x1 = pMid.x, x2 = pMax.x;
    uint y0 = pMin.y, y1 = pMid.y, y2 = pMax.y;

    // 9점 샘플
    float z00 = HiZTex.Load(int3(uint2(x0, y0), mip));
    float z10 = HiZTex.Load(int3(uint2(x1, y0), mip));
    float z20 = HiZTex.Load(int3(uint2(x2, y0), mip));

    float z01 = HiZTex.Load(int3(uint2(x0, y1), mip));
    float z11 = HiZTex.Load(int3(uint2(x1, y1), mip));
    float z21 = HiZTex.Load(int3(uint2(x2, y1), mip));

    float z02 = HiZTex.Load(int3(uint2(x0, y2), mip));
    float z12 = HiZTex.Load(int3(uint2(x1, y2), mip));
    float z22 = HiZTex.Load(int3(uint2(x2, y2), mip));

    float hizMaxDepth01 =
        max(max(max(z00, z10), z20),
        max(max(max(z01, z11), z21),
        max(max(z02, z12), z22)));

    uint visible = (inputData.objMinDepth01 <= hizMaxDepth01 + epsilon) ? 1u : 0u;
    VisibleFlags[inputIndex] = visible;
}
