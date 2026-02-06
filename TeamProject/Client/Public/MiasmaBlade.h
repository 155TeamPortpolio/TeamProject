#pragma once
#include "Enemy.h"
NS_BEGIN(Client)
class CMiasmaBlade :
    public CEnemy
{
public:
    struct BladeDesc : GAMEOBJECT_DESC {
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
    virtual void OnPooledAcquire(INIT_DESC* pArg = nullptr) override;		// 풀에서 꺼낼 때
    virtual void OnPooledRelease()override;

private:
    _float3 m_vTargetPos = {};
public:
    static CMiasmaBlade* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END
