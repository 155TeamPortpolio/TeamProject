#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

class CMeleeJaeger_Shield final : public CEnemy
{
public:
    typedef struct tagMeleeJaegerShieldDesc : public CEnemy::ENEMY_DESC
    {
        const _float4x4* pWeaponBone = { nullptr };
        const _float4x4* pHandBone = { nullptr };
    }JAEGERSHIELD_DESC;

private:
    CMeleeJaeger_Shield();
    CMeleeJaeger_Shield(const CMeleeJaeger_Shield& rhg);
    virtual ~CMeleeJaeger_Shield() = default;

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
    /* Getter */


    /* Setter */
    virtual void    TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName = CHARACTER::END) override;
    void            StartRoll(_float fDegree);
    void            EndRoll() { m_isEndRoll = true; }
    
private:
    void    ComputePosition();
    void    ComputeRoll(_float dt);

private:
    const _float4x4*    m_pWeaponBone = { nullptr };
    const _float4x4*    m_pHandBone = { nullptr };
    _float3             m_vOffset = {};
    _bool               m_isFirstCompute = { false };

    _bool               m_hasPrev = { false };
    _vector             m_prevQ = XMQuaternionIdentity();

    _bool               m_isRoll = { false };
    _float              m_fRollDegree = {};
    _float              m_fComputeDegree = {};

    _bool               m_isStartRoll = { false };
    _float2             m_vStartRollTime = { 0.3f,0.f };

    _bool               m_isEndRoll = { false };
    _float2             m_vEndRollTime = {0.5f,0.f};

public:
    static CMeleeJaeger_Shield* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END