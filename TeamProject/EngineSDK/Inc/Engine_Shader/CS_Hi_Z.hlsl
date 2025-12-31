Texture2D<float> HiZSrc : register(t0); // HiZ SRV (전체 mip)
RWTexture2D<float> HiZDst : register(u0); // HiZ UAV (특정 mip)

cbuffer DownCB : register(b0)
{
    uint SrcMip;
    uint SrcWidth;
    uint SrcHeight;
    uint Pad;
};

[numthreads(8, 8, 1)]
void CS_MAIN(uint3 tid : SV_DispatchThreadID)
{
    uint2 dst = tid.xy;

    uint dstW = max(1, SrcWidth >> 1);
    uint dstH = max(1, SrcHeight >> 1);
    if (dst.x >= dstW || dst.y >= dstH)
        return;

    uint2 base = dst * 2;

    uint2 p0 = base;
    uint2 p1 = uint2(min(base.x + 1, SrcWidth - 1), base.y);
    uint2 p2 = uint2(base.x, min(base.y + 1, SrcHeight - 1));
    uint2 p3 = uint2(min(base.x + 1, SrcWidth - 1), min(base.y + 1, SrcHeight - 1));

    float d0 = HiZSrc.Load(int3(p0, SrcMip));
    float d1 = HiZSrc.Load(int3(p1, SrcMip));
    float d2 = HiZSrc.Load(int3(p2, SrcMip));
    float d3 = HiZSrc.Load(int3(p3, SrcMip));

    HiZDst[dst] = min(min(d0, d1), min(d2, d3));
}
