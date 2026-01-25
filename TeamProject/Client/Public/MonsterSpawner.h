#pragma once
#include "GameObject.h"
#include "BattleSystem_Struct.h"

NS_BEGIN(Client)
class CMonsterSpawner final	: public CGameObject
{
public:
    typedef struct tagSpawnDesc
    {
        string      tagProto = "";
        _int        iSpawnID = {};
        _float3     vRotation = {};
        _float3     vPosition = {};
    }SPAWN_DESC;

private:
	CMonsterSpawner();
	CMonsterSpawner(const CMonsterSpawner& rhs);
	virtual ~CMonsterSpawner() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
	virtual void    OnTriggerEnter(CGameObject* pOther) override;
    
public:
    HRESULT         AddMonsterData(BATTLE_POINT_DATA MonsterPointData, MONSTER_SPAWN_DESC MonsterSpawnData);

private:
    vector<SPAWN_DESC>   m_SpawnDesc;

    // BattleSystem에서 몬스터 미리 생성해서 풀에 넣어주고
    // 기본적으로 트리거 밟았을 때, 생성하게 해보기
    // OntriggerEnter테스트 후
    // 트리거 오프시키기

public:
    static CMonsterSpawner* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END
