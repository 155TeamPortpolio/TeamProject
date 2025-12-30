Texture2D<float4> InputDepth : register(t0);
RWTexture2D<float> OutputHiZ : register(u0);

cbuffer CB_Copy : register(b0)
{
    uint2 dstSize; /*뎁스 텍스처 크기*/
    uint2 paddingCopy;
};

[numthreads(8, 8, 1)]
void CS_MAIN(uint3 tid : SV_DispatchThreadID)
{
    uint2 p = tid.xy;
    if (p.x >= dstSize.x || p.y >= dstSize.y)
        return;

    float linearZ = InputDepth.Load(int3(p, 0)).z;
    OutputHiZ[p] = linearZ;
}