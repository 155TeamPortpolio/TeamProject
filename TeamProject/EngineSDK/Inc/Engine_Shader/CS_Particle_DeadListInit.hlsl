#include "CS_Particle.hlsli"

[numthreads(256,1,1)]
void CS_MAIN(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    if (DispatchThreadID.x >= iMaxParticleCount)
        return;
    
    DeadAppend.Append(DispatchThreadID.x);
}