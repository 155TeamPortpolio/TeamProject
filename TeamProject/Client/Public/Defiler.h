#pragma once
#include "Enemy.h"
#include "Defiler_Control.h"
#include "MiasmaSpawner.h"

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
    MONSTER_STATUS& GetStatus() { return m_tStatus; }
    TARGETING_INFO& TargetInfo() { return m_tTargetingInfo; }
    CStateMachine<CDefiler>* Get_MainStateMachine() { return m_pStateMachine; }

public:
    virtual void TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName = CHARACTER::END);

public:
    void Change_CollisionMask(_uint iMask = ENUM(COLLISION_GROUP::PLAYER));
    void Release_CollisionMask();
    void Release_AttackCollider();
    void ChainParry(_bool OnStart);
    void HideHUD(_bool hide);

public:
    void Hide_MeshGroup(const string& mesh);
    void Show_MeshGroup(const string& mesh);
    virtual void   Set_Alive(_bool alive) override;

public:
    void Set_CCTPos(_vector3 pos);
    _float3 Get_BipedPos(const string Bone = {"Bip001"});
    FOUR_DIR Get_FourDirection();
    void Control_Sound(const string& event);
    void Control_Summon(const string& event);
    void Control_TargetEnable(_bool On);
    void Update_Dissolve(_float dt);

    void Play_AttackRange(_float3 position, _float scale);
    void Play_Effect(const string& effectTag, _fvector offsetPosition, _fvector offsetQuaternion, _bool syncTransform = true);
    void Stop_Effect(const string& effectTag);

public:
    void Parried() override;
private:
    void MoveByTraceMode(_float dt, _float moveScale = 1.f);
    void RotateToTarget(_float dt, _float rotateSpeed = 1.f);
    void Update_States(_float dt);
    void Route_AnimEvent(CAnimator3D* animator);
    void Controll_Attack(const string& event);
    void Send_DamageText(_float damage, CHARACTER charaName, _bool isSpecial = false);
    void SummonWave();

public:
    void ControlEnv(ENVTYPE type, _bool set);
    void ControlBGM();
public:
    void ResetAllFlags();

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Initialize_Effects();
    HRESULT Create_Colliders();
    _float3 Calc_WorldOffsetWithBip();
    virtual void  Create_UIEnemyStatus(string boneTag) override;
    virtual void  Create_UIBossHUD();

private:
    CStateMachine<CDefiler>* m_pStateMachine = { nullptr };
    DEFILER_BLACK_BOARD m_BlackBoard = {};
    DefilerDissolve m_Dissolve = {};
    CMiasmaSpawner m_MiasmaSpawner = {};
    DefilerMaterialPreset m_MatPreset = {};
    _uint m_BaseMask = 
        ENUM(COLLISION_GROUP::COMMON) | 
        ENUM(COLLISION_GROUP::PLAYER) | 
        ENUM(COLLISION_GROUP::PLAYER_ATTACK);
    _bool m_isRecovering = false;
    _float m_pendToRecover = {};

    _float m_BaseY = {};

    _bool m_bDirLockedNear = false;
    _float m_passDampTime = 0.f;
    _vector3 m_passDir = { 0.f, 0.f, 1.f };
    _bool    m_hasPassDir = false;
    _vector3 m_lastMoveDir = _vector3(0.f, 0.f, 1.f);
    _bool    m_hasLastMoveDir = false;
    _vector3 m_lastTargetPos = _vector3(FLT_MAX, FLT_MAX, FLT_MAX);
    _bool    m_prevPassedTarget = false;
    _bool    m_passArmed = false; // 근접했을 때만 "패스 판정" 허용

    UI_HANDLE m_BoneHUD;
public:
    static CDefiler* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END
