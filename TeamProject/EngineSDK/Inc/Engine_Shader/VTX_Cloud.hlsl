#include "Shader_Define.hlsl"

float g_Time;
float3 SunDir;

float g_SunIntensity = 5.f; 
float3 g_SkyTopColor = float3(0.002, 0.005, 0.015);
float3 g_SkyHorizonColor = float3(0.01, 0.02, 0.05);
float g_SkyAtmosphereBlend = 0.5;

float3 g_CloudBright = float3(0.12, 0.14, 0.22);
float3 g_CloudDark = float3(0.02, 0.02, 0.05);
float g_CloudCoverage_Param = 0.40;

static const float EARTH_RADIUS = 6371000.0;
static const float ATMO_RADIUS = EARTH_RADIUS + 100000.0;
static const float3 BETA_R = float3(5.8e-6, 13.5e-6, 33.1e-6);
static const float BETA_M = 21e-6;
static const float MIE_G = 0.76;
static const float H_R = 8000.0;
static const float H_M = 1200.0;
static const float SUN_INTENSITY = 22.0;

static const float3 HorizonColor = float3(0.65, 0.80, 1.00);
static const float3 SunsetMid = float3(1.0, 0.55, 0.20);

static const float CloudBottom = 800.0;
static const float CloudTop = 1800.0;
static const float CloudCoverage = 0.65; 

static const int CLOUD_STEPS = 32;
static const int LIGHT_STEPS = 4;

float remap(float v, float lo1, float hi1, float lo2, float hi2)
{
    return lo2 + (v - lo1) * (hi2 - lo2) / (hi1 - lo1);
}

float3 hashGrad(float3 p)
{
    p = frac(p * float3(0.1031, 0.1030, 0.0973));
    p += dot(p, p.yxz + 33.33);
    float3 h = frac(float3(
        (p.x + p.y) * p.z,
        (p.x + p.z) * p.y,
        (p.y + p.z) * p.x
    ));
    return h * 2.0 - 1.0;
}

uint pcgHash(uint input)
{
    uint state = input * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float hash3(float3 p)
{
    uint3 u = uint3(int3(floor(p)));
    uint h = u.x * 374761393u + u.y * 668265263u + u.z * 967981169u;
    h = pcgHash(h);
    return float(h) * (1.0 / 4294967295.0);
}

float valueNoise(float3 p)
{
    float3 i = floor(p);
    float3 f = frac(p);
    f = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);

    float a = hash3(i);
    float b = hash3(i + float3(1, 0, 0));
    float c = hash3(i + float3(0, 1, 0));
    float d = hash3(i + float3(1, 1, 0));
    float e = hash3(i + float3(0, 0, 1));
    float f2 = hash3(i + float3(1, 0, 1));
    float g = hash3(i + float3(0, 1, 1));
    float h = hash3(i + float3(1, 1, 1));

    float x1 = lerp(a, b, f.x);
    float x2 = lerp(c, d, f.x);
    float x3 = lerp(e, f2, f.x);
    float x4 = lerp(g, h, f.x);

    return lerp(lerp(x1, x2, f.y),
                lerp(x3, x4, f.y), f.z);
}

static const float3x3 FBM_ROT = float3x3(
     0.00, 0.80, 0.60,
    -0.80, 0.36, -0.48,
    -0.60, -0.48, 0.64
);

float fbmShape(float3 p)
{
    float v = 0.0, a = 0.5;
    [unroll]
    for (int i = 0; i < 3; i++)
    {
        v += a * valueNoise(p);
        p = mul(FBM_ROT, p) * 2.0 + 0.5;
        a *= 0.5;
    }
    return saturate(v / 0.875);
}

float hg(float cosT, float g)
{
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * 3.14159265 * pow(1.0 + g2 - 2.0 * g * cosT, 1.5));
}

float cloudPhase(float cosT)
{
    return hg(cosT, 0.6) * 1.5 + hg(cosT, -0.3) * 0.25 + 0.1;
}

float2 raySphereIntersect(float3 ro, float3 rd, float3 center, float radius)
{
    float3 oc = ro - center;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - radius * radius;
    float disc = b * b - c;
    if (disc < 0.0)
        return float2(-1, -1);
    float sq = sqrt(disc);
    return float2(-b - sq, -b + sq);
}

float3 atmosphericScatter(float3 dir, float3 sunDir)
{
    float3 camPos = float3(0, EARTH_RADIUS + 1.0, 0);
    float3 earthCenter = float3(0, 0, 0);

    float2 atmoHit = raySphereIntersect(camPos, dir, earthCenter, ATMO_RADIUS);
    if (atmoHit.y < 0.0)
        return float3(0, 0, 0);

    float tMax = atmoHit.y;
    float2 groundHit = raySphereIntersect(camPos, dir, earthCenter, EARTH_RADIUS);
    if (groundHit.x > 0.0)
        tMax = groundHit.x;

    const int STEPS = 10;
    float segLen = tMax / (float) STEPS;

    float3 sumR = 0, sumM = 0;
    float optR = 0, optM = 0;

    for (int i = 0; i < STEPS; i++)
    {
        float3 sampleP = camPos + dir * (segLen * ((float) i + 0.5));
        float h = max(length(sampleP) - EARTH_RADIUS, 0.0);

        float hr = exp(-h / H_R) * segLen;
        float hm = exp(-h / H_M) * segLen;
        optR += hr;
        optM += hm;

        float2 sunHit = raySphereIntersect(sampleP, sunDir, earthCenter, ATMO_RADIUS);
        float sunSeg = sunHit.y / 4.0;
        float optLR = 0, optLM = 0;
        bool shadow = false;

        [unroll]
        for (int j = 0; j < 4; j++)
        {
            float3 sp = sampleP + sunDir * (sunSeg * ((float) j + 0.5));
            float sh = length(sp) - EARTH_RADIUS;
            if (sh < 0.0)
            {
                shadow = true;
                break;
            }
            optLR += exp(-sh / H_R) * sunSeg;
            optLM += exp(-sh / H_M) * sunSeg;
        }
        if (shadow)
            continue;

        float3 tau = BETA_R * (optR + optLR) + BETA_M * 1.1 * (optM + optLM);
        float3 atten = exp(-tau);
        sumR += hr * atten;
        sumM += hm * atten;
    }

    float cosT = dot(dir, sunDir);
    float phaseR = 3.0 / (16.0 * 3.14159265) * (1.0 + cosT * cosT);
    float phaseM = hg(cosT, MIE_G);

    return g_SunIntensity * (sumR * BETA_R * phaseR + sumM * BETA_M * phaseM);
}

float sampleDensity(float3 pos)
{
    float h = saturate((pos.y - CloudBottom) / (CloudTop - CloudBottom));

    float heightMask = smoothstep(0.0, 0.08, h) * smoothstep(1.0, 0.3, h);
    float hCoverage = lerp(g_CloudCoverage_Param, g_CloudCoverage_Param * 0.2, pow(h, 0.5));
   
    float3 sp = pos;
    float lo = fbmShape(sp * 0.0006);
    float mid = fbmShape(sp * 0.0018);

    float baseShape = lo * 0.7 + mid * 0.3;

    float density = saturate(remap(baseShape, 1.0 - hCoverage, 1.0, 0.0, 1.0));

    density = smoothstep(0.0, 0.15, density);

    density *= heightMask;
    return density;
}

float sampleDensityCheap(float3 pos)
{
    float h = saturate((pos.y - CloudBottom) / (CloudTop - CloudBottom));
    float heightMask = smoothstep(0.0, 0.08, h) * smoothstep(1.0, 0.3, h);
    float hCoverage = lerp(g_CloudCoverage_Param, g_CloudCoverage_Param * 0.2, pow(h, 0.5));

    float3 sp = pos + float3(g_Time * 6.0, 0.0, g_Time * 2.0);

    float lo = fbmShape(sp * 0.0006);
    float density = saturate(remap(lo, 1.0 - hCoverage, 1.0, 0.0, 1.0));
    density = smoothstep(0.0, 0.15, density);
    density *= heightMask;
    return density;
}

float lightMarch(float3 pos)
{
    float3 lightDir = normalize(-SunDir);
    float stepLen = (CloudTop - CloudBottom) * 0.15;
    float totalDensity = 0.0;

    [unroll]
    for (int i = 0; i < LIGHT_STEPS; i++)
    {
        pos += lightDir * stepLen;
        if (pos.y < CloudBottom || pos.y > CloudTop)
            break;
        totalDensity += sampleDensityCheap(pos);
    }

    float beer = exp(-totalDensity * 5.0);
    float powder = 1.0 - exp(-totalDensity * 10.0);
    return lerp(beer, beer * powder, 0.4);
}

float InterleavedGradientNoise(float2 pixelPos)
{
    float3 magic = float3(0.06711056, 0.00583715, 52.9829189);
    return frac(magic.z * frac(dot(pixelPos, magic.xy)));
}

float4 raymarchClouds(float3 dir, float3 sunDir, float3 skyCol, float2 pixelPos)
{
    float3 camPos = vCamPosition.xyz;

    float tBot = (CloudBottom - camPos.y) / dir.y;
    float tTop = (CloudTop - camPos.y) / dir.y;

    float tStart = min(tBot, tTop);
    float tEnd = max(tBot, tTop);
    tStart = max(tStart, 0.0);
    tEnd = min(tEnd, 15000.0);

    if (tStart >= tEnd || tEnd <= 0.0)
        return float4(0, 0, 0, 0);

    float stepSize = (tEnd - tStart) / (float) CLOUD_STEPS;

    float jitter = InterleavedGradientNoise(pixelPos);
    tStart += stepSize * jitter;

    float transmittance = 1.0;
    float3 light = float3(0, 0, 0);

    float sunHeight = saturate(sunDir.y);
    float3 sunCol = lerp(float3(1.5, 0.5, 0.15), g_CloudBright, saturate(sunHeight * 3.0));
    sunCol *= g_SunIntensity * 0.15;
    float3 ambientCol = lerp(g_CloudDark, skyCol * 0.5, sunHeight);

    float cosAngle = dot(dir, sunDir);
    float phase = cloudPhase(cosAngle);

    int zeroCount = 0;

    for (int i = 0; i < CLOUD_STEPS; i++)
    {
        float t = tStart + stepSize * ((float) i + 0.5);
        float3 p = camPos + dir * t;

        float d = sampleDensity(p);

        if (d < 0.001)
        {
            zeroCount++;
            if (zeroCount > 3)
            {
                i++;
                zeroCount = 0;
            }
            continue;
        }
        zeroCount = 0;

        float li = lightMarch(p);

        float3 s1 = sunCol * li * phase;
        float3 s2 = sunCol * (li * 0.4 + 0.15) * 0.3;
        float3 s3 = ambientCol * 0.35;
        float3 scattered = s1 + s2 + s3;

        float alpha = 1.0 - exp(-d * stepSize * 1.5);

        float aerial = 1.0 - exp(-t * 0.00008);
        scattered = lerp(scattered, skyCol * 0.7, aerial * 0.6);

        light += scattered * alpha * transmittance;
        transmittance *= (1.0 - alpha);

        if (transmittance < 0.01)
            break;
    }

    return float4(light, 1.0 - transmittance);
}

float stars(float3 dir)
{
    float3 p = dir * 300.0;
    float n = hash3(floor(p));
    float brightness = step(0.997, n);
    float twinkle = sin(g_Time * 3.0 + n * 100.0) * 0.5 + 0.5;
    return brightness * twinkle * 2.0;
}

float3 sunDisk(float3 dir, float3 sunDir)
{
    float cosAngle = dot(dir, sunDir);
    float angularSize = 0.9997;
    float edge = smoothstep(angularSize - 0.0002, angularSize, cosAngle);
    float center = saturate((cosAngle - angularSize) / (1.0 - angularSize));
    float limb = 1.0 - pow(1.0 - center, 0.4) * 0.3;
    return float3(1.0, 0.95, 0.85) * 80.0 * limb * edge;
}

float3 ACESFilm(float3 x)
{
    return saturate((x * (2.51 * x + 0.03)) / (x * (2.43 * x + 0.59) + 0.14));
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vWorldDir : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    Out.vPosition = float4(In.vPosition, 1.f);
    Out.vTexcoord = In.vTexcoord;

    float4 clip = float4(In.vPosition.xy, 1.0, 1.0);
    float4 vDir = mul(clip, matProjectionInverse);
    vDir.xyz /= vDir.w;
    vDir.w = 0.f;
    Out.vWorldDir = mul(vDir, matViewInverse).xyz;
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vWorldDir : TEXCOORD1;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;

    float3 dir = normalize(In.vWorldDir);
    float3 sunDir = normalize(-SunDir);

    float3 scatterColor = atmosphericScatter(dir, sunDir);
    float heightFactor = saturate(dir.y * 0.5 + 0.5);
    float3 manualSky = lerp(g_SkyHorizonColor, g_SkyTopColor, pow(heightFactor, 1.2));
    float3 skyColor = lerp(scatterColor, manualSky, g_SkyAtmosphereBlend);
    skyColor += sunDisk(dir, sunDir);

    float nightFactor = saturate(-sunDir.y * 2.0);
    skyColor += stars(dir) * nightFactor * saturate(dir.y) * float3(0.8, 0.85, 1.0);

    float4 cloud = raymarchClouds(dir, sunDir, skyColor, In.vPosition.xy);
    float3 finalColor = lerp(skyColor, cloud.rgb, cloud.a);

    //float sunHeight = saturate(sunDir.y);
    //float3 hazeColor = lerp(
    //    float3(0.15, 0.1, 0.12),
    //    lerp(SunsetMid, HorizonColor, sunHeight),
    //    saturate(sunDir.y + 0.1)
    //);
    //float hazeFactor = 1.0 - smoothstep(0.0, 0.15, abs(dir.y));
    //finalColor = lerp(finalColor, hazeColor, hazeFactor * 0.65);

    //float groundMask = saturate(-dir.y * 8.0);
    //float3 groundCol = lerp(hazeColor, float3(0.15, 0.15, 0.13), groundMask);
    //finalColor = lerp(finalColor, groundCol, groundMask * 0.9);

    finalColor = ACESFilm(finalColor);
    finalColor = pow(max(finalColor, 0.0), 1.0 / 2.2);

    Out.vDiffuse = float4(finalColor, 1.f);
    return Out;
}

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_ReadOnly, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
