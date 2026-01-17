Texture2D<float4> SourceTex : register(t0);
RWStructuredBuffer<uint> GlobalFlags : register(u0); 

cbuffer CBInspect : register(b0)
{
    uint2 textureSize;
    uint mipIndex;
    float epsilon;
    uint pad0;
};

[numthreads(8, 8, 1)]
void CS_MAIN(uint3 dispatchId : SV_DispatchThreadID)
{
    uint x = dispatchId.x;
    uint y = dispatchId.y;

    if (x >= textureSize.x || y >= textureSize.y)
        return;

    float a = saturate(SourceTex.Load(int3((int) x, (int) y, (int) mipIndex)).a);

    if (a < (1.0 - epsilon))
        InterlockedOr(GlobalFlags[0], 1u);
}
