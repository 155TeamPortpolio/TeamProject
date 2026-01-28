#include "pch.h"
#include "BattleSystem.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "CharacterController.h"
#include "BattlePlayer.h"
#include "DataBase.h"
#include "FieldSystem.h"
#include "Enemy.h"
#include "MonsterSpawner.h"
#include "Character.h"
#include "BattleFXFlow.h"

IMPLEMENT_SINGLETON(CBattleSystem)

CBattleSystem::CBattleSystem()
{
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_MonsterSpawner", CMonsterSpawner::Create());

	// 빈 값 채우기
	for (_int i = 0; i < static_cast<_int>(BATTLE_OBJ_TYPE::END); ++i) {
		auto eType = static_cast<BATTLE_OBJ_TYPE>(i);
		vector<BATTLEOBJ_INFO> infos;
		infos.reserve(10);
		m_BattleObjInfos.emplace(eType, move(infos));
	}

	
	m_pFXFlow = CBattleFXFlow::Create();
	m_pFXFlow->Initialize_Preset();
	m_pFXFlow->SetLayerTag(BATTLE_OBJ_TYPE::PLAYER, "Model_Layer");
	m_pFXFlow->SetLayerTag(BATTLE_OBJ_TYPE::MONSTER, "Enemy_Layer");
}

void CBattleSystem::Update()
{
	if (false == m_isActive)
		return;

	const _float dt = CGameInstance::GetInstance()->Get_EngineDeltaTime();

	CheckVFX(dt);
	Update_BattleInfo();

	if(InputDevice()->Key_Tap(VK_SHIFT))
	{
		StartGimmick(BATTLE_VFX_TYPE::PARRY);
	}
}


OBJECT_HANDLE CBattleSystem::GetCurCharacterHandle() const
{
	return m_pBattlePlayer->GetCurCharacterHandle();
}

const vector<BATTLEOBJ_INFO>& CBattleSystem::GetBattleObjects(BATTLE_OBJ_TYPE eType) const
{
	return m_BattleObjInfos.at(eType);
}

vector<BATTLEOBJ_INFO> CBattleSystem::CopyBattleObjects(BATTLE_OBJ_TYPE eType)
{
	return m_BattleObjInfos[eType];
}

_int CBattleSystem::GetPlayerParryingCount()
{
	if (nullptr == m_pBattlePlayer)
		return -1;

	return m_pBattlePlayer->GetParryingCount();
}

void CBattleSystem::ReadyBattle(const string& tagArea, _uint iPrefabIndex)
{
	auto pDataBase = CDataBase::GetInstance(); 
	auto pObjMgr = ObjectManager();

#pragma region  BattleData Load
	//const auto pBattleDataPacket = pDataBase->GetBattleFieldDataPacket(tagArea);
	//
	//if (nullptr == pBattleDataPacket)
	//	return;
	//
	//filesystem::path OpenPath = pBattleDataPacket->TagDataFilePath;
	//
	//if (OpenPath.empty())
	//	return;
	//
	//if (OpenPath.extension().string() != ".json") 
	//{
	//	MSG_BOX("[BattleSystem] Load BattleData Failed.\nJson 파일이 아닙니다.");
	//	return;
	//}
	//
	//m_BattleFieldData = BATTLE_FIELD_DATA{};
	//
	//m_BattleFieldData = Helper::LoadJson<BATTLE_FIELD_DATA>(OpenPath.string());
	//if ("BattleData" != m_BattleFieldData.TagDataFormat)
	//	return;
#pragma endregion

#pragma region MonsterSpawn Data Load
	//const vector<SPAWN_MONSTER_DESC>* pMonsterSpawnData = pDataBase->GetMonsterSpawnData(tagArea);
	//const EncounterTable* pMonsterSpawnData = pDataBase->GetMonsterSpawnData(tagArea)

	//if (nullptr == pMonsterSpawnData)
	//	return;
	//
	//unordered_map<_int, SPAWN_MONSTER_DESC> MonsterSpawnDataContainer;
	//for (auto& SpawnData : *pMonsterSpawnData)
	//{
	//	// 천 단위 == 몬스터 매핑 테이블 인덱스
	//	_uint version = SpawnData.MonsterID / 1000;
	//	_uint MonsterSpawnIndex = SpawnData.MonsterSpawnID % 1000;
	//	if (iPrefabIndex == version)
	//		MonsterSpawnDataContainer.emplace(MonsterSpawnIndex, SpawnData);
	//		//m_MonsterSpawnData.push_back(SpawnData);
	//}
#pragma endregion
	
// 몬스터 스폰 포인트용 데이터 정리
#pragma region Packing MonsterPointData
	unordered_map<_int, BATTLE_POINT_DATA> MonsterPointDataContainer;
	for (auto& MonsterPointData : m_BattleFieldData.Monsters)
	{
		if ("MonsterPoint" != MonsterPointData.tagType)
			continue;

		MonsterPointDataContainer.emplace(MonsterPointData.iIndex, MonsterPointData);
	}
#pragma endregion


// 스포너 세팅
//#pragma region Setting Spawner
//	for (auto& SpawnerData : m_BattleFieldData.Spawners)
//	{
//		if ("SpawnerPoint" != SpawnerData.tagType)
//			continue;
//
//		COLLIDER_DESC ColDesc = {};
//		ColDesc.eType = COLLIDER_TYPE::BOX;
//		ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
//		ColDesc.vSize = { SpawnerData.vScale[0], SpawnerData.vScale[1], SpawnerData.vScale[2] };
//		
//		// 추후에 스포너 타입(트리거랑 충돌, 직접 호출 등)이 필요해 보임
//
//		string tagInstanceName = SpawnerData.tagType + to_string(SpawnerData.iIndex);
//		CGameObject* pSpawnerObject = Builder::Create_Object({ G_GlobalLevelKey ,"Proto_GameObject_MonsterSpawner" })
//			.Collider(ColDesc)
//			.Position({ SpawnerData.vTranslation[0], SpawnerData.vTranslation[1], SpawnerData.vTranslation[2] })
//			.Build(tagInstanceName);
//
//#ifdef _USING_GUI
//		pSpawnerObject->Get_Component<CCollider>()->Set_DebugRender(true);
//#endif 
//		pObjMgr->Add_Object(pSpawnerObject, { LevelManager()->Get_NowLevelKey(), "Spawner_Layer"});
//
//		// 나중에 밟아서 활성화 되는 스포너 외에 타이밍 제어에 필요한 스포너가 있을 경우,
//		// 핸들로 접근해서 소환시키게 하려고 스포너 핸들 저장해둠
//		m_SpawnerHandles.push_back(pSpawnerObject->Get_Handle());
//
//		// 스포너가 포함하고 있는 몬스터포인트에 매핑된 값들을 찾아서 넣음
//		for (size_t i = 0;  i < SpawnerData.MonsterIndices.size();  i++)
//		{
//			auto SpawnDataiter = MonsterSpawnDataContainer.find(SpawnerData.MonsterIndices[i]);
//			if (SpawnDataiter == MonsterSpawnDataContainer.end())
//				continue;
//
//			auto PointDataiter = MonsterPointDataContainer.find(SpawnerData.MonsterIndices[i]);
//			if (PointDataiter == MonsterPointDataContainer.end())
//				continue;
//
//			static_cast<CMonsterSpawner*>(pSpawnerObject)->AddMonsterData(PointDataiter->second, SpawnDataiter->second);
//		}
//	}
//#pragma endregion

// 몬스터 미리 세팅 (오브젝트 풀에 넣어놓기)
// 을 의도했으나 일단 그냥 소환하는 방식으로 ㄱㄱ
#pragma region Setting Monster(X)
	// 풀에 들어간 몬스터 프로토 태그 저장용
	/*vector<string>		AddMonsterList;

	for (auto& data : *pMonsterSpawnData)
	{
		string tagProto = "Proto_GameObject_" + data.MonsterKey;

		if (find(AddMonsterList.begin(), AddMonsterList.end(), tagProto) != AddMonsterList.end())
			continue;

		MonsterCreationDesc MonsterTableDesc = CDataBase::GetInstance()->GetMonsterDesc(tagProto);
		if (true == MonsterTableDesc.ProtoTag.empty())
			return;

		CCT_DESC MonsterCCT;
		MonsterCCT.eGroup = COLLISION_GROUP::MONSTER;
		MonsterCCT.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::COMMON) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
		MonsterCCT.bAutoFit = false;
		MonsterCCT.fHeight = MonsterTableDesc.CCT_fHeight;
		MonsterCCT.fRadius = MonsterTableDesc.CCT_fRadius;

		CEnemy::ENEMY_DESC* enemyDesc = new CEnemy::ENEMY_DESC();
		enemyDesc->iMaxHP = MonsterTableDesc.iMaxHP;

		const string NowLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

		auto pMonster = Builder::Create_Object({ NowLevel, MonsterTableDesc.ProtoTag })
			.Add_ObjDesc(enemyDesc)
			.CharacterController(MonsterCCT)
			.FromPool()
			.Build(MonsterTableDesc.DisplayName);

		// 풀에 넣어야함


		if (nullptr == pMonster)
			return;
		CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pMonster, { NowLevel, "Enemy_Layer" });

		AddMonsterList.push_back(tagProto);
	}*/
#pragma endregion

// 포탈(끝지점) 세팅
#pragma region Setting EndPoint
	// 포탈 만드는 로직 추가 필요
#pragma endregion

}

void CBattleSystem::ReadyBattle(const string& tagArea, _uint StageNumber, _uint iPrefabIndex)
{
	auto pDatabase = CDataBase::GetInstance();
	auto CacheData = CDataBase::GetInstance()->Get_CashedData(tagArea);

	if (!CacheData->Battle.HasBattleData)
		return;

	/*PlayerPos*/
	CacheData->Battle.PlayerPoint;

	/*MonsterPos*/
	CacheData->Battle.MonsterPoint;
	CacheData->Battle.PortalPoint;
	const vector<MONSTER_SPAWN_DESC>* pMonsterSpawnData = pDatabase->GetMonsterSpawnData(tagArea);

	
}


void CBattleSystem::SetActive(_bool isActive)
{
	if (false == isActive) {
		m_isActive = false;
		ClearBattleStage();
		return;
	}
	else {
		m_isActive = true;
	}
}

void CBattleSystem::SpawnMosnter(const string& MonsterProtoTag, _float3 vSpawnPos, _float3 vRot)
{
	MonsterCreationDesc MonsterTableDesc = CDataBase::GetInstance()->GetMonsterDesc(MonsterProtoTag);
	if (true == MonsterTableDesc.ProtoTag.empty())
		return;

	CCT_DESC MonsterCCT;
	MonsterCCT.eGroup = COLLISION_GROUP::MONSTER;
	MonsterCCT.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::COMMON) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
	MonsterCCT.bAutoFit = false;
	MonsterCCT.fHeight = MonsterTableDesc.CCT_fHeight;
	MonsterCCT.fRadius = MonsterTableDesc.CCT_fRadius;
	MonsterCCT.vPos = vSpawnPos;
	MonsterCCT.vPos.y += MonsterCCT.fHeight;
	
	CEnemy::ENEMY_DESC* enemyDesc = new CEnemy::ENEMY_DESC();
	enemyDesc->iMaxHP = MonsterTableDesc.iMaxHP;

	const string NowLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

	auto pMonster = Builder::Create_Object({ NowLevel,MonsterTableDesc.ProtoTag })
		.Add_ObjDesc(enemyDesc)
		.CharacterController(MonsterCCT)
		.Rotate(vRot)
		.Build(MonsterTableDesc.DisplayName);

	if (nullptr == pMonster)
		return;

	CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pMonster, { NowLevel, "Enemy_Layer" });

	BATTLEOBJ_INFO EnemyInfo = {};
	EnemyInfo.hObject = pMonster->Get_Handle();
	m_BattleObjInfos[BATTLE_OBJ_TYPE::MONSTER].push_back(EnemyInfo);
}

void CBattleSystem::SpawnMosnterFromPool(const string& MonsterProtoTag, _float3 vSpawnPos, _float3 vRot)
{
	MonsterCreationDesc MonsterTableDesc = CDataBase::GetInstance()->GetMonsterDesc(MonsterProtoTag);
	if (true == MonsterTableDesc.ProtoTag.empty())
		return;

	_float3 vCorrectionSpawnPos = vSpawnPos;
	vCorrectionSpawnPos.y += MonsterTableDesc.CCT_fHeight;

	const string NowLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
	auto pMonster = Builder::Create_Object({ NowLevel, MonsterProtoTag })
		.Position(vCorrectionSpawnPos)
		.Rotate(vRot)
		.FromPool()
		.Build(MonsterTableDesc.DisplayName);

	if (nullptr == pMonster)
		return;

	CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pMonster, { NowLevel, "Enemy_Layer" });

	BATTLEOBJ_INFO EnemyInfo = {};
	EnemyInfo.hObject = pMonster->Get_Handle();
	m_BattleObjInfos[BATTLE_OBJ_TYPE::MONSTER].push_back(EnemyInfo);
}

_bool CBattleSystem::ExitBattleObject(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject)
{
	auto iter = m_BattleObjInfos.find(eObjType);
	if(iter == m_BattleObjInfos.end())
		return false;

	auto& handles = iter->second;

	auto hiter = find_if(
		handles.begin(),
		handles.end(),
		[&](BATTLEOBJ_INFO& info){return info.hObject == hObject;});

	if (hiter == handles.end())
		return false;

	handles.erase(hiter);
	return true;
}

void CBattleSystem::SetPlayer(vector<OBJECT_HANDLE> hPlayers)
{
	for (auto& hPlayer : hPlayers)
	{
		if (!hPlayer.isValid()) continue;

		BATTLEOBJ_INFO playerInfo = {};
		playerInfo.hObject = hPlayer;
		m_BattleObjInfos[BATTLE_OBJ_TYPE::PLAYER].push_back(playerInfo);
	}
}

void CBattleSystem::SetBattleCharacters(vector<CHARACTER> battleCharacters)
{
	m_pBattlePlayer->SetBattleCharacters(battleCharacters);
}

void CBattleSystem::TakeAreaDamage(const _float3& vCenter, _float fRadius, const HitDesc& hitDesc)
{
	_float fRadiusSq = fRadius * fRadius;

	for (auto& info : m_BattleObjInfos[BATTLE_OBJ_TYPE::MONSTER])
	{
		_float3 vDiff = info.vPos - vCenter;
		_float fDistSq = vDiff.x * vDiff.x + vDiff.y * vDiff.y + vDiff.z * vDiff.z;

		if (fDistSq > fRadiusSq)
			continue;

		auto pEnemy = dynamic_cast<CEnemy*>(info.hObject.Get());
		if (pEnemy)
			pEnemy->TakeDamage(hitDesc.eDamageType, hitDesc.fDamage);
	}
}

void CBattleSystem::TakeAllDamage(const HitDesc& hitDesc)
{
	for (auto& info : m_BattleObjInfos[BATTLE_OBJ_TYPE::MONSTER])
	{
		auto& handle = info.hObject;

		if (!handle.isValid())
			continue;
		auto pEnemy = dynamic_cast<CEnemy*>(handle.Get());
		if (pEnemy)
		{
			pEnemy->TakeDamage(hitDesc.eDamageType, hitDesc.fDamage);
		}
	}
}

void CBattleSystem::ClearBattleStage()
{
	for (auto& Pair : m_BattleObjInfos) {
		for (auto info : Pair.second)
		{
			info.hObject.Delete();
			info.Reset();
		}
		Pair.second.clear();
	}

	m_SpawnerHandles.clear();
	m_BattleFieldData = {};
}

void CBattleSystem::Update_BattleInfo()
{
 	for (auto& infovector : m_BattleObjInfos)
	{
		for (auto& info : infovector.second)
		{
			if (!info.hObject.isValid()) continue;

			CGameObject* pObject = info.hObject.Get();
			_float4 objWorldPos = pObject->Get_Position();
			info.TagInstanceName = pObject->Get_InstanceName();
			info.vPos = { objWorldPos.x, objWorldPos.y,objWorldPos.z };
			auto* cct = pObject->Get_Component<CCharacterController>();
			info.fRadius = (cct != nullptr)? cct->Get_Radius():0;
			info.isOnField = true;
		}
	}
}

_bool CBattleSystem::isMonsterCleared()
{
	return m_BattleObjInfos[BATTLE_OBJ_TYPE::MONSTER].empty();
}

void CBattleSystem::CheckVFX(const _float dt)
{
	if (!m_pFXFlow->IsRunning())
		return;

	m_pFXFlow->Update(dt);
}

void CBattleSystem::StartGimmick(BATTLE_VFX_TYPE eVFXType)
{
	m_pFXFlow->StartVfx(eVFXType);
}

void CBattleSystem::StartTimeScale(BATTLE_OBJ_TYPE eObjType, _float fDuration, _float fScale, _float fStartLerpTime, _float fEndLerpTime)
{
}

void CBattleSystem::Free()
{
	__super::Free();
	Safe_Release(m_pFXFlow);
}
