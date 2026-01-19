cbuffer CB_Occlusion : register(b0)
{
    uint2 viewportSize;
    uint mipCount;
    float epsilon;

    uint inputCount;
    uint padding;
    float padding1;
    float padding2;
};

struct OcclusionInput
{
    uint minX, minY, maxX, maxY; // [min, max) 픽셀 rect
    float objMinDepth01; // 0..1 선형, 작을수록 가까움
    uint indexInList;
    uint flags;
};

Texture2D<float> HiZTex : register(t0); // 전체 밉 SRV (Max-HiZ)
StructuredBuffer<OcclusionInput> Inputs : register(t1);
RWStructuredBuffer<uint> VisibleFlags : register(u0);

static const uint OCCL_FLAG_RISK_FLAT_OR_HUGE = 1u << 0;
static const uint OCCL_FLAG_RISK_GROUNDCONTACT = 1u << 1;

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

    OcclusionInput inData = Inputs[inputIndex];

    // 기본은 보이게
    uint visible = 1u;

    // ---- sanity / early-out ----
    if (inData.maxX <= inData.minX || inData.maxY <= inData.minY)
    {
        VisibleFlags[inputIndex] = 1u;
        return;
    }

    // 지면접촉 위험군은 통과(원하면 완화)
    if ((inData.flags & OCCL_FLAG_RISK_GROUNDCONTACT) != 0u)
    {
        VisibleFlags[inputIndex] = 1u;
        return;
    }

    uint rectW0 = inData.maxX - inData.minX;
    uint rectH0 = inData.maxY - inData.minY;
    uint rectMax0 = (rectW0 > rectH0) ? rectW0 : rectH0;

    // 너무 작거나 너무 큰 rect는 보수적으로 통과
    if (rectW0 < 1u || rectH0 < 1u)
    {
        VisibleFlags[inputIndex] = 1u;
        return;
    }
    if (rectMax0 <= 4u)
    {
        VisibleFlags[inputIndex] = 1u;
        return;
    }
    if (rectMax0 >= 300u)
    {
        VisibleFlags[inputIndex] = 1u;
        return;
    }

    int mipSel = (int) floor(log2((float) rectMax0)) - 1;
    mipSel = max(mipSel, 0);

    uint mip = ClampMip((uint) mipSel);
    mip = min(mip, 4u);

    uint mipScale = 1u << mip;

    // ---- inflate ----
    uint inflate = 2u + (mip > 0u ? 2u : 0u);

    int minX = (int) inData.minX - (int) inflate;
    int minY = (int) inData.minY - (int) inflate;
    int maxX = (int) inData.maxX + (int) inflate;
    int maxY = (int) inData.maxY + (int) inflate;

    minX = max(minX, 0);
    minY = max(minY, 0);
    maxX = min(maxX, (int) viewportSize.x);
    maxY = min(maxY, (int) viewportSize.y);

    if (maxX <= minX || maxY <= minY)
    {
        VisibleFlags[inputIndex] = 1u;
        return;
    }

    uint maxXInclusive = (uint) (maxX - 1);
    uint maxYInclusive = (uint) (maxY - 1);

    uint2 pMin = uint2((uint) minX, (uint) minY) / mipScale;
    uint2 pMax = uint2(maxXInclusive, maxYInclusive) / mipScale;

    uint mipW, mipH, mipLevels;
    HiZTex.GetDimensions(mip, mipW, mipH, mipLevels);

    uint2 maxCoord = uint2(max(1u, mipW) - 1u, max(1u, mipH) - 1u);
    pMin = clamp(pMin, uint2(0u, 0u), maxCoord);
    pMax = clamp(pMax, uint2(0u, 0u), maxCoord);

    uint2 pMid = (pMin + pMax) >> 1;

    uint x0 = pMin.x, x1 = pMid.x, x2 = pMax.x;
    uint y0 = pMin.y, y1 = pMid.y, y2 = pMax.y;

    float z00 = HiZTex.Load(int3(uint2(x0, y0), mip));
    float z10 = HiZTex.Load(int3(uint2(x1, y0), mip));
    float z20 = HiZTex.Load(int3(uint2(x2, y0), mip));
    float z01 = HiZTex.Load(int3(uint2(x0, y1), mip));
    float z11 = HiZTex.Load(int3(uint2(x1, y1), mip));
    float z21 = HiZTex.Load(int3(uint2(x2, y1), mip));
    float z02 = HiZTex.Load(int3(uint2(x0, y2), mip));
    float z12 = HiZTex.Load(int3(uint2(x1, y2), mip));
    float z22 = HiZTex.Load(int3(uint2(x2, y2), mip));

    // ---- Max-HiZ: 9개 중 최댓값 ----
    float hizMaxDepth01 = max(max(max(z00, z10), z20),
                          max(max(max(z01, z11), z21),
                          max(max(z02, z12), z22)));

    float obj = inData.objMinDepth01;

    float epsLocal = max(0.0f, epsilon * 0.5f);
    float biasLocal = 0.0010f + 0.0004f * (float) mip;

    if ((inData.flags & OCCL_FLAG_RISK_FLAT_OR_HUGE) != 0u)
    {
        biasLocal *= 2.0f;
        epsLocal *= 2.0f;
    }

    // 덜 가리게: obj를 조금 더 가깝게
    obj = max(0.0f, obj - biasLocal);
    
    if (obj > hizMaxDepth01 + epsLocal)
        visible = 0u;

    VisibleFlags[inputIndex] = visible;
}
