struct OcclusionInput
{
    uint minX, minY, maxX, maxY;
    float objMinDepth01;
    uint indexInList;
    uint pad;
};

Texture2D<float> HiZTex : register(t0);
StructuredBuffer<OcclusionInput> Inputs : register(t1);
RWStructuredBuffer<uint> VisibleFlags : register(u0);

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

    // mip 선택: 화면 크기 기반
    // log2는 float이니 근사
    float mipFloat = floor(log2((float) rectMax));
    uint mip = ClampMip((uint) mipFloat);

    // mip 해상도에서의 좌표로 변환
    uint mipScale = 1u << mip;

    uint2 p0 = uint2(inputData.minX, inputData.minY) / mipScale;
    uint2 p1 = uint2(inputData.maxX, inputData.minY) / mipScale;
    uint2 p2 = uint2(inputData.minX, inputData.maxY) / mipScale;
    uint2 p3 = uint2(inputData.maxX, inputData.maxY) / mipScale;
    uint2 pc = uint2((inputData.minX + inputData.maxX) >> 1, (inputData.minY + inputData.maxY) >> 1) / mipScale;

    // 해당 mip의 크기
    uint mipW, mipH, mipLevels;
    HiZTex.GetDimensions(mip, mipW, mipH, mipLevels);

    p0 = clamp(p0, uint2(0, 0), uint2(max(1u, mipW) - 1, max(1u, mipH) - 1));
    p1 = clamp(p1, uint2(0, 0), uint2(max(1u, mipW) - 1, max(1u, mipH) - 1));
    p2 = clamp(p2, uint2(0, 0), uint2(max(1u, mipW) - 1, max(1u, mipH) - 1));
    p3 = clamp(p3, uint2(0, 0), uint2(max(1u, mipW) - 1, max(1u, mipH) - 1));
    pc = clamp(pc, uint2(0, 0), uint2(max(1u, mipW) - 1, max(1u, mipH) - 1));

    // HiZ는 "min depth" 피라미드라고 가정
    float z0 = HiZTex.Load(int3(p0, mip));
    float z1 = HiZTex.Load(int3(p1, mip));
    float z2 = HiZTex.Load(int3(p2, mip));
    float z3 = HiZTex.Load(int3(p3, mip));
    float zc = HiZTex.Load(int3(pc, mip));

    float hizMinDepth01 = min(min(z0, z1), min(min(z2, z3), zc));

    // objMinDepth01이 hizMin보다 "더 멀면"(값이 크면) 가려졌다고 판단
    uint visible = (inputData.objMinDepth01 <= hizMinDepth01 + epsilon) ? 1u : 0u;
    VisibleFlags[inputIndex] = visible;
}
