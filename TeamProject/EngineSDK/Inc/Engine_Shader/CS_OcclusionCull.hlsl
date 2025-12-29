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

struct OcclusionInput
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

    // mip 선택: 기존보다 1단계 더 고해상도(오검출 줄이기)
    int mipSel = (int) floor(log2((float) rectMax)) - 1;
    if (mipSel < 0)
        mipSel = 0;
    uint mip = ClampMip((uint) mipSel);

    uint mipScale = 1u << mip;

    uint maxXInclusive = (inputData.maxX > 0) ? (inputData.maxX - 1) : 0;
    uint maxYInclusive = (inputData.maxY > 0) ? (inputData.maxY - 1) : 0;

    uint2 p0 = uint2(inputData.minX, inputData.minY) / mipScale;
    uint2 p1 = uint2(maxXInclusive, inputData.minY) / mipScale;
    uint2 p2 = uint2(inputData.minX, maxYInclusive) / mipScale;
    uint2 p3 = uint2(maxXInclusive, maxYInclusive) / mipScale;
    uint2 pc = uint2((inputData.minX + maxXInclusive) >> 1,
                     (inputData.minY + maxYInclusive) >> 1) / mipScale;

    uint mipW, mipH, mipLevels;
    HiZTex.GetDimensions(mip, mipW, mipH, mipLevels);

    uint2 maxCoord = uint2(max(1u, mipW) - 1, max(1u, mipH) - 1);
    p0 = clamp(p0, uint2(0, 0), maxCoord);
    p1 = clamp(p1, uint2(0, 0), maxCoord);
    p2 = clamp(p2, uint2(0, 0), maxCoord);
    p3 = clamp(p3, uint2(0, 0), maxCoord);
    pc = clamp(pc, uint2(0, 0), maxCoord);

    // HiZ는 이제 "max LinearZ" 피라미드
    float z0 = HiZTex.Load(int3(p0, mip));
    float z1 = HiZTex.Load(int3(p1, mip));
    float z2 = HiZTex.Load(int3(p2, mip));
    float z3 = HiZTex.Load(int3(p3, mip));
    float zc = HiZTex.Load(int3(pc, mip));

    float hizMaxDepth01 = max(max(z0, z1), max(max(z2, z3), zc));

    // 판정 (보수적으로):
    // 오브젝트의 "가장 가까운 값"(작은 값)이
    // 타일의 "가장 먼 값"(큰 값)보다도 더 멀다(큰 값)면 -> 뒤에 있으니 가려졌다고 보기 쉬움
    // 반대로 더 가깝거나 비슷하면 -> visible
    uint visible = (inputData.objMinDepth01 <= hizMaxDepth01 + epsilon) ? 1u : 0u;
    VisibleFlags[inputIndex] = visible;
}
