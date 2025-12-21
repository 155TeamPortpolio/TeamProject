struct Particle
{
    uint IsAlive;
    float3 vPosition;
    float3 vVelocity;
    float4 vColor;
    float fLifeTime;
    float fMaxLifeTime;
    float2 vStartSize;
    float2 vSize;
    float fNoiseFrequency;
    uint iFrameIndex;
    float pad;
};

struct InstanceData
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslate;
    float3 vVelocity;
    float4 vColor;
    float2 vLife;
    uint iFrameIndex;
    float2 pad;
};

// 파티클 업데이트에 필요한 파라미터
cbuffer CBFrame : register(b0)
{
    float fDeltaTime;
    uint iAliveCount;
    uint iMaxParticles;
    uint UseGravity;
    float fGravityScale;
    float3 framePad;
};

/* 파티클 생성에 필요한 파라미터 */
cbuffer CBSpawn : register(b1)
{
    uint iSpawnCount;
    uint3 spawnPad;
};

cbuffer CBDeadListInit : register(b2)
{
    uint iMaxParticleCount;
    uint3 initPad;
};

cbuffer CBPacked : register(b3)
{
    uint iInstanceCount;
    uint3 packedPad;
}

/* 이번 프레임에 살아있는 파티클들의 인덱스가 담겨있음 -> 파티클 업데이트에서 사용 */
StructuredBuffer<uint> AliveIn : register(t0);

/* 이번 프레임에 생성 할 파티클의 데이터들, cpu에서 계산해서 올려줌 */
StructuredBuffer<Particle> SpawnIn : register(t1);

/* 다음 프레임에 사용 할 살아있는 파티클들의 인덱스, AliveIn을 통해 들어온 파티클들이 갱신되어 만들어진 값들 */
AppendStructuredBuffer<uint> AliveOut : register(u0);

/* 파티클 업데이트 -> append    파티클 스폰 -> consume*/
AppendStructuredBuffer<uint> DeadAppend : register(u1);
ConsumeStructuredBuffer<uint> DeadConsume : register(u1);

/* 전체 파티클 데이터 */
RWStructuredBuffer<Particle> Particles : register(u2);

/* 드로우에 사용할 인스턴스 데이터 */
RWStructuredBuffer<InstanceData> Instances : register(u3);