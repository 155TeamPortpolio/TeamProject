#include "pch.h"
#include "MonsterSpawner.h"
#include "GameInstance.h"
#include "Collider.h"
#include "BattleSystem.h"


CMonsterSpawner::CMonsterSpawner()
	: CGameObject()
{
}

CMonsterSpawner::CMonsterSpawner(const CMonsterSpawner& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterSpawner::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CMonsterSpawner::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CMonsterSpawner::Awake()
{
}

void CMonsterSpawner::Priority_Update(_float dt)
{
}

void CMonsterSpawner::Update(_float dt)
{
}

void CMonsterSpawner::Late_Update(_float dt)
{
}

void CMonsterSpawner::OnTriggerEnter(CGameObject* pOther)
{
	if (true == m_isUsed)
		return;

	for (auto& data : m_SpawnDesc)
	{
		string tagInstanceName = to_string(data.iSpawnID) + "_" + data.tagProto;
		BattleSystem()->SpawnMosnter(data.tagProto, data.vPosition, data.vRotation);
	}

	m_isUsed = true;
}

HRESULT CMonsterSpawner::AddMonsterData(BATTLE_POINT_DATA MonsterPointData, SPAWN_MONSTER_DESC MonsterSpawnData)
{
	//if ("MonsterPoint" != MonsterPointData.tagType || true == MonsterSpawnData.MonsterID.empty())
	//	return E_FAIL;
	//
	//_int iWithoutTableIndex = MonsterSpawnData.MonsterSpawnID % 1000;
	//
	//// 인덱스 정보들이 맞지 않다면 취소
	//if (iWithoutTableIndex != MonsterPointData.iIndex)
	//	return E_FAIL;
	//
	//SPAWN_DESC desc = {};
	//desc.tagProto = "Proto_GameObject_" + MonsterSpawnData.MonsterKey;
	//desc.iSpawnID = MonsterPointData.iIndex;
	//desc.vRotation = { MonsterPointData.vRotation[0], MonsterPointData.vRotation[1],MonsterPointData.vRotation[2] };
	//desc.vPosition = { MonsterPointData.vTranslation[0], MonsterPointData.vTranslation[1],MonsterPointData.vTranslation[2] };
	//
	//m_SpawnDesc.push_back(desc);
	//
	return S_OK;
}

CMonsterSpawner* CMonsterSpawner::Create()
{
	CMonsterSpawner* instance = new CMonsterSpawner();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CMonsterSpawner");
	}

	return instance;
}

CGameObject* CMonsterSpawner::Clone(INIT_DESC* pArg)
{
	CMonsterSpawner* instance = new CMonsterSpawner(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CMonsterSpawner");
	}

	return instance;
}

void CMonsterSpawner::Free()
{
	__super::Free();
}