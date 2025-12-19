#include "CS_Particle.hlsli"

[numthreads(256,1,1)]
void CS_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    uint i = DTid.x;
    if(i>=iMaxParticles)
        return;
    
    Particle p = Particles[i];
    
    if(p.IsAlive == 0)
        return;
    
    /* Life Time */
    p.fLifeTime += fDeltaTime;
    if(p.fLifeTime >= p.fMaxLifeTime)
    {
        p.IsAlive = 0;
        Particles[i] = p;
        DeadIndices.Append(i);
        return;
    }
    
    /* Gravity */
    if (UseGravity == 1)
    {
        p.vVelocity.y -= fGravityScale * 10.f * fDeltaTime;
    }
    
    p.vPosition += p.vVelocity * fDeltaTime;

    Particles[i] = p;
}