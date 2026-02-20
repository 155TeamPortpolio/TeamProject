#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CJaegerLaser :
    public CGameObject
{
private:
    CJaegerLaser();
    CJaegerLaser(const CJaegerLaser& rhg);
    virtual ~CJaegerLaser() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI()override;

public:
    void Active_Laser();
    void Deactive_Laser();

private:
    void Initialize_Effects();

public:
    static CJaegerLaser* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    _float m_fDuration{};
    _float m_fElaspedTime{};
};
NS_END
