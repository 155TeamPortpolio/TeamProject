#include "Shader_Define.hlsl"

float3 color = float3(0.75f, 0.75f, 0.75f);
float alpha = 1.f;

float3 fillDark = float3(0.35f, 0.35f, 0.38f);
float3 fillLight = float3(0.90f, 0.90f, 0.92f);

float3 outlineColor = float3(0.03f, 0.03f, 0.03f);
float rimStart = 0.55f;
float rimEnd = 0.92f;
float rimStrength = 0.25f;

float edgeHardness = 1.5f;
float shadePow = 1.1f;

float topBlink = 0.f;
float3 topRed = float3(0.55f, 0.08f, 0.10f);
float3 topWhite = float3(1.f, 1.f, 1.f);
float topNStart = 0.85f;
float topNEnd = 0.98f;
float topNPow = 6.0f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 worldPos : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    float4x4 W = ObjectBufferArray[TransformIndex].Transform;
    float3 worldPos = mul(float4(In.vPosition, 1.f), W).xyz;

    float4 viewPos = mul(float4(worldPos, 1.f), matView);
    float4 projPos = mul(viewPos, matProjection);

    Out.vPosition = projPos;
    Out.worldPos = worldPos;

    return Out;
}

float4 PS_MAIN(VS_OUT In) : SV_TARGET0
{
    clip(alpha - 0.001f);
    
    float3 V = normalize(vCamPosition.xyz - In.worldPos);

    float3 dpdx = ddx(In.worldPos);
    float3 dpdy = ddy(In.worldPos);
    float3 Ng = normalize(cross(dpdx, dpdy));

    float3 N = Ng;
    if (dot(N, V) < 0.f)
        N = -N;

    float3 L = normalize(float3(-0.35f, 0.90f, -0.25f));
    float nl = saturate(dot(N, L));
    float shade = pow(saturate(0.35f + 0.65f * nl), shadePow);

    float3 baseRgb = lerp(fillDark, fillLight, shade) * color;

    float fres = 1.f - saturate(dot(N, V));
    float rim = smoothstep(rimStart, rimEnd, fres) * rimStrength;

    float3 dNx = ddx(N);
    float3 dNy = ddy(N);
    float hard = saturate((length(dNx) + length(dNy)) * edgeHardness);

    float edge = saturate(max(rim, hard));

    float3 worldUp = float3(0.f, 1.f, 0.f);
    float upDot = abs(dot(Ng, worldUp));
    float topMask = smoothstep(topNStart, topNEnd, upDot);
    topMask = pow(topMask, topNPow);

    float active = step(0.f, topBlink);
    float blinkT = saturate(topBlink);
    float3 blinkRgb = lerp(topRed, topWhite, blinkT);

    float tTop = topMask * active;
    float3 midRgb = lerp(baseRgb, blinkRgb, tTop);

    float3 rgb = lerp(midRgb, outlineColor, edge);

    return float4(rgb, alpha);
}

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ReadOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
