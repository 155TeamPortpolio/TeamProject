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
    void Set_Target(_float3 targetPos) { m_vTargetPos = targetPos; }

private:
    void Initialize_Effects();
    void Update_Laser(_float dt);

public:
    static CJaegerLaser* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    _float3 m_vTargetPos{};
    _bool m_IsDeactive = false;
    _bool m_IsFlikering = false;
    _float m_fDuration{};
    _float m_fElaspedTime{};
};
NS_END
