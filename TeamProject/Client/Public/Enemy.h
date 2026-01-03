#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
typedef struct tagAttackBlackBoard
{
    deque<string> stateQueue;
    _bool isRequestNext = false;//다음 상태가 존재 할 때 상태 전환 요청
    _bool isChainOpen = false;  //현재 상태에서 다음으로 진행 가능여부
    _bool isEnd = false;
    string currentStateTag{};
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