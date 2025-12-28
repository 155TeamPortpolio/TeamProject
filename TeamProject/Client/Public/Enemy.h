#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
typedef struct tagAttackBlackBoard
{

}ATTACK_BLACK_BOARD;

class CEnemy abstract:
    public CGameObject
{
protected:
    CEnemy();
    CEnemy(const CEnemy& rhg);
    virtual ~CEnemy() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt) override {};
    virtual void    Update(_float dt) override {};
    virtual void    Late_Update(_float dt) override {};

protected:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};
NS_END