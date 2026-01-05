#include "pch.h"
#include "Enemy.h"
#include "BattleSystem.h"

CEnemy::CEnemy()
	:CGameObject()
{
}

CEnemy::CEnemy(const CEnemy& rhg)
	:CGameObject(rhg)
{
}

HRESULT CEnemy::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CEnemy::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CEnemy::Update(_float dt)
{
	m_PlayerCharacterInfos.clear();
	m_PlayerCharacterInfos = CBattleSystem::GetInstance()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::PLAYER);
}

BATTLEOBJ_INFO* CEnemy::GetCharacterOnField()
{
	// 추후에 캐릭터 여러명 나올 때 로직 나왔을 때 변경 예정
	for (auto& info : m_PlayerCharacterInfos) {
		if (true == info.isOnField)
			return &info;
	}
	return nullptr;
}

void CEnemy::Free()
{
	__super::Free();
}
