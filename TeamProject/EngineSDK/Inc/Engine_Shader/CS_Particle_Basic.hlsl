#include "CS_Particle.hlsli"
#include "CS_Particle_Func.hlsl"

[numthreads(256,1,1)] /*계산에 사용할 스레드 그룹 x,y,z의 좌표로 각 스레드를 식별 함, 그룹에 포함된 스레드 갯수는 x*y*z 만큼 */
void CS_MAIN(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint i = DispatchThreadID.x;
    if (i >= iAliveCount)
        return;
    
    uint index = AliveIn[i];
    
    Particle p = Particles[index];
    
    if(p.IsAlive == 0)
        return;
    
    /* Life Time */
    p.fLifeTime += fDeltaTime;
    if(p.fLifeTime >= p.fMaxLifeTime)
    {
        p.IsAlive = 0;
        Particles[index] = p;
        DeadAppend.Append(index); /* 죽은 파티클 append에 추가 */
        return;
    }
    
    /* Module */
    p = LIFE_TIME_VELOCITY(p);
    p = LIFE_TIME_SIZE(p);
    p = LIFE_TIME_COLOR(p);
    
    /* Gravity */
    if (UseGravity == 1)
    {
        p.vVelocity.y -= fGravityScale * 10.f * fDeltaTime;
    }
    
    p.vPosition += p.vVelocity * fDeltaTime;

    Particles[index] = p;
    AliveOut.Append(index);
}