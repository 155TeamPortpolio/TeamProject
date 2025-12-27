#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;


class CSacrifice_Hand :
    public CEnemy
{
private:
    CSacrifice_Hand();
    CSacrifice_Hand(const CSacrifice_Hand& rhg);
    virtual ~CSacrifice_Hand() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

public:
    static CSacrifice_Hand* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    CStateMachine<CSacrifice_Hand>* m_pStateMachine = { nullptr };
};
NS_END
