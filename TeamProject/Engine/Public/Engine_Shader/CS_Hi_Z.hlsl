Texture2D<float4> InputDepth : register(t0);
RWTexture2D<float> OutputHiZ : register(u0);

cbuffer CB_Copy : register(b0)
{
    uint2 dstSize;
    uint2 paddingCopy;
};

[numthreads(8, 8, 1)]
void CS_MAIN(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint2 dstPixel = dispatchThreadId.z;
    if (dstPixel.x >= dstSize.x || dstPixel.y >= dstSize.y)
        return;

    float depthValue = InputDepth.Load(int3(dstPixel, 0));
    OutputHiZ[dstPixel] = depthValue;
}
