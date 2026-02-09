#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

class CMeleeJaeger_Shield final : public CEnemy
{
public:
    typedef struct tagMeleeJaegerShieldDesc : public Engine::GAMEOBJECT_DESC
    {
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

private:
    void    ComputePosition(_bool isFirst = false);

private:
    const _float4x4*    m_pHandBone = { nullptr };
    _float3             m_vOffset = {};
    _bool               m_isFirstCompute = { false };

public:
    static CMeleeJaeger_Shield* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END