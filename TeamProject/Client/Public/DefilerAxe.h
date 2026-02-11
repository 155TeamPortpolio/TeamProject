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
    virtual void OnPooledAcquire(INIT_DESC* pArg = nullptr) override;		// Ç®¿¡¼­ ²¨³¾ ¶§
    virtual void OnPooledRelease()override;
    virtual void TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName = CHARACTER::END);

private:
    void SummonWall();

private:
    _bool m_bDangle = {};
    _float m_fElapsedTime = {};
    _vector3 m_vSlide = {};
    _quaternion m_BaseRot;

    _float m_ShakeAmpDeg = 15.f; // Èçµé °¢µµ(µµ)
    _float m_ShakeHz = 4.f;    // ÃÊ´ç Èçµé È½¼ö

public:
    static CDefilerAxe* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END