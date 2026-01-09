#include "pch.h"
#include "BattleSystem.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "CharacterController.h"
#include "BattlePlayer.h"
#include "DataBase.h"

IMPLEMENT_SINGLETON(CBattleSystem)

CBattleSystem::CBattleSystem()
{
	// ºó °ª Ã¤¿ì±â
	for (_int i = 0; i < static_cast<_int>(BATTLE_OBJ_TYPE::END); ++i) {
		auto eType = static_cast<BATTLE_OBJ_TYPE>(i);
		m_BattleObjInfos.emplace(eType, vector<BATTLEOBJ_INFO>{});
	}
}

void CBattleSystem::Update()
{
	if (false == m_isActive)
		return;

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

const vector<BATTLEOBJ_INFO>& CBattleSystem::GetBattleObjects(BATTLE_OBJ_TYPE eType) const
{
	return m_BattleObjInfos.at(eType);
}

vector<BATTLEOBJ_INFO> CBattleSystem::CopyBattleObjects(BATTLE_OBJ_TYPE eType)
{
	return m_BattleObjInfos[eType];
}

void CBattleSystem::SpawnMosnter(const string& MonsterProtoTag, _float3 vSpawnPos)
{
	MonsterCreationDesc MonsterTableDesc = CDataBase::GetInstance()->GetMonsterDesc(MonsterProtoTag);
	if (true == MonsterTableDesc.ProtoTag.empty())
		return;

	CCT_DESC MonsterCCT;
	MonsterCCT.eGroup = COLLISION_GROUP::MONSTER;
	MonsterCCT.iCollisionMask = 0xFFFFFFFF;
	MonsterCCT.bAutoFit = false;
	MonsterCCT.fHeight = MonsterTableDesc.CCT_fHeight;
	MonsterCCT.fRadius = MonsterTableDesc.CCT_fRadius;
	MonsterCCT.vPos = vSpawnPos;
	MonsterCCT.vPos.y += MonsterCCT.fHeight;
	
	const string NowLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

	auto pMonster = Builder::Create_Object({ NowLevel,MonsterTableDesc.ProtoTag })
		.CharacterController(MonsterCCT)
		.Build(MonsterTableDesc.DisplayName);

	if (nullptr == pMonster)
		return;
	
	CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pMonster, { NowLevel, "Enemy_Layer"});

	m_Handles[BATTLE_OBJ_TYPE::MONSTER].push_back(pMonster->Get_Handle());
}

void CBattleSystem::SetPlayer(OBJECT_HANDLE hPlayer)
{
	if (hPlayer.isValid())
		m_Handles[BATTLE_OBJ_TYPE::PLAYER].push_back(hPlayer);
}

void CBattleSystem::SetBattleCharacters(vector<_uint> battleCharacters)
{
	m_pBattlePlayer->SetBattleCharacters(battleCharacters);
}

void CBattleSystem::ClearBattleStage()
{
	for (auto& Pair : m_Handles) 
		Pair.second.clear();

	for (auto& Pair : m_BattleObjInfos)
		Pair.second.clear();
}

void CBattleSystem::Free()
{
	__super::Free();

}
