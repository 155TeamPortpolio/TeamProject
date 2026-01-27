#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CDefiler :
    public CEnemy
{
    struct Defiler_Target {

    };
private:
    CDefiler();
    CDefiler(const CDefiler& rhg);
    virtual ~CDefiler() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

    void    Render_GUI();

private:
    void MoveByRootMotion(_float dt, _float moveScale = 1.f);
    void RotateToTarget(_float dt, _float rotateSpeed = 1.f);
    void Update_States(_float dt);

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    //HRESULT Create_Colliders();

private:
    CStateMachine<CDefiler>* m_pStateMachine = { nullptr };

public:
    static CDefiler* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

};

NS_END
