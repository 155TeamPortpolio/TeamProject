#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CAttackRange :
    public CGameObject
{
private:
    CAttackRange();
    CAttackRange(const CAttackRange& rhg);
    virtual ~CAttackRange() DEFAULT;

public:
    HRESULT Initialize_Prototype()     override;
    HRESULT Initialize(INIT_DESC* arg) override;
    void    Awake()                    override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt)          override;
    void    Late_Update(_float dt)     override;

public:
    void Play_AttackRange(_float3 position, _float range);

public:
    static CAttackRange* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    void Free() override;

private:

};

NS_END