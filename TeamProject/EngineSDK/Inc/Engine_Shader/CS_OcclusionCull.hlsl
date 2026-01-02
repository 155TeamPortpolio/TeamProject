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
    uint flags; // C++에서 플래그 전달 (기존 padding 자리)
};

Texture2D<float> HiZTex : register(t0); // 전체 밉 SRV
StructuredBuffer<OcclusionInput> Inputs : register(t1);
RWStructuredBuffer<uint> VisibleFlags : register(u0);

// flags bits (C++와 동일하게 맞추기)
static const uint OCCL_FLAG_RISK_FLAT_OR_HUGE = 1u << 0; // 평평/거대 위험군
static const uint OCCL_FLAG_RISK_GROUNDCONTACT = 1u << 1; // 지면접촉 추정(선택)

// 1: visible, 2: occluded
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

    // ---- sanity ----
    if (inData.maxX <= inData.minX || inData.maxY <= inData.minY)
    {
        VisibleFlags[inputIndex] = 1;
        return;
    }
    if (inData.flags & OCCL_FLAG_RISK_GROUNDCONTACT)
    {
        VisibleFlags[inputIndex] = 1;
        return;
    }
    uint rectW0 = inData.maxX - inData.minX;
    uint rectH0 = inData.maxY - inData.minY;
    uint rectMax0 = (rectW0 > rectH0) ? rectW0 : rectH0;

    if (rectW0 < 1 || rectH0 < 1)
    {
        VisibleFlags[inputIndex] = 1;
        return;
    }

    // 큰 rect는 태그 없이도 위험하므로 보수적으로 visible
    // (너 씬에 맞춰 256/512/1024로 튜닝)
    if (rectMax0 >= 256)
    {
        VisibleFlags[inputIndex] = 1;
        return;
    }
    if (rectMax0 <= 4u)
    {
        VisibleFlags[inputIndex] = 1;
        return;
    } // 2~8 사이 튜닝

    // ---- mip 선택 (원본 rect 기준) ----
    int mipSel = (int) ceil(log2((float) rectMax0));
    mipSel = max(mipSel, 0);

    uint mip = ClampMip((uint) mipSel);

    // mip 상한(큰 밉일수록 지면 영향이 커짐)
    mip = min(mip, 4u);

    uint mipScale = 1u << mip;

    // ---- mip 기반 inflate (샘플 안정화용: mip 재계산 X) ----
    uint inflate = 2u + (1u << mip);

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
        VisibleFlags[inputIndex] = 1;
        return;
    }

    // inclusive max
    uint maxXInclusive = (uint) (maxX - 1);
    uint maxYInclusive = (uint) (maxY - 1);

    uint2 pMin = uint2((uint) minX, (uint) minY) / mipScale;
    uint2 pMax = uint2(maxXInclusive, maxYInclusive) / mipScale;

    uint mipW, mipH, mipLevels;
    HiZTex.GetDimensions(mip, mipW, mipH, mipLevels);

    uint2 maxCoord = uint2(max(1u, mipW) - 1, max(1u, mipH) - 1);
    pMin = clamp(pMin, uint2(0, 0), maxCoord);
    pMax = clamp(pMax, uint2(0, 0), maxCoord);

    // 3x3 샘플(코너/중앙)
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

    float hizMinDepth01 =
        min(min(min(z00, z10), z20),
        min(min(min(z01, z11), z21),
        min(min(z02, z12), z22)));

    // ---- bias/epsilon (위험군은 더 보수적으로) ----
    float obj = inData.objMinDepth01;
   
    float epsLocal = epsilon;
    float biasLocal = 0.003f + 0.0015f * mip;

    // 위험군(평평/거대)일 때는 "잘 안 가리게"
    if ((inData.flags & OCCL_FLAG_RISK_FLAT_OR_HUGE) != 0u)
    {
        // 너무 세게 보수 처리하고 싶으면 여기서 바로 visible 처리 가능
        // VisibleFlags[inputIndex] = 1; return;

        biasLocal *= 3.0f;
        epsLocal *= 3.0f;
    }

    // 지면접촉 추정이면 추가로 더 보수 처리
    if ((inData.flags & OCCL_FLAG_RISK_GROUNDCONTACT) != 0u)
    {
        biasLocal += 0.01f;
        epsLocal *= 2.0f;
    }

    obj += biasLocal;

    // ---- 판정 (min-pyramid) ----
    uint visible = 1;
    if (obj > hizMinDepth01 + epsLocal)
        visible = 2;

    VisibleFlags[inputIndex] = visible;
}
