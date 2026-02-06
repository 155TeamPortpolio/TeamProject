#pragma once
#include "GameObject.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CCyclops_Spit final : public CEnemy
{
public:
    typedef struct tagCyclopsSpitDesc : public Engine::GAMEOBJECT_DESC
    {
        const _float4x4* pHeadBone = { nullptr };
    }SPIT_DESC;

    enum class SPIT { STRAIGHT, ARC_CENTER, ARC_LEFT, ARC_RIGHT };

private:
    CCyclops_Spit();
    CCyclops_Spit(const CCyclops_Spit& rhg);
    virtual ~CCyclops_Spit() = default;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    virtual void Render_GUI() override;
    virtual void OnTriggerEnter(CGameObject* pOther) override;

public:
    void    ShootSpit(SPIT eSpitType);

private:
    void    FinishSpit();
    _float  ComputeGravityScale(_float lifeTime, _float rampTime);
    _vector MakeSpreadDir_Yaw(_fvector vLookDir, _float fYawDeg);

private:
    const _float4x4*    m_pHeadBone = { nullptr };
    _bool               m_isStraight = { false };
    _bool               m_isArc = { false };
    _float3             m_vDir = {};
    _float              m_fShootSpeed = { 20.f };

    _float              m_fLifeTime = {};

    /* Arc */
    _float3             m_vVelocity{};   // 현재 속도(벡터)
    _float              m_fArcSpeed = 5.f;   // 전방 속도
    _float              m_fUpKick = 3.5f;   // “살짝 떠오름” 정도 (초기 Y 속도)
    _float              m_fGravity = 9.8f;   // 중력
    _float              m_fRampTime = 0.12f;  // 중력 램프업 시간(0.08~0.2 추천)

    _bool               m_isCollisionCooltime = { false };
    _float2             m_vCollisionCooltime = { 0.1f, 0.f };


public:
    static CCyclops_Spit* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END