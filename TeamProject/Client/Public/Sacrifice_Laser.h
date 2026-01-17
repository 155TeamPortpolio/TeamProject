#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CEffectContainer;
NS_END

NS_BEGIN(Client)

class CSacrifice_Laser :
    public CGameObject
{
private:
    CSacrifice_Laser();
    CSacrifice_Laser(const CSacrifice_Laser& rhg);
    virtual ~CSacrifice_Laser() DEFAULT;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI() override;

public:
    void ActiveLaser(_uint mode);
    void DeactiveLaser();

public:
    static CSacrifice_Laser* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    void Free() override;

private:
    _bool m_IsPendingDeactive = false;
    _uint m_iLaserMode{};
    _float m_fElapseTime{};
    _float m_fDuration = 0.1f;

    /* Target Mode */
    _float3 m_vTargetDir{};
    _float3 m_vTargetPos{};
};

NS_END