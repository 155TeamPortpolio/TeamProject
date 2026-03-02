Texture2D<float> InputHiZ : register(t0);
RWTexture2D<float> OutputHiZ : register(u0);

cbuffer CB_Reduce : register(b0)
{
    uint2 srcSize;
    uint2 dstSize;
    uint srcMip;
};

[numthreads(8, 8, 1)]
void CS_MAIN(uint3 tid : SV_DispatchThreadID)
{
    uint2 dst = tid.xy;
    if (dst.x >= dstSize.x || dst.y >= dstSize.y)
        return;

    uint2 srcBase = dst * 2;

    uint2 p0 = min(srcBase + uint2(0, 0), srcSize - 1);
    uint2 p1 = min(srcBase + uint2(1, 0), srcSize - 1);
    uint2 p2 = min(srcBase + uint2(0, 1), srcSize - 1);
    uint2 p3 = min(srcBase + uint2(1, 1), srcSize - 1);

    float z0 = InputHiZ.Load(int3(p0, 0));
    float z1 = InputHiZ.Load(int3(p1, 0));
    float z2 = InputHiZ.Load(int3(p2, 0));
    float z3 = InputHiZ.Load(int3(p3, 0));

    // LinearZ(near=0, far=1) 기준: MAX 피라미드
    OutputHiZ[dst] = max(max(z0, z1), max(z2, z3));
}
