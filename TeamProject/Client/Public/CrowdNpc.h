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
    virtual void    Render_GUI() override;
private:
    _float4 m_fHairColor = { 1.00f, 0.86f, 0.70f,1.f };

public:
    static CCrowdNpc* Create();
    CGameObject* Clone(INIT_DESC* pArg);
    virtual void Free() override;
};

NS_END