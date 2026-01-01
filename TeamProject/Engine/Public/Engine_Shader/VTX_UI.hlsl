#include "Shader_Define.hlsl"

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vWorldPos : POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), ObjectBufferArray[TransformIndex].Transform);
   
    return Out;
}

struct GS_IN
{
    float4 vWorldPos : POSITION;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> triStream)
{
    GS_OUT v[4];

    float3 worldPos = In[0].vWorldPos.xyz;
    
    float3 right = normalize(ObjectBufferArray[TransformIndex].Transform[0].xyz);
    float3 up = normalize(ObjectBufferArray[TransformIndex].Transform[1].xyz);
    float scaleX = length(ObjectBufferArray[TransformIndex].Transform[0].xyz);
    float scaleY = length(ObjectBufferArray[TransformIndex].Transform[1].xyz);

    float3 offsetRight = right * ( scaleX*0.5f);
    float3 offsetUp = up * (scaleY * 0.5f);

    // 정점 4개 위치 계산 (월드 기준)
    float3 p0 = worldPos + (-offsetRight + offsetUp);
    float3 p1 = worldPos + (offsetRight + offsetUp);
    float3 p2 = worldPos + (offsetRight - offsetUp);
    float3 p3 = worldPos + (-offsetRight - offsetUp);

    // 직교 투영 사용
    v[0].vPosition = mul(float4(p0, 1.f), matOrthograph);
    v[0].vTexcoord = float2(0, 0);

    v[1].vPosition = mul(float4(p1, 1.f), matOrthograph);
    v[1].vTexcoord = float2(1, 0);

    v[2].vPosition = mul(float4(p2, 1.f), matOrthograph);
    v[2].vTexcoord = float2(1, 1);

    v[3].vPosition = mul(float4(p3, 1.f), matOrthograph);
    v[3].vTexcoord = float2(0, 1);

    triStream.Append(v[0]);
    triStream.Append(v[1]);
    triStream.Append(v[2]);
    triStream.RestartStrip();

    triStream.Append(v[0]);
    triStream.Append(v[2]);
    triStream.Append(v[3]);
    triStream.RestartStrip();
}

uint Col;
uint Row;
uint FrameIndex;

float2 UVOffset;
 
float FillAmount;
float Direction;

float4 vColor;

float2 vFlip;

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    float2 vTexcoord = { In.vTexcoord.x * (1.f - 2.f * vFlip.x) + vFlip.x, In.vTexcoord.y * (1.f - 2.f * vFlip.y) + vFlip.y };
    
    vector vDiffuse = SpriteTexture.Sample(LinearSampler, vTexcoord);
    if (vDiffuse.a < 0.1f)
        discard;
    
    float3 vColorLinear = pow(vColor.rgb, 2.2f);
    Out.vColor = vDiffuse * float4(vColorLinear, vColor.a);
    
    return Out;
}

PS_OUT PS_MAIN_SPRITEANIMATION(PS_IN In)
{    
    PS_OUT Out;
    
    vector vDiffuse = SpriteTexture.Sample(LinearSampler, CalculateFrameIndex(Col, Row, FrameIndex, In.vTexcoord));
    if (vDiffuse.a < 0.1f)
        discard;
    
    float3 vColorLinear = pow(vColor.rgb, 2.2f);
    Out.vColor = vDiffuse * float4(vColorLinear, vColor.a);
    
    return Out;
}

PS_OUT PS_MAIN_UVANIMATION(PS_IN In)
{
    PS_OUT Out;
    
    vector vDiffuse = SpriteTexture.Sample(LinearSampler, In.vTexcoord + UVOffset);
    if (vDiffuse.a < 0.1f)
        discard;
    
    float3 vColorLinear = pow(vColor.rgb, 2.2f);
    Out.vColor = vDiffuse * float4(vColorLinear, vColor.a);
    
    return Out;
}
// ---------------------------------------------
float  MaskThreshold;
float  MaskSoftness;
float2 MaskTexSize;

PS_OUT PS_MAIN_UVANIMATION_MASK(PS_IN In)
{
    PS_OUT Out;

    float2 diffuseUV = float2(
        In.vTexcoord.x * (1.f - 2.f * vFlip.x) + vFlip.x,
        In.vTexcoord.y * (1.f - 2.f * vFlip.y) + vFlip.y
    );

    vector vDiffuse = SpriteTexture.Sample(LinearSampler, diffuseUV + UVOffset);
    clip(vDiffuse.a - 0.1f);

    float uiW = length(ObjectBufferArray[TransformIndex].Transform[0].xyz);
    float uiH = length(ObjectBufferArray[TransformIndex].Transform[1].xyz);
    float2 uiSizePx = float2(uiW, uiH);

    float2 localUV = In.vTexcoord;
    float2 localPx = (localUV - 0.5f) * uiSizePx;

    float2 rightDir = normalize(ObjectBufferArray[TransformIndex].Transform[0].xy);
    float2 upDir = normalize(ObjectBufferArray[TransformIndex].Transform[1].xy);

    float2 worldDelta = rightDir * localPx.x + upDir * localPx.y;

    float2 maskPx = worldDelta + 0.5f * MaskTexSize;
    float2 maskUV = maskPx / MaskTexSize;

    float mask = 0.f;
    if (maskUV.x >= 0.f && maskUV.x <= 1.f && maskUV.y >= 0.f && maskUV.y <= 1.f)
        mask = MaskTex.Sample(LinearSampler, maskUV).r;

    if (MaskSoftness <= 0.f)
        clip(mask - MaskThreshold);
    else
    {
        float maskA = smoothstep(MaskThreshold - MaskSoftness, MaskThreshold + MaskSoftness, mask);
        vDiffuse.a *= maskA;
    }

    float3 vColorLinear = pow(vColor.rgb, 2.2f);
    Out.vColor = vDiffuse * float4(vColorLinear, vColor.a);
    
    return Out;
}

PS_OUT PS_MAIN_MASK_DEBUG_RAW(PS_IN In)
{
    PS_OUT Out;

    float uiW = length(ObjectBufferArray[TransformIndex].Transform[0].xyz);
    float uiH = length(ObjectBufferArray[TransformIndex].Transform[1].xyz);
    float2 uiSizePx = float2(uiW, uiH);

    float2 localUV = In.vTexcoord;
    float2 localPx = (localUV - 0.5f) * uiSizePx;

    float2 rightDir = normalize(ObjectBufferArray[TransformIndex].Transform[0].xy);
    float2 upDir = normalize(ObjectBufferArray[TransformIndex].Transform[1].xy);

    float2 worldDelta = rightDir * localPx.x + upDir * localPx.y;

    float2 maskPx = worldDelta + 0.5f * MaskTexSize;
    float2 maskUV = maskPx / MaskTexSize;

    float mask = 0.f;
    if (maskUV.x >= 0.f && maskUV.x <= 1.f && maskUV.y >= 0.f && maskUV.y <= 1.f)
        mask = MaskTex.Sample(LinearSampler, maskUV).r;

    Out.vColor = float4(mask, mask, mask, 1.f);
    return Out;
}

PS_OUT PS_MAIN_MASK_DEBUG_APPLIED(PS_IN In)
{
    PS_OUT Out;

    float uiW = length(ObjectBufferArray[TransformIndex].Transform[0].xyz);
    float uiH = length(ObjectBufferArray[TransformIndex].Transform[1].xyz);
    float2 uiSizePx = float2(uiW, uiH);

    float2 localUV = In.vTexcoord;
    float2 localPx = (localUV - 0.5f) * uiSizePx;

    float2 rightDir = normalize(ObjectBufferArray[TransformIndex].Transform[0].xy);
    float2 upDir = normalize(ObjectBufferArray[TransformIndex].Transform[1].xy);

    float2 worldDelta = rightDir * localPx.x + upDir * localPx.y;

    float2 maskPx = worldDelta + 0.5f * MaskTexSize;
    float2 maskUV = maskPx / MaskTexSize;

    float mask = 0.f;
    if (maskUV.x >= 0.f && maskUV.x <= 1.f && maskUV.y >= 0.f && maskUV.y <= 1.f)
        mask = MaskTex.Sample(LinearSampler, maskUV).r;

    float maskA = 0.f;
    if (MaskSoftness <= 0.f)
        maskA = step(MaskThreshold, mask);
    else
        maskA = smoothstep(MaskThreshold - MaskSoftness, MaskThreshold + MaskSoftness, mask);

    Out.vColor = float4(maskA, maskA, maskA, 1.f);
    return Out;
}

// ------------------------------------------
PS_OUT PS_MAIN_LINEARFILL(PS_IN In)
{
    PS_OUT Out;
    
    float2 vTexcoord = { In.vTexcoord.x * (1.f - 2.f * Direction) + Direction, In.vTexcoord.y };
    vector vDiffuse = SpriteTexture.Sample(LinearSampler, vTexcoord);
    if (vDiffuse.a < 0.1f)
        discard;
    
    if (vTexcoord.x > FillAmount)
        discard;
    
    float3 vColorLinear = pow(vColor.rgb, 2.2f);
    Out.vColor = vDiffuse * float4(vColorLinear, vColor.a);
    
    return Out;
}

PS_OUT PS_MAIN_RADIALFILL(PS_IN In)
{
    PS_OUT Out;
    
    vector vDiffuse = SpriteTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a < 0.1f)
        discard;
    
    float2 vTexcoord = In.vTexcoord - 0.5f;
    float fAngle = atan2(vTexcoord.y, vTexcoord.x);
    fAngle = fAngle / (3.14159265 * 2.f) + 0.5f;        // 0 ~ 1로 정규화
    fAngle = (1.f - Direction) - frac(fAngle - 0.25f) * (Direction * -2.f + 1.f);
    
    float fGauge = step(fAngle, FillAmount);
    
    if (!fGauge)
        discard;
    
    float3 vColorLinear = pow(vColor.rgb, 2.2f);
    Out.vColor = vDiffuse * float4(vColorLinear, vColor.a);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }  

    pass SpriteAnimation
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_SPRITEANIMATION();
    }
// --------------------------------------------------------
    pass UVAnimation
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_UVANIMATION();
    }

    pass UVAnimation_Mask
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_UVANIMATION_MASK();
    }

    pass MaskDebugRaw
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_MASK_DEBUG_RAW();
    }

    pass MaskDebugApplied
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_MASK_DEBUG_APPLIED();
    }


// ----------------------------------------------------------
    pass LinearFill
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_LINEARFILL();
    }

    pass RadialFill
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_RADIALFILL();
    }
}

