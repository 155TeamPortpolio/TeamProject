#pragma once
#include "Enemy.h"

NS_BEGIN(Client)
class CDefilerLaser :
    public CEnemy
{
public:
    enum class LASER_TYPE { NORMAL, STRONG, END };
private:
    CDefilerLaser();
    CDefilerLaser(const CDefilerLaser& rhg);
    virtual ~CDefilerLaser() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI()override;
    
private:
    HRESULT Initialize_Effects();
    void Apply_PendingActive();
    void SetUp_Effect();
    void SetUp_Collider();

public:
    void Set_ActiveLaser(_bool active, LASER_TYPE iType);

public:
    static CDefilerLaser* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    _bool m_IsPendingActive = false;
    _bool m_IsActive = false;

    _float m_fElapsedTime{};
    _float m_fDuration = 0.15f;

    LASER_TYPE m_eType = LASER_TYPE::NORMAL;
    _float3 m_vStartPoint{};
    _float3 m_vEndPoint{};
};
NS_END
