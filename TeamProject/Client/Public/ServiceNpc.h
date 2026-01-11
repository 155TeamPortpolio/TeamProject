#pragma once
#include "Npc.h"

NS_BEGIN(Client)

class CServiceNpc abstract :
    public CNpc
{
protected:
    CServiceNpc();
    CServiceNpc(const CServiceNpc& rhs);
    virtual ~CServiceNpc() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

public:
    CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};

NS_END