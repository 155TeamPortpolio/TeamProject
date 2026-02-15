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
#include"BattleSystem_Panel.h"
#include "AudioSource.h"

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
	m_BattleSnapShots = m_BattleObjInfos;
	
	m_pFXFlow = CBattleFXFlow::Create();
	m_pFXFlow->Initialize_Preset();
	m_pFXFlow->SetLayerTag(BATTLE_OBJ_TYPE::PLAYER,		"Model_Layer");
	m_pFXFlow->SetLayerTag(BATTLE_OBJ_TYPE::MONSTER,	"Enemy_Layer");
	m_pFXFlow->SetLayerTag(BATTLE_OBJ_TYPE::CAMERA,		"Camera_Layer");
	m_pFXFlow->SetLayerTag(BATTLE_OBJ_TYPE::EFFECT,		"Effect_Layer");

#ifdef _USING_GUI
	auto panel=CBattleSystem_Panel::Create(GUISystem()->Get_Context());
	GUISystem()->Register_Panel(panel);
#endif
}

void CBattleSystem::Update()
{
	if (false == m_isActive)
		return;
	_uint frame = GameInstance()->Get_FrameCount();
	if (m_LastFrame == frame)
		return;
	m_LastFrame = frame;

	const _float dt = TimeManager()->Get_RawDeltaTime(G_EngineTimerID);

	CheckVFX(dt);
	Update_BattleInfo();
	CleanUp_Data();

	if(InputDevice()->Key_Tap(VK_SHIFT))
	{
		StartGimmick(BATTLE_VFX_TYPE::WIPEOUT);
	}
	if(InputDevice()->Key_Tap(VK_CONTROL))
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
	return m_BattleSnapShots.at(eType);
}

vector<BATTLEOBJ_INFO> CBattleSystem::CopyBattleObjects(BATTLE_OBJ_TYPE eType)
{
	return m_BattleSnapShots[eType];
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
		//ClearBattleStage();
		m_pFXFlow->Clear();
		return;
	}
	else {
		m_isActive = true;
	}
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
	EnterBattleObject(BATTLE_OBJ_TYPE::MONSTER, pMonster->Get_Handle());
}

void CBattleSystem::SetPlayer(vector<OBJECT_HANDLE> hPlayers)
{
	for (auto& hPlayer : hPlayers)
	{
		if (!hPlayer.isValid()) continue;

		BATTLEOBJ_INFO playerInfo = {};
		playerInfo.hObject = hPlayer;
		playerInfo.isOnField = true;
		m_BattleObjInfos[BATTLE_OBJ_TYPE::PLAYER].push_back(playerInfo);
	}
}

void CBattleSystem::TakeAreaDamage(const _float3& vCenter, _float fRadius, const HitDesc& hitDesc)
{
	_float fRadiusSq = fRadius * fRadius;

	for (auto& info : m_BattleSnapShots[BATTLE_OBJ_TYPE::MONSTER])
	{
		_float3 vDiff = info.vPos - vCenter;
		_float fDistSq = vDiff.x * vDiff.x + vDiff.y * vDiff.y + vDiff.z * vDiff.z;

		if (fDistSq > fRadiusSq)
			continue;

		auto pEnemy = dynamic_cast<CEnemy*>(info.hObject.Get());
		if (pEnemy)
		{
			pEnemy->TakeDamage(hitDesc.eDamageType, hitDesc.fDamage, hitDesc.eName);
			m_pBattlePlayer->Add_Gauge(hitDesc.fEnergyCharge, hitDesc.fDecibelCharge);
			auto pCharacter = m_pBattlePlayer->GetCurCharacterHandle().GetAs<CCharacter>();
			pCharacter->OnDamage();
			if (hitDesc.eDamageType == DAMAGE_TYPE::HARD && pEnemy->IsGroggy() && pEnemy->Get_ComboCount() != 0)
			{
				if (!pCharacter->Is_ReserveCombo())
				{
					pEnemy->Decrease_ComboCount();
					pCharacter->Reserve_ComboAttack();
				}
			}
		}
	}
	if (!m_BattleSnapShots[BATTLE_OBJ_TYPE::MONSTER].empty())
		HitVFX(hitDesc.eDamageType);
}

void CBattleSystem::TakeAreaDamage(const _float3& vCenter, _float fRadius, const _float3& vDir, _float fAngle, const HitDesc& hitDesc)
{
	_float fRadiusSq = fRadius * fRadius;
	_float fCosHalfAngle = cosf(XMConvertToRadians(fAngle * 0.5f));
	_vector3 vDirNorm = vDir;
	vDirNorm.Normalize();

	for (auto& info : m_BattleSnapShots[BATTLE_OBJ_TYPE::MONSTER])
	{
		_vector3 vDiff = info.vPos - vCenter;
		_float fDistSq = vDiff.x * vDiff.x + vDiff.y * vDiff.y + vDiff.z * vDiff.z;
		if (fDistSq > fRadiusSq)
			continue;

		_vector3 vToTarget = vDiff;
		vToTarget.y = 0.f;
		vToTarget.Normalize();

		_vector3 vDirFlat = vDirNorm;
		vDirFlat.y = 0.f;
		vDirFlat.Normalize();

		if (vToTarget.Dot(vDirFlat) < fCosHalfAngle)
			continue;

		auto pEnemy = dynamic_cast<CEnemy*>(info.hObject.Get());
		if (pEnemy)
		{
			pEnemy->TakeDamage(hitDesc.eDamageType, hitDesc.fDamage, hitDesc.eName);
			m_pBattlePlayer->Add_Gauge(hitDesc.fEnergyCharge, hitDesc.fDecibelCharge);
			auto pCharacter = m_pBattlePlayer->GetCurCharacterHandle().GetAs<CCharacter>();
			pCharacter->OnDamage();
			if (hitDesc.eDamageType == DAMAGE_TYPE::HARD && pEnemy->IsGroggy() && pEnemy->Get_ComboCount() != 0)
			{
				if (!pCharacter->Is_ReserveCombo())
				{
					pEnemy->Decrease_ComboCount();
					pCharacter->Reserve_ComboAttack();
				}
			}
		}
	}
	if (!m_BattleSnapShots[BATTLE_OBJ_TYPE::MONSTER].empty())
		HitVFX(hitDesc.eDamageType);
}

void CBattleSystem::TakeBoxDamage(const _float3& vCenter, const _float3& vHalfExtents, const _quaternion& qRotation, const HitDesc& hitDesc)
{
	_quaternion qInverse;
	qRotation.Inverse(qInverse);

	for (auto& info : m_BattleSnapShots[BATTLE_OBJ_TYPE::MONSTER])
	{
		_vector3 vLocal = _vector3::Transform(info.vPos - vCenter, qInverse);

		if (fabs(vLocal.x) > vHalfExtents.x ||
			fabs(vLocal.y) > vHalfExtents.y ||
			fabs(vLocal.z) > vHalfExtents.z)
			continue;

		auto pEnemy = dynamic_cast<CEnemy*>(info.hObject.Get());
		if (pEnemy)
		{
			pEnemy->TakeDamage(hitDesc.eDamageType, hitDesc.fDamage, hitDesc.eName);
			m_pBattlePlayer->Add_Gauge(hitDesc.fEnergyCharge, hitDesc.fDecibelCharge);
			auto pCharacter = m_pBattlePlayer->GetCurCharacterHandle().GetAs<CCharacter>();
			pCharacter->OnDamage();
			if (hitDesc.eDamageType == DAMAGE_TYPE::HARD && pEnemy->IsGroggy() && pEnemy->Get_ComboCount() != 0)
			{
				if (!pCharacter->Is_ReserveCombo())
				{
					pEnemy->Decrease_ComboCount();
					pCharacter->Reserve_ComboAttack();
				}
			}
		}
	}
	if (!m_BattleSnapShots[BATTLE_OBJ_TYPE::MONSTER].empty())
		HitVFX(hitDesc.eDamageType);
}

void CBattleSystem::TakeAllDamage(const HitDesc& hitDesc)
{
	for (auto& info : m_BattleSnapShots[BATTLE_OBJ_TYPE::MONSTER])
	{
		auto& handle = info.hObject;

		if (!handle.isValid())
			continue;
		auto pEnemy = dynamic_cast<CEnemy*>(handle.Get());
		if (pEnemy)
		{
			pEnemy->TakeDamage(hitDesc.eDamageType, hitDesc.fDamage, hitDesc.eName);
			m_pBattlePlayer->Add_Gauge(hitDesc.fEnergyCharge, hitDesc.fDecibelCharge);
			auto pCharacter = m_pBattlePlayer->GetCurCharacterHandle().GetAs<CCharacter>();
			pCharacter->OnDamage();
			if (hitDesc.eDamageType == DAMAGE_TYPE::HARD && pEnemy->IsGroggy() && pEnemy->Get_ComboCount() != 0)
			{
				if (!pCharacter->Is_ReserveCombo())
				{
					pEnemy->Decrease_ComboCount();
					pCharacter->Reserve_ComboAttack();
				}
			}
		}
	}
	if(!m_BattleSnapShots[BATTLE_OBJ_TYPE::MONSTER].empty())
		HitVFX(hitDesc.eDamageType);
}

void CBattleSystem::CleanUp_Data()
{
	m_BattleSnapShots.clear();
	for (size_t i = 0; i < ENUM(BATTLE_OBJ_TYPE::END); i++)
	{
		BATTLE_OBJ_TYPE eType = BATTLE_OBJ_TYPE(i);
		auto& vector = m_BattleObjInfos[eType];
		auto& snapVector = m_BattleSnapShots[eType];

		for (size_t i = 0; i < vector.size(); i++)
		{
			if (!vector[i].isOnField) continue;
			if (!vector[i].hObject.isValid()) continue;
			snapVector.push_back(vector[i]);
		}
	}
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

void CBattleSystem::EnterBattleObject(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject)
{
	auto BattleInfo = FindBattleObjInfo(hObject);

	if (nullptr != BattleInfo) {
		BattleInfo->isOnField = true;
		return;
	}

	auto& vector = FindBattleType(eObjType);

	BattleObjectInfo info = {};
	info.hObject = hObject;
	info.isOnField = true;

	vector.push_back(info);
	m_BattleObjIndex[hObject] = { eObjType, static_cast<_uint>(vector.size() - 1) };
}

_bool CBattleSystem::ExitBattleObject(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject)
{
	auto indexIter = m_BattleObjIndex.find(hObject);
	if (indexIter == m_BattleObjIndex.end())
		return false;
	auto& indexList = FindBattleType(eObjType);
	_uint removeIndex= indexIter->second.indexInVector;

	RemoveFromListSwapPop(indexList, removeIndex, eObjType);
	return true;
}

void CBattleSystem::ExcludeBattleObject(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject)
{
	auto indexIter = m_BattleObjIndex.find(hObject);
	if (indexIter == m_BattleObjIndex.end())
		return;
	auto& indexList = FindBattleType(eObjType);
	indexList[indexIter->second.indexInVector].isOnField = false;
}

_bool CBattleSystem::isValidTarget(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject)
{
	auto info = FindBattleObjInfo(hObject);
	if (!info)
		return false;

	return info->isOnField;
}

void CBattleSystem::ClearBattleStage()
{
	for (auto& Pair : m_BattleObjInfos) {
		if (Pair.first == BATTLE_OBJ_TYPE::PLAYER)
			continue;
		for (auto& info : Pair.second)
		{
			info.Reset();
		}
		Pair.second.clear();
	}
	m_BattleObjIndex.clear();
	m_BattleSnapShots.clear();

	m_BattleSnapShots = m_BattleObjInfos;
}

void CBattleSystem::AllKill()
{
	HitDesc AllKill{};

	AllKill.fDamage = 10000.f;
	AllKill.eDamageType = DAMAGE_TYPE::NORMAL;

	TakeAllDamage(AllKill);
}

BATTLEOBJ_INFO* CBattleSystem::FindBattleObjInfo(OBJECT_HANDLE objectHandle)
{
	auto it = m_BattleObjIndex.find(objectHandle);
	if (it == m_BattleObjIndex.end())
		return nullptr;

	const BattleObjIndex& indexInfo = it->second;

	TypeVector& infoList = m_BattleObjInfos[indexInfo.objType];

	if (indexInfo.indexInVector >= infoList.size())
		return nullptr;
	
	return &infoList[indexInfo.indexInVector];
}

vector<BATTLEOBJ_INFO>& CBattleSystem::FindBattleType(BATTLE_OBJ_TYPE eObjType)
{
	return m_BattleObjInfos[eObjType];
}

_bool CBattleSystem::RemoveFromListSwapPop(TypeVector& infoList, _uint removeIndex, BATTLE_OBJ_TYPE objType)
{
	const _uint lastIndex = (_uint)infoList.size() - 1;
	if (removeIndex != lastIndex)
	{
		infoList[removeIndex] = infoList[lastIndex]; /*마지막 인덱스 애를 리무브 인덱스로 옮겨줌*/

		OBJECT_HANDLE movedHandle = infoList[removeIndex].hObject; 
		auto itMoved = m_BattleObjIndex.find(movedHandle);
		if (itMoved != m_BattleObjIndex.end())
		{	/*인덱스 관련 정보도 바꿔줌*/
			itMoved->second.objType = objType;
			itMoved->second.indexInVector = removeIndex;
		}
	}

	infoList.pop_back(); /*마지막 정보 삭제*/
	return true;
}

void CBattleSystem::StartGimmick(BATTLE_VFX_TYPE eVFXType)
{
	m_pFXFlow->StartVfx(eVFXType);
}
void CBattleSystem::HitVFX(DAMAGE_TYPE eDamageType)
{
	switch (eDamageType)
	{
	case Client::DAMAGE_TYPE::NORMAL:
		m_pFXFlow->StartVfx(BATTLE_VFX_TYPE::HIT_NORMAL);
		break;
	case Client::DAMAGE_TYPE::HARD:
		m_pFXFlow->StartVfx(BATTLE_VFX_TYPE::HIT_HARD);
		break;
	case Client::DAMAGE_TYPE::AIRBORNE:
		break;
	case Client::DAMAGE_TYPE::ULTIMATE:
		break;
	default:
		break;
	}
}

void CBattleSystem::Free()
{
	__super::Free();
	Safe_Release(m_pFXFlow);
}
