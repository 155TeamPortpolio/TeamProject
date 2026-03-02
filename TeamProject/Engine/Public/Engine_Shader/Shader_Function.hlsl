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

float3 UchimuraTonemap(float3 x, float P, float a, float m, float l, float c, float b)
{
    // P = max brightness, a = contrast, m = linear section start
    // l = linear section length, c = black, b = pedestal
    float l0 = ((P - m) * l) / a;
    float L0 = m - m / a;
    float L1 = m + (1.0f - m) / a;
    float S0 = m + l0;
    float S1 = m + a * l0;
    float C2 = (a * P) / (P - S1);
    float CP = -C2 / P;

    float3 w0 = 1.0f - smoothstep(0.0f, m, x);
    float3 w2 = step(m + l0, x);
    float3 w1 = 1.0f - w0 - w2;

    float3 T = m * pow(x / m, c) + b;
    float3 S = P - (P - S1) * exp(CP * (x - S0));
    float3 L = m + a * (x - m);

    return T * w0 + L * w1 + S * w2;
}

float3 UchimuraTonemap(float3 color)
{
    const float P = 1.0f; // max brightness
    const float a = 1.0f; // contrast
    const float m = 0.22f; // linear section start
    const float l = 0.4f; // linear section length
    const float c = 1.33f; // black tightness
    const float b = 0.0f; // pedestal
    
    return UchimuraTonemap(color, P, a, m, l, c, b);
}

float3 ZZZStyleTonemap(float3 color)
{
    color *= 1.2f;
    
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    color = saturate((color * (a * color + b)) / (color * (c * color + d) + e));

    float luminance = dot(color, float3(0.299f, 0.587f, 0.114f));
    color = lerp(luminance, color, 1.15f);
    
    return color;
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