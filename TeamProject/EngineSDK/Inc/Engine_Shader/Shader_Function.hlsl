#ifndef __SHADER_FUNCTION_HLSL__
#define __SHADER_FUNCTION_HLSL__

float2 CalculateFrameIndex(uint Col, uint Row, uint FrameIndex, float2 InTexcoord)
{
    float2 FrameSize = float2(1.f / Col, 1.f / Row);
    
    int iFrameX = FrameIndex % Col;
    int iFrameY = FrameIndex / Col;
    float2 FrameMin = float2(iFrameX, iFrameY) * FrameSize;
    
    float2 OutTexcoord = FrameMin + InTexcoord * FrameSize;

    return OutTexcoord;
}

//Thereshold - 어느정도 밝기 이상 추출할건지 (높을수록 진짜 밝은것만 낮을수록 더 많이)
//Softness - 부드러운정도
//Intensity - 밝기
float4 ExtractBright(float4 color, float Thereshold =2.f, float Softness = 0.5f, float Intensity = 1.f)
{
    float Bright = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    
    if (Bright < Thereshold)
        return float4(0, 0, 0, 0);
    
    float contribution = saturate((Bright - Thereshold) / Softness);
    
    return color * contribution * Intensity;
}

float4 SoftExtractBright(float4 color, float Threshold = 1.f, float Softness = 0.5f, float Intensity = 1.f)
{
    float brightness = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    
    float knee = Threshold * Softness;
    float soft = brightness - Threshold + knee;
    soft = clamp(soft, 0.0, 2.0 * knee);
    soft = soft * soft / (4.0 * knee + 0.00001);
    
    float contribution = max(soft, brightness - Threshold);
    contribution /= max(brightness, 0.00001);
    
    return color * contribution * Intensity;
}

float3 ACESFilm(float3 color)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    
    return saturate((color * (a * color + b)) / (color * (c * color + d) + e));
}

float4 ApplyColorMode(uint ColorMode, float4 Diffuse, float4 Color)
{
    float4 vResult = Diffuse;
    
    if (0 == ColorMode) /* Multiply */
    {
        vResult = Diffuse * Color;
        vResult.a = Diffuse.a * Color.a;
    }
    else if(1 == ColorMode) /* Additive */
    {
        vResult = Diffuse + Color;
        vResult.a = Diffuse.a * Color.a;
    }
    else
    {
        vResult = Diffuse;
    }
    
    return vResult;
}
#endif