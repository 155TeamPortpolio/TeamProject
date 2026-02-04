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

void CBattleSystem::ReadyBattle(const string& tagArea, _uint iPrefabIndex)
{
	auto pDataBase = CDataBase::GetInstance();
	auto pObjMgr = ObjectManager();
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
	//const vector<MONSTER_SPAWN_DESC>* pMonsterSpawnData = pDatabase->GetMonsterSpawnData(tagArea);
}
/*지금 싸우려는 애들->*/
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
	enemyDesc->isUseInspector = m_isUseInspector;

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

void CBattleSystem::EnterBattleObject(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject)
{
	auto iter = m_BattleObjInfos.find(eObjType);
	if (iter == m_BattleObjInfos.end())
		return;

	auto& handles = iter->second;
	BATTLEOBJ_INFO info = {};
	info.hObject = hObject;
	handles.push_back(info);
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

void CBattleSystem::TakeBoxDamage(const _float3& vCenter, const _float3& vHalfExtents, const _quaternion& qRotation, const HitDesc& hitDesc)
{
	_quaternion qInverse;
	qRotation.Inverse(qInverse);

	for (auto& info : m_BattleObjInfos[BATTLE_OBJ_TYPE::MONSTER])
	{
		_vector3 vLocal = _vector3::Transform(info.vPos - vCenter, qInverse);

		if (fabs(vLocal.x) > vHalfExtents.x ||
			fabs(vLocal.y) > vHalfExtents.y ||
			fabs(vLocal.z) > vHalfExtents.z)
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
		if (Pair.first == BATTLE_OBJ_TYPE::PLAYER)
			continue;
		for (auto info : Pair.second)
		{
			info.Reset();
		}
		Pair.second.clear();
	}

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
