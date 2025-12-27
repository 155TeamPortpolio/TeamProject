#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CSacrifice final:
    public CEnemy
{
private:
    CSacrifice();
    CSacrifice(const CSacrifice& rhg);
    virtual ~CSacrifice() DEFAULT;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    virtual void Post_EngineUpdate(_float dt) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

public:
    static CSacrifice* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();

private:
    CStateMachine<CSacrifice>* m_pStateMachine = { nullptr };
};
NS_END
