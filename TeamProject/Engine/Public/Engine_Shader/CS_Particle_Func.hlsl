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

Particle NOISE(Particle p)
{
    Particle Out = p;
    
    return Out;
}

#endif //__PARTICLE_CS_FUNC_HLSL__
