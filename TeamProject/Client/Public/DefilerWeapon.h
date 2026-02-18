#pragma once
#include "Enemy.h"
#include "Defiler_Control.h"

NS_BEGIN(Client)
class CDefilerWeapon :
    public CEnemy
{
public:
    struct DefilerWeaponDesc : GAMEOBJECT_DESC { _bool isFinal = { false }; _float3 vTargetPos = {}; };

private:
    CDefilerWeapon();
    CDefilerWeapon(const CDefilerWeapon& rhg);
    virtual ~CDefilerWeapon() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI()override;
public:
    virtual void    OnTriggerEnter(CGameObject* pOther) override;

public:
    void Update_Dissolve(_float dt);

private:
    _bool   Try_Hit(CGameObject* pTarget);
    void Reset_Value(DefilerWeaponDesc* pArg);
    void SummonAxe();

private:
    HRESULT Initialize_Effects();

    void Play_Effect(const string& effectTag, _fvector offsetPosition, _fvector offsetQuaternion, _bool syncTransform = true);
    void Stop_Effect(const string& effectTag);

private:
    _bool m_isEnd = {};
    _bool m_isSliding = false;
    _bool m_isFinalThrow = false;

    _float m_ElapsedTime = {};
    _float m_fMoveSpeed = { 120 };
    _float   m_groundY = 0.f;

    _vector3 m_vTargetVelocity = {};
    _vector3 m_vVelocity = {};
    _vector3 m_vTargetPos = {};
    _vector3 m_slideVelXZ = { 0.f, 0.f, 0.f };

    DefilerDissolve m_Dissolve;
public:
    static CDefilerWeapon* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END
