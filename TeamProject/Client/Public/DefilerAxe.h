#pragma once
#include "Enemy.h"
NS_BEGIN(Client)
class CDefilerAxe :
    public CEnemy
{
public:
    struct DefilerAxeDesc:GAMEOBJECT_DESC{
        _vector3 vLook = {};
    };
private:
    CDefilerAxe();
    CDefilerAxe(const CDefilerAxe& rhg);
    virtual ~CDefilerAxe() DEFAULT;

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
    virtual void TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName = CHARACTER::END);

public:
    void DisAppear();
private:
    void SummonWall();
    void SpawnEffect();

private:
    _bool m_bDangle = {};
    _float m_fElapsedTime = {};
    _vector3 m_vSlide = {};
    _quaternion m_BaseRot;

    _float m_ShakeAmpDeg = 15.f;        // 흔들 각도(도)
    _float m_ShakeHz = 4.f;             // 초당 흔들 횟수
    _float m_HitShakeAmpDeg = 0.f;     // hit로 누적되는 흔들림 각도
    _float m_HitShakeMaxDeg = 7.f;     // 최대 각도(캡)
    _float m_HitShakeDecay = 10.f;     // 각도 감쇠 속도

public:
    static CDefilerAxe* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END