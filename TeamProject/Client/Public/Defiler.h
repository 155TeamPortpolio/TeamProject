#pragma once
#include "Enemy.h"
#include "Defiler_Control.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CDefiler :
    public CEnemy
{

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

public:
    DEFILER_BLACK_BOARD& GetBlackBoard() { return m_BlackBoard; }
    DefilerDissolve& GetDissolve() { return m_Dissolve; }
    CStateMachine<CDefiler>* Get_MainStateMachine() { return m_pStateMachine; }
public:
    virtual void TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName = CHARACTER::END);

public:
    void Change_CollisionMask(_uint iMask = ENUM(COLLISION_GROUP::PLAYER));
    void Release_CollisionMask();

public:
    void Set_CCTPos(_vector3 pos);
    _float3 Get_BipedPos();

private:
    void MoveByTraceMode(_float dt, _float moveScale = 1.f);
    void RotateToTarget(_float dt, _float rotateSpeed = 1.f);
    void Update_States(_float dt);
    void Route_AnimEvent(CAnimator3D* animator);
    void Controll_Attack(const string& event);
    void Controll_Summon(const string& event);
    void Update_Dissolve(_float dt);

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Initialize_Effects();
    HRESULT Create_Colliders();
    _float3 Calc_WorldOffsetWithBip();

private:
    CStateMachine<CDefiler>* m_pStateMachine = { nullptr };
    DEFILER_BLACK_BOARD m_BlackBoard = {};
    DefilerDissolve m_Dissolve = {};

    _uint m_BaseMask = 
        ENUM(COLLISION_GROUP::COMMON) | 
        ENUM(COLLISION_GROUP::PLAYER) | 
        ENUM(COLLISION_GROUP::PLAYER_ATTACK);

    _bool m_bDirLockedNear = false;
    _float m_passDampTime = 0.f;

public:
    static CDefiler* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END
