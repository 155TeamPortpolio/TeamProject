#pragma once
#include "Npc.h"
NS_BEGIN(Client)
class CBackgroundNpc :
    public CNpc
{
public:
    struct BackgroundDesc : GAMEOBJECT_DESC
    {
        _bool isPedestrian = false;
        vector<_float3> movePoint;
        _uint BackgroundCount;
        _bool isCircle = false;
    };

protected:
    CBackgroundNpc();
    CBackgroundNpc(const CBackgroundNpc& rhs);
    virtual ~CBackgroundNpc() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

private:
    void Build_Crowd(_uint Count, _bool Round = false);
    void Build_Pedestrian(_uint Count, vector<_float3> points);
public:
    static CBackgroundNpc* Create();
    CGameObject* Clone(INIT_DESC* pArg);
    virtual void Free() override;
};
NS_END