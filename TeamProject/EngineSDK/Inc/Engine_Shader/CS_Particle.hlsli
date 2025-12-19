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
    float2 pad;
};

// 파티클 업데이트에 필요한 파라미터
cbuffer CBFrame : register(b0)
{
    float fDeltaTime;
    uint iMaxParticles;
    uint UseGravity;
    float fGravityScale;
}

// 살아있는 파티클 슬롯 인덱스 목록
StructuredBuffer<uint> AliveIn : register(t0);

// 다음 프레임 Alive 인덱스 리스트(append)
AppendStructuredBuffer<uint> AliveOut : register(u0);

// 죽은 슬롯 반환 리스트(append)
AppendStructuredBuffer<uint> DeadIndices : register(u1);

// 파티클 데이터 풀(갱신 대상)
RWStructuredBuffer<Particle> Particles : register(u2);