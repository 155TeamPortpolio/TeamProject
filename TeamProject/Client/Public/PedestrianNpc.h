#pragma once
#include "Npc.h"
#include "NPC_Preset.h"

NS_BEGIN(Client)
class CPedestrianNpc :
    public CNpc
{
protected:
    CPedestrianNpc();
    CPedestrianNpc(const CPedestrianNpc& rhs);
    virtual ~CPedestrianNpc() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

public:
    void Set_MovePoint(vector<_float3> points);
private:
    _vector3 RotateTowardsXZ(_vector3 curDir, _vector3 targetDir, float maxRadDelta);
    void Calc_Destination(_float dt);
    void SnapToStart();
    _vector3 Rotate90ByCw(const _vector3& v, bool cw);

private:
    NpcColorPreset colorPreset;
    NpcModelPreset modelPreset;
protected:
    _uint m_CurPointIdx = {};
    _float m_CurYaw = {};
    _float m_MoveSpeed = {};
    _vector3 m_Vel = { 3.f, 0.f, 3.f };  
    _float    m_PathOffset = 0.f;        

    vector<_float3> movePoints;

public:
    static CPedestrianNpc* Create();
    CGameObject* Clone(INIT_DESC* pArg);
    virtual void Free() override;
};
NS_END