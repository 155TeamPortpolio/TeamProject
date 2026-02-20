#pragma once
#include "GameObject.h"
NS_BEGIN(Client)
class CMiasmaSpawnBall :
    public CGameObject
{
public:
    struct SpawnParbolar : GAMEOBJECT_DESC {
        _float3 startPos = {};
        _float3 targetPos = {};
        _float  travelTime = 0.25f;    
        _float  arcHeight = 2.5f;      
        _float  sideArc = 0.0f;        
        _bool   destroyOnArrive = true;
   };
private:
    CMiasmaSpawnBall();
    CMiasmaSpawnBall(const CMiasmaSpawnBall& rhg);
    virtual ~CMiasmaSpawnBall() DEFAULT;

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
    void Initialize_Effects();
    void SpawnJaeger();
private:
    _float3 m_startPos = {};
    _float3 m_targetPos = {};
    _float  m_travelTime = 0.45f;
    _float  m_arcHeight = 3.0f;
    _float  m_sideArc = 0.0f;

    _float  m_elapsed = 0.f;
    _bool   m_destroyOnArrive = true;
public:
    static CMiasmaSpawnBall* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END