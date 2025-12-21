#include "CS_Particle.hlsli"

[numthreads(256,1,1)]
void CS_MAIN(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint i = DispatchThreadID.x;
    if (i >= iSpawnCount)
        return;
    
    uint index = DeadConsume.Consume(); /* dead indices 에서 하나 가져옴*/
    
    Particle p = SpawnIn[i];            
    p.IsAlive = 1;
    p.fLifeTime = 0.f;
    
    Particles[index] = p;
    AliveOut.Append(index);
}