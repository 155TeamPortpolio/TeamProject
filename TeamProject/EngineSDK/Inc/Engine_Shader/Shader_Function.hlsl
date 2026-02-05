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
    if (any(isnan(color)) || any(isinf(color)))
        return float4(0, 0, 0, 0);
    
    color = clamp(color, 0.0f, 100.0f);

    float brightness = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    brightness = max(brightness, 0.0f);

    Threshold = max(Threshold, 0.001f);
    Softness = clamp(Softness, 0.0f, 1.0f);
    Intensity = clamp(Intensity, 0.0f, 10.0f);
    
    float knee = Threshold * Softness;
    knee = max(knee, 0.00001f);

    float soft = brightness - Threshold + knee;
    soft = clamp(soft, 0.0, 2.0 * knee);
    soft = soft * soft / (4.0 * knee + 0.00001);

    float contribution = max(soft, brightness - Threshold);

    float safeBrightness = max(brightness, 0.001f);
    contribution /= safeBrightness;

    contribution = clamp(contribution, 0.0f, 10.0f);

    float4 result = color * contribution * Intensity;

    result = clamp(result, 0.0f, 50.0f);
    
    if (any(isnan(result)) || any(isinf(result)))
        return float4(0, 0, 0, 0);
    
    return result;
}

float3 ACESFilm(float3 color)
{
    float A = 0.15f;
    float B = 0.50f;
    float C = 0.10f;
    float D = 0.20f;
    float E = 0.02f;
    float F = 0.30f;
    return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
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