#pragma once
#include "Enemy.h"

NS_BEGIN(Client)
class CMiasmaProjectile :
    public CEnemy
{
public:
    struct MiasmaProjectileDesc : GAMEOBJECT_DESC {
        _float3 vTargetPos = {};
        MiasmaProjectileDesc() {};
        MiasmaProjectileDesc(_float3 pos) :vTargetPos(pos) {};
    };

private:
    CMiasmaProjectile();
    CMiasmaProjectile(const CMiasmaProjectile& rhg);
    virtual ~CMiasmaProjectile() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI()override;
public:
    virtual void OnPooledAcquire(INIT_DESC* pArg = nullptr) override;		// 풀에서 꺼낼 때
    virtual void OnPooledRelease()override;
public:
    virtual void    OnTriggerEnter(CGameObject* pOther) override;

private:
    _float m_ElapsedTime = {};
    _float m_fMoveSpeed = { 45 };
    _vector3 m_vTargetVelocity = {};
    _vector3 m_vVelocity = {};
public:
    static CMiasmaProjectile* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END