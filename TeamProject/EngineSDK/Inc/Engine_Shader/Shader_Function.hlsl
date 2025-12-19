#ifndef __SHADER_FUNCTION_HLSL__
#define __SHADER_FUNCTION_HLSL__

float2 CalculateFrameIndex(uint Col, uint Row, uint FrameIndex)
{
    float2 FrameSize = float2(1.f / Col, 1.f / Row);
    
    int iFrameX = FrameIndex % Col;
    int iFrameY = FrameIndex / Col;
    
    float2 Frame = float2(iFrameX, iFrameY) * FrameSize;
  
    return Frame;
}

//Thereshold - 어느정도 밝기 이상 추출할건지 (높을수록 진짜 밝은것만 낮을수록 더 많이)
//Softness - 부드러운정도
//Intensity - 밝기
float4 ExtractBright(float4 color, float Thereshold = 1.f, float Softness = 0.5f, float Intensity = 1.f)
{
    float Bright = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    
    if (Bright < Thereshold)
        return float4(0, 0, 0, 0);
    
    float contribution = saturate((Bright - Thereshold) / Softness);
    
    return color * contribution * Intensity;
}

#endif