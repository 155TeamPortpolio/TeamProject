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

IMPLEMENT_SINGLETON(CBattleSystem)

CBattleSystem::CBattleSystem()
{
	// 빈 값 채우기
	for (_int i = 0; i < static_cast<_int>(BATTLE_OBJ_TYPE::END); ++i) {
		auto eType = static_cast<BATTLE_OBJ_TYPE>(i);
		m_BattleObjInfos.emplace(eType, vector<BATTLEOBJ_INFO>{});
	}
	m_TimeScaling.resize(ENUM(BATTLE_OBJ_TYPE::END), {});
	m_BattleVFXData.resize(ENUM(BATTLE_VFX_TYPE::END), {});

	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_MonsterSpawner", CMonsterSpawner::Create());

	// 임시
	{
		/* fDuration, fValue, fStartLerpTime */
		m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::EVADE)].fVFXDuration = 2.5f;
		m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::EVADE)].fBlurDuration = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::EVADE)].fVFXDuration/2.f;
		m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::EVADE)].tPlayerTimeScale =	{ m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::EVADE)].fVFXDuration / 3.f, 0.1f,0.f,0.f};
		m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::EVADE)].tMonsterTimeScale =	{ m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::EVADE)].fVFXDuration, 0.1f, 0.f, 0.f};
	}
}

void CBattleSystem::Update()
{
	if (false == m_isActive)
		return;

	const _float dt = CGameInstance::GetInstance()->Get_EngineDeltaTime();

	CheckTimeScale(dt);
	CheckVFX(dt);

	Update_BattleInfo();
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
	const auto pBattleDataPacket = pDataBase->GetBattleFieldDataPacket(tagArea);
	
	if (nullptr == pBattleDataPacket)
		return;

	filesystem::path OpenPath = pBattleDataPacket->TagDataFilePath;

	if (OpenPath.empty())
		return;

	if (OpenPath.extension().string() != ".json") 
	{
		MSG_BOX("[BattleSystem] Load BattleData Failed.\nJson 파일이 아닙니다.");
		return;
	}

	m_BattleFieldData = {};

	m_BattleFieldData = Helper::LoadJson<BATTLE_FIELD_DATA>(OpenPath.string());
	if ("BattleData" != m_BattleFieldData.TagDataFormat)
		return;
#pragma endregion

#pragma region MonsterSpawn Data Load
	const vector<MONSTER_SPAWN_DESC>* pMonsterSpawnData = pDataBase->GetMonsterSpawnData(tagArea);
	
	if (nullptr == pMonsterSpawnData)
		return;

	unordered_map<_int, MONSTER_SPAWN_DESC> MonsterSpawnDataContainer;
	for (auto& SpawnData : *pMonsterSpawnData)
	{
		// 천 단위 == 몬스터 매핑 테이블 인덱스
		_uint version = SpawnData.MonsterSpawnID / 1000;
		_uint MonsterSpawnIndex = SpawnData.MonsterSpawnID % 1000;
		if (iPrefabIndex == version)
			MonsterSpawnDataContainer.emplace(MonsterSpawnIndex, SpawnData);
			//m_MonsterSpawnData.push_back(SpawnData);
	}
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

// 플레이어 위치 세팅
#pragma region Setting Player Position
#pragma endregion

// 스포너 세팅
#pragma region Setting Spawner
	for (auto& SpawnerData : m_BattleFieldData.Spawners)
	{
		if ("SpawnerPoint" != SpawnerData.tagType)
			continue;

		COLLIDER_DESC ColDesc = {};
		ColDesc.eType = COLLIDER_TYPE::BOX;
		ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
		ColDesc.vSize = { SpawnerData.vScale[0], SpawnerData.vScale[1], SpawnerData.vScale[2] };
		
		string tagInstanceName = SpawnerData.tagType + to_string(SpawnerData.iIndex);
		CGameObject* pSpawnerObject = Builder::Create_Object({ G_GlobalLevelKey ,"Proto_GameObject_MonsterSpawner" })
			.Collider(ColDesc)
			.Position({ SpawnerData.vTranslation[0], SpawnerData.vTranslation[1], SpawnerData.vTranslation[2] })
			.Build(tagInstanceName);

#ifdef _USING_GUI
		pSpawnerObject->Get_Component<CCollider>()->Set_DebugRender(true);
#endif 
		pObjMgr->Add_Object(pSpawnerObject, { LevelManager()->Get_NowLevelKey(), "Spawner_Layer"});

		m_SpawnerHandles.push_back(pSpawnerObject->Get_Handle());

		// 스포너가 포함하고 있는 몬스터포인트에 매핑된 값들을 찾아서 넣음
		for (size_t i = 0;  i < SpawnerData.MonsterIndices.size();  i++)
		{
			auto SpawnDataiter = MonsterSpawnDataContainer.find(SpawnerData.MonsterIndices[i]);
			if (SpawnDataiter == MonsterSpawnDataContainer.end())
				continue;

			auto PointDataiter = MonsterPointDataContainer.find(SpawnerData.MonsterIndices[i]);
			if (PointDataiter == MonsterPointDataContainer.end())
				continue;

			static_cast<CMonsterSpawner*>(pSpawnerObject)->AddMonsterData(PointDataiter->second, SpawnDataiter->second);
		}
	}
#pragma endregion

// 몬스터 미리 세팅 (오브젝트 풀에 넣어놓기)
#pragma region Setting Monster
	// 풀에 들어간 몬스터 프로토 태그 저장용
	vector<string>		AddMonsterList;

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
	}
#pragma endregion

// 포탈(끝지점) 세팅
#pragma region Setting EndPoint
	// 포탈 만드는 로직?
#pragma endregion

}

void CBattleSystem::SpawnMosnter(const string& MonsterProtoTag, _float3 vSpawnPos)
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
		.Build(MonsterTableDesc.DisplayName);

	if (nullptr == pMonster)
		return;
	
	CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pMonster, { NowLevel, "Enemy_Layer"});

	m_Handles[BATTLE_OBJ_TYPE::MONSTER].push_back(pMonster->Get_Handle());
}

_bool CBattleSystem::ExitBattleObject(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject)
{
	auto iter = find(m_Handles[eObjType].begin(), m_Handles[eObjType].end(), hObject);
	if (iter == m_Handles[eObjType].end())
		return false;

	m_Handles[eObjType].erase(iter);
	return true;
}

void CBattleSystem::SetPlayer(vector<OBJECT_HANDLE> hPlayers)
{
	for (auto& hPlayer : hPlayers)
	{
		if (hPlayer.isValid())
			m_Handles[BATTLE_OBJ_TYPE::PLAYER].push_back(hPlayer);
	}
}

void CBattleSystem::SetBattleCharacters(vector<CHARACTER> battleCharacters)
{
	m_pBattlePlayer->SetBattleCharacters(battleCharacters);
}

void CBattleSystem::StartGimmick(BATTLE_VFX_TYPE eVFXType)
{
	if (true == m_BattleVFX.isRunning)
		return;

	StartShaderVFX(eVFXType);
}

void CBattleSystem::StartTimeScale(BATTLE_OBJ_TYPE eObjType, _float fDuration, _float fScale, _float fStartLerpTime, _float fEndLerpTime)
{
	// PLAYER, MONSTER 가 아닐때, 예외
	if (ENUM(BATTLE_OBJ_TYPE::ENVOBJECT) <= ENUM(eObjType))
		return;

	// 값들이 음수일때, 예외
	if (fDuration < 0.f || fScale < 0.f)
		return;

	m_TimeScaling[ENUM(eObjType)].isRunning = true;
	m_TimeScaling[ENUM(eObjType)].fDuration = fDuration;
	m_TimeScaling[ENUM(eObjType)].fCurPos = 0.f;
	m_TimeScaling[ENUM(eObjType)].fScaleValue = fScale;
	m_TimeScaling[ENUM(eObjType)].vStartLerpTime = { fStartLerpTime, 0.f };
	m_TimeScaling[ENUM(eObjType)].vEndLerpTime = { fEndLerpTime, 0.f };

}

void CBattleSystem::StartShaderVFX(BATTLE_VFX_TYPE eVFXType)
{
	// 처음에
	//RenderSystem()->Apply_RadialBlur(fDuration);
	//RenderSystem()->Register_AddictiveColor(&m_vLerpColor);
	//여기서 m_vLerpColor를 멤버변수로 가지고 업데이트때 색상을 보간주든 해서 효과를 부여.
	//끝날때
	//RenderSystem()->UnRegister_AddictiveColor();
	
	if (true == m_BattleVFX.isRunning)
		return;

	BATTLE_VFX_DATA tVFX  = m_BattleVFXData[ENUM(eVFXType)];

	// 스케일값이 있을 때 fDuration 동안 스케일 적용
	if (1 != tVFX.tPlayerTimeScale.fValue &&
		0 < tVFX.tPlayerTimeScale.fValue)
		StartTimeScale(
			BATTLE_OBJ_TYPE::PLAYER, 
			tVFX.tPlayerTimeScale.fDuration,
			tVFX.tPlayerTimeScale.fValue,
			tVFX.tPlayerTimeScale.fStartLerpTime,
			tVFX.tPlayerTimeScale.fEndLerpTime
		);
	if (1 != tVFX.tMonsterTimeScale.fValue &&
		0 < tVFX.tMonsterTimeScale.fValue)
		StartTimeScale(
			BATTLE_OBJ_TYPE::MONSTER, 
			tVFX.tMonsterTimeScale.fDuration,
			tVFX.tMonsterTimeScale.fValue,
			tVFX.tMonsterTimeScale.fStartLerpTime,
			tVFX.tMonsterTimeScale.fEndLerpTime
		);
	// 이펙트는 추후 추가예정
	//if (1 != tVFX.tEffectTimeScale.fValue &&
	// 0 < tVFX.tEffectTimeScale.fValue)
	//	StartTimeScale(
	//		BATTLE_OBJ_TYPE::PLAYER, 
	//		tVFX.tEffectTimeScale.fDuration,
	//		tVFX.tEffectTimeScale.fValue,
	//		tVFX.tEffectTimeScale.fStartLerpTime,
	//		tVFX.tEffectTimeScale.fEndLerpTime
	//	);

	m_BattleVFX.eVFXType = eVFXType;
	m_BattleVFX.isRunning = true;
	m_BattleVFX.fDuration = tVFX.fVFXDuration;
	m_BattleVFX.fCurPos = 0.f;
	m_BattleVFX.vLerpColor = {};

	RenderSystem()->Apply_RadialBlur(tVFX.fBlurDuration);
	RenderSystem()->Register_AddictiveColor(&m_BattleVFX.vLerpColor);
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
	for (auto& handle : m_Handles[BATTLE_OBJ_TYPE::MONSTER])
	{
		if (!handle.isValid())
			continue;

		auto pEnemy = dynamic_cast<CEnemy*>(handle.Get());
		if (pEnemy)
		{
			pEnemy->TakeDamage(hitDesc.eDamageType, hitDesc.fDamage);
		}
	}
}

_bool CBattleSystem::isMonsterCleared()
{
	return m_BattleObjInfos[BATTLE_OBJ_TYPE::MONSTER].empty(); 
}

void CBattleSystem::Update_BattleInfo()
{
	for (_int i = 0; i < static_cast<_int>(BATTLE_OBJ_TYPE::END); ++i) {
		auto eType = static_cast<BATTLE_OBJ_TYPE>(i);
		m_BattleObjInfos[eType].clear();

		size_t test = m_Handles[eType].size();

		for (size_t j = 0; j < m_Handles[eType].size(); ++j) {
			auto handle = m_Handles[eType][j];
			if (false == handle.isValid())
				continue;

			CGameObject* pObject = m_Handles[eType][j].Get();

			_float4x4 mObjWorld = pObject->Get_Component<CTransform>()->Get_WorldMatrix();

			BATTLEOBJ_INFO info = {};
			info.TagInstanceName = m_Handles[eType][j].Get()->Get_InstanceName();
			info.hObject = m_Handles[eType][j];
			info.vPos = { mObjWorld._41, mObjWorld._42,mObjWorld._43 };
			info.fRadius = pObject->Get_Component<CCharacterController>()->Get_Radius();
			info.isOnField = true;

			m_BattleObjInfos[eType].push_back(info);
		}
	}
}

void CBattleSystem::ClearBattleStage()
{
	for (auto& Pair : m_Handles) 
		Pair.second.clear();

	for (auto& Pair : m_BattleObjInfos)
		Pair.second.clear();
}

void CBattleSystem::CheckTimeScale(const _float dt)
{
	for (_uint i = 0; i < ENUM(BATTLE_OBJ_TYPE::END); i++)
	{
		if (ENUM(BATTLE_OBJ_TYPE::ENVOBJECT) <= i)
			continue;
		
		auto& TimeScaling = m_TimeScaling[i];

		if (true == TimeScaling.isRunning)
		{
			string tagNowLevel = LevelManager()->Get_NowLevelKey();
			
			/* x : duration, y : curpos */
			const _bool hasStartLerp =
				0.f < TimeScaling.vStartLerpTime.x &&
				TimeScaling.vStartLerpTime.x > TimeScaling.vStartLerpTime.y;

			// 시작 보간있을 때
			if (hasStartLerp)
			{
				TimeScaling.vStartLerpTime.y += dt;

				_float T = clamp(TimeScaling.vStartLerpTime.y / TimeScaling.vStartLerpTime.x, 0.f, 1.f);
				_float fResultScale = Math::Lerp(1.f, TimeScaling.fScaleValue, T);

				ObjectManager()->Set_LayerTimeScale({ tagNowLevel, m_LayerTag[i] }, fResultScale);

				if (TimeScaling.vStartLerpTime.y >= TimeScaling.vStartLerpTime.x)
					TimeScaling.vStartLerpTime.y = TimeScaling.vStartLerpTime.x;
			}
			else 
			{
				// 시작보간 끝난 후 혹은 없을 때

				TimeScaling.fCurPos += dt;

				_float T = TimeScaling.GetTimeRatio();

				if (TimeScaling.fCurPos >= TimeScaling.fDuration)
				{
					ObjectManager()->Reset_LayerTimeScale({ LevelManager()->Get_NowLevelKey(), m_LayerTag[i] });
					TimeScaling.isRunning = false;
					TimeScaling.fCurPos = 0.f;

					TimeScaling.vStartLerpTime.y = 0.f;
					TimeScaling.vEndLerpTime.y = 0.f;
				}
				else
					ObjectManager()->Set_LayerTimeScale({ tagNowLevel, m_LayerTag[i] }, TimeScaling.fScaleValue);

			}
		}
	}
}

void CBattleSystem::CheckVFX(const _float dt)
{
	if (false == m_BattleVFX.isRunning)
		return;

	m_BattleVFX.fCurPos += dt;

	// 0 나누기 방지 및 클램프 처리 되어있음
	_float T = m_BattleVFX.GetTimeRatio();

	// 시간만 관리
	if (T > 0.99f)
	{
		RenderSystem()->UnRegister_AddictiveColor();
		m_BattleVFX.fCurPos = {};
		m_BattleVFX.vLerpColor = {};
		m_BattleVFX.isRunning = false;
		return;
	}

	switch (m_BattleVFX.eVFXType)
	{
	case Client::BATTLE_VFX_TYPE::EVADE:
	{
		_float normalizedT = 1 - T;
		_float pingpongT = (normalizedT < 0.5f) ? (normalizedT * 2.f) : (2.f - normalizedT * 2.f);
		_float EaseT = Math::ApplyEase(EaseType::InOutSine, pingpongT);

		//_float EaseT = {};
		//
		//// 시작 T = 0 ~ 0.2 : 0 -> 1
		//if (T < 0.2f)
		//{
		//	_float x = T / 0.2f;								// 0~1
		//	EaseT = Math::ApplyEase(EaseType::InOutSine, x);	// 0~1
		//}
		//// 중간 T = 0.2 ~ 0.8 : 1 고정
		//else if (T < 0.8f)
		//	EaseT = 1.f;
		//else
		//{
		//	// 3) T = 끝 0.8 ~ 1 : 1 -> 0
		//	_float x = (T - (1.f - 0.2f)) / 0.2f;					// 0~1
		//	EaseT = Math::Lerp(1.f, 0.f, x);  // 1~0
		//}
		//_float3 vTarget = { 0.1f, 0.54f, 0.58f};
		_float3 vTarget = { 0.1f, 0.3f, 0.3f};

		_vector vZero = XMVectorZero();
		_vector	vTargetColor = XMLoadFloat3(&vTarget);
		XMStoreFloat3(&m_BattleVFX.vLerpColor, XMVectorLerp(vZero, vTargetColor, EaseT));

		RenderSystem()->Register_AddictiveColor(&m_BattleVFX.vLerpColor);


		break;
	}
	}
}

void CBattleSystem::Free()
{
	__super::Free();

}
