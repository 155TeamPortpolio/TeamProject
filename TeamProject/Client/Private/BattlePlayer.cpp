#include "pch.h"
#include "BattlePlayer.h"

#include "BattleSystem.h"
#include "DataBase.h"
#include "Character.h"

CBattlePlayer::CBattlePlayer()
{
}

void CBattlePlayer::SetBattleCharacters(vector<_uint> battleCharacters)
{

}

HRESULT CBattlePlayer::Initialize()
{
	CBattleSystem::GetInstance()->SetBattlePlayer(this);
	auto PlayerDesc = CDataBase::GetInstance()->GetPlayerDesc("Miyabi");
	auto LVDesc = CDataBase::GetInstance()->GetLevelDesc(10);
	return S_OK;
}

void CBattlePlayer::Priority_Update(_float dt)
{
}

void CBattlePlayer::Update(_float dt)
{
}

void CBattlePlayer::Late_Update(_float dt)
{
}

CBattlePlayer* CBattlePlayer::Create()
{
	CBattlePlayer* Instance = new CBattlePlayer();
	if (FAILED(Instance->Initialize()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CBattlePlayer::Free()
{
	for (auto& character : m_BattleCharacters)
		Safe_Release(character.second);
	m_BattleCharacters.clear();

	__super::Free();
}
