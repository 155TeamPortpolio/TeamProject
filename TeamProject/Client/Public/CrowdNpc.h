#pragma once
#include "Npc.h"
NS_BEGIN(Client)
class CCrowdNpc :
    public CNpc
{
protected:
    CCrowdNpc();
    CCrowdNpc(const CCrowdNpc& rhs);
    virtual ~CCrowdNpc() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

private:
    void Calc_Destination(_float dt);

protected:
    _uint m_CurPointIdx = {};
    _float3 m_NowPos = {};
    _float3 m_DstPos = {};
    _float m_CurYaw = {};
public:
    static CCrowdNpc* Create();
    CGameObject* Clone(INIT_DESC* pArg);
    virtual void Free() override;
};
NS_END