
Texture2D<float4> InputTex : register(t0); // SRV -> Float4짜리로 읽는다.
RWTexture2D<float> OutputTex : register(u0); // UAV

[numthreads(8, 8, 1)]
void CS_MAIN(uint3 pixelID : SV_DispatchThreadID)
{
    uint2 pixel = pixelID.xy; //좌표
    float value = InputTex.Load(int3(pixel, 0)).z;  //mip level 0 / 내가 z에저장한 값을 읽을 거임
    OutputTex[pixel] = value; //그 값을 기록할 거임
}