#pragma once
#include "Enemy.h"
NS_BEGIN(Client)
class CMiasmaBlade :
    public CEnemy
{
public:
    struct BladeDesc : GAMEOBJECT_DESC {
        _uint iCount = {};
        _float3 vTargetPos = {};
        class CDefiler* pOwner = { nullptr };
    };

private:
    CMiasmaBlade();
    CMiasmaBlade(const CMiasmaBlade& rhg);
    virtual ~CMiasmaBlade() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI()override;

public:
    virtual void Parried() override;

private:
    _bool   Try_Hit(CGameObject* pTarget);
    void Initialize_Effects();
private:
    _float m_ElapsedTime = {};
    _float m_fMoveSpeed = {70.f};
    _vector3 m_vTargetVelocity = {};
    _vector3 m_vVelocity = {};
    class CDefiler* m_pOwner = { nullptr };
    _bool isParried = { false };

public:
    static CMiasmaBlade* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END
