#pragma once
#include "GameObject.h"
#include "Enemy.h"

NS_BEGIN(Client)

class CThugPoacher_Arrow final : public CEnemy
{
public:
    typedef struct tagPoacherArrowDesc : public Engine::GAMEOBJECT_DESC
    {
        const _float4x4* pWeapon = { nullptr };
    }ARROW_DESC;

private:
    CThugPoacher_Arrow();
    CThugPoacher_Arrow(const CThugPoacher_Arrow& rhg);
    virtual ~CThugPoacher_Arrow() = default;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    virtual void Render_GUI() override;
    virtual void OnTriggerEnter(CGameObject* pOther) override;
    //virtual void OnTriggerStay(CGameObject* pOther) {};
    //virtual void OnTriggerExit(CGameObject* pOther) {};

public:
    void    ShootArrow();

private:
    void    FinishArrow();

private:
    const _float4x4*    m_pWeaponBone = { nullptr };
    _bool               m_isShoot = { false };
    _float3             m_vDir = {};
    _float              m_fSpeed = {20.f};

public:
    static CThugPoacher_Arrow* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END