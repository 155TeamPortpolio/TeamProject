#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CAttackSign :
    public CGameObject
{
private:
    CAttackSign();
    CAttackSign(const CAttackSign& rhs);
    virtual ~CAttackSign() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

public:
    void Active();

public:
    static CAttackSign* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    _float m_fWidth = 50.f; //screen size
    _float m_fHeight = 40.f;
    _float3 m_vColor{1.f,0.25f,0.f};
    _float m_fAlpha{ 1.f };

    _float m_fScreenWidth = g_iWinSizeX;
    _float m_fScreenHeight = g_iWinSizeY;

    _bool m_IsActive = false;
    _float m_fElapsedTime{};
    _float m_fDuration = 0.35f;

};

NS_END