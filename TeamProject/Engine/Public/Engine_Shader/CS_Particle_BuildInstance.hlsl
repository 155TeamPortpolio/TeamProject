#include "CS_Particle.hlsli"

[numthreads(256,1,1)]
void CS_MAIN(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint i = DispatchThreadID.x;
    if (i >= iAliveCount)
        return;
    
    uint index = AliveIn[i];
    Particle p = Particles[index];
    
    if (p.IsAlive == 0)
        return;
    
    InstancePoint instance;
    instance.vRight = float4(1.f, 0.f, 0.f, 0.f) * p.vSize.x;
    instance.vUp = float4(0.f, 1.f, 0.f, 0.f) * p.vSize.y;
    instance.vLook = float4(0.f, 0.f, 1.f, 0.f);
    instance.vTranslate = float4(p.vPosition, 1.f);
    instance.vVelocity = p.vVelocity;
    instance.vColor = p.vColor;
    instance.vLife = float2(p.fLifeTime, p.fMaxLifeTime);
    instance.iFrameIndex = p.iFrameIndex;
    instance.pad = float2(0.f, 0.f);
    
    Instances[i] = instance;
}