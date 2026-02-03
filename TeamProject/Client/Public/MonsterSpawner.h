#pragma once
#include "GameObject.h"
#include "MapData_Defines.h"

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

    // 추후에 스포너 타입 지정해서 배틀시스템이 호출할 떄, 스포너 트리거랑 충돌할 때 등 나눌 예정

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
    HRESULT         AddMonsterData(BATTLE_POINT_DATA MonsterPointData, SPAWN_MONSTER_DESC MonsterSpawnData);

private:
    _bool               m_isUsed = { false };
    vector<SPAWN_DESC>  m_SpawnDesc;
    

public:
    static CMonsterSpawner* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END
