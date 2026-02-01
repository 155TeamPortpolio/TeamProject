#pragma once
#include "Enemy.h"

NS_BEGIN(Client)
class CSacrifice_Orb : public CEnemy
{
private:
    CSacrifice_Orb();
    CSacrifice_Orb(const CSacrifice_Orb& rhg);
    virtual ~CSacrifice_Orb() DEFAULT;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

    void Render_GUI()override;

public:
    void OnTriggerEnter(CGameObject* pOther) override;

public:
    static CSacrifice_Orb* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    void Free() override;


private:
    void ChaseTarget();
    HRESULT Create_Colliders();

    _float m_fSpeed{};
    _float3 vTargetPos{};
};
NS_END
