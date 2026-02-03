#pragma once
#include "Npc.h"

NS_BEGIN(Client)

class CPairBoo :
    public CNpc
{
private:
    CPairBoo();
    CPairBoo(const CPairBoo& rhs);
    virtual ~CPairBoo() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

public:
    static CPairBoo* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END