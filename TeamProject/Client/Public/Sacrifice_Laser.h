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

public:
    void ActiveLaser();
    void DeactiveLaser();

public:
    static CSacrifice_Laser* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    void Free() override;

private:
    _bool m_IsPendingDeactive = false;
    _float m_fElapseTime{};
    _float m_fDuration = 0.5f;
};

NS_END