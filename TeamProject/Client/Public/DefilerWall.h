#pragma once
#include "Enemy.h"
NS_BEGIN(Client)
class CDefilerWall :
    public CEnemy
{
public:
    struct DefilerWallDesc : GAMEOBJECT_DESC{
        _vector3 vLook;
    };

private:
    CDefilerWall();
    CDefilerWall(const CDefilerWall& rhg);
    virtual ~CDefilerWall() DEFAULT;

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
    virtual void OnPooledAcquire(INIT_DESC* pArg = nullptr) override;		// 풀에서 꺼낼 때
    virtual void OnPooledRelease()override;

private:
    void DisAppear();

private:
    _float  m_ElapsedTime = 0.f;
    _float  m_EndY = 0.f;
    _bool m_bAwake = {};
public:
    static CDefilerWall* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END