#include "CS_Particle.hlsli"

[numthreads(256,1,1)] /*계산에 사용할 스레드 그룹 x,y,z의 좌표로 각 스레드를 식별 함, 그룹에 포함된 스레드 갯수는 x*y*z 만큼 */
void CS_MAIN(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint i = DispatchThreadID.x;
    if (i >= iAliveCount)
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
        DeadAppend.Append(i); /* 죽은 파티클 append에 추가 */
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