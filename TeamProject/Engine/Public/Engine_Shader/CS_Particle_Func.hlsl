#ifndef __PARTICLE_CS_FUNC_HLSL__
#define __PARTICLE_CS_FUNC_HLSL__

#include "CS_Particle.hlsli"

Particle LIFE_TIME_VELOCITY(Particle p)
{
    Particle Out = p;
    
    float3 vVelocity = Out.vVelocity;
    float fDecay = exp(-fDampScale * fDeltaTime);
    
    Out.vVelocity = vVelocity * fDecay;
    
    return Out;
}

Particle LIFE_TIME_SIZE(Particle p)
{
    Particle Out = p;
    
    float t = Out.fLifeTime / Out.fMaxLifeTime;
    
    float2 vCurrScale = lerp(vStartScale, vEndScale, t);
    Out.vSize = Out.vStartSize * vCurrScale;
    
    return Out;
}

Particle LIFE_TIME_COLOR(Particle p)
{
    Particle Out = p;
    
    float t = Out.fLifeTime / Out.fMaxLifeTime;
    
    float4 vCurrColor = lerp(vStartColor, vEndColor, t);
    Out.vColor = vCurrColor;
    
    return Out;
}

Particle TEXTURE_SHEET_ANIMATION(Particle p)
{
    Particle Out = p;
    
    if (IsAnimated)
    {
        float t = Out.fLifeTime / Out.fMaxLifeTime;
        uint iFrameIndex = floor(iMaxFrameIndex * t);
        
        Out.iFrameIndex = iFrameIndex;
    }
    
    return Out;
}

uint Hash(uint x)
{
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;

    return x;
}

int Floor(float x)
{
    return (int) floor(x);
}

float Fade(float t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float NormalizeHash(int x, int y, int z)
{
    uint seed = 0u;

    seed ^= Hash((uint) x * 73856093u);
    seed ^= Hash((uint) y * 19349663u);
    seed ^= Hash((uint) z * 83492791u);

    seed &= 0x7fffffffu;
    
    float p = (float) seed / 2147483647.0;
    return p * 2.0 - 1.0;
}

float MakeNoise(float3 sample)
{
    int x0, y0, z0;
    int x1, y1, z1;
    x0 = Floor(sample.x);
    y0 = Floor(sample.y);
    z0 = Floor(sample.z);

    x1 = x0 + 1;
    y1 = y0 + 1;
    z1 = z0 + 1;

    float fx = sample.x - (float) (x0);
    float fy = sample.y - (float) (y0);
    float fz = sample.z - (float) (z0);

    float u = Fade(fx);
    float v = Fade(fy);
    float w = Fade(fz);

    float c000 = NormalizeHash(x0, y0, z0);
    float c100 = NormalizeHash(x1, y0, z0);
    float c010 = NormalizeHash(x0, y1, z0);
    float c110 = NormalizeHash(x1, y1, z0);
	
    float c001 = NormalizeHash(x0, y0, z1);
    float c101 = NormalizeHash(x1, y0, z1);
    float c011 = NormalizeHash(x0, y1, z1);
    float c111 = NormalizeHash(x1, y1, z1);
	
    float x00 = lerp(c000, c100, u);
    float x10 = lerp(c010, c110, u);
    float x01 = lerp(c001, c101, u);
    float x11 = lerp(c011, c111, u);
	
    float y0v = lerp(x00, x10, v);
    float y1v = lerp(x01, x11, v);

    return lerp(y0v, y1v, w); // [-1,1]
}

Particle NOISE(Particle p)
{
    Particle Out = p;
    
    float3 vSample = Out.vPosition * vFrequency.xyz * Out.fNoiseFrequency + fElapsedTime * vScrollSpeed.xyz;
    float3 vNoise;
    
    vNoise.x = MakeNoise(vSample + float3(17.1f, 0.f, 0.f));
    vNoise.y = MakeNoise(vSample + float3(0.f, 31.7f, 0.f));
    vNoise.z = MakeNoise(vSample + float3(0.f, 0.f, 47.3f));
    
    vNoise *= vStrength.xyz;
    Out.vVelocity = Out.vVelocity + vNoise * fDeltaTime;
    
    return Out;
}

#endif //__PARTICLE_CS_FUNC_HLSL__
