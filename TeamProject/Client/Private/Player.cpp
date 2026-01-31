#include "pch.h"
#include "Player.h"

#include "BattlePlayer.h"
#include "FieldPlayer.h"

#include "GameInstance.h"
#include "BattleSystem.h"

CPlayer::CPlayer(const CPlayer& rhs)
	:CGameObject(rhs)
{
}

OBJECT_HANDLE CPlayer::Get_CurCharacterHandle()
{
	if (m_ePlayerType == PLAYER::BATTLE)
		return m_pBattlePlayer->GetCurCharacterHandle();

	else if (m_ePlayerType == PLAYER::FIELD)
		return m_pFieldPlayer->GetCurCharacterHandle();

	return OBJECT_HANDLE{};
}

HRESULT CPlayer::Clear_Characters()
{
	if (m_ePlayerType == PLAYER::BATTLE)
		m_pBattlePlayer->ClearCharacters();

	else if (m_ePlayerType == PLAYER::FIELD)
		m_pFieldPlayer->Clear_Character();

	m_ePlayerType = PLAYER::END;

	return S_OK;
}

void CPlayer::Set_PlayerType(PLAYER ePlayer)
{
	m_ePlayerType = ePlayer;

	if (m_ePlayerType == PLAYER::BATTLE)
	{
		m_pBattlePlayer->Initialize();
	}
	else
	{
		m_pFieldPlayer->Initialize();
	}
}

HRESULT CPlayer::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CPlayer::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	m_pBattlePlayer = CBattlePlayer::Create();
	m_pFieldPlayer = CFieldPlayer::Create();

	return S_OK;
}

void CPlayer::Awake()
{
	//First Setting Denny
	_uint Denny = 10000;
	RuntimeBucket().Int64.Set(PersistScope::SaveSlot, "Denny", Denny);
}

void CPlayer::Priority_Update(_float dt)
{
	if (m_ePlayerType == PLAYER::END) return;

	if(m_ePlayerType == PLAYER::BATTLE) m_pBattlePlayer->Priority_Update(dt);
	else m_pFieldPlayer->Priority_Update(dt);
}

void CPlayer::Update(_float dt)
{
	if (m_ePlayerType == PLAYER::END) return;

	if (m_ePlayerType == PLAYER::BATTLE) m_pBattlePlayer->Update(dt);
	else m_pFieldPlayer->Update(dt);
}

void CPlayer::Late_Update(_float dt)
{
	if (m_ePlayerType == PLAYER::END) return;

	if (m_ePlayerType == PLAYER::BATTLE) m_pBattlePlayer->Late_Update(dt);
	else m_pFieldPlayer->Late_Update(dt);
}

void CPlayer::Lock_Input()
{
	if (m_ePlayerType == PLAYER::END) return;

	if (m_ePlayerType == PLAYER::BATTLE) 
		m_pBattlePlayer->Lock_Input();

	if (m_ePlayerType == PLAYER::FIELD)
		m_pFieldPlayer->Lock_Input();
}

void CPlayer::Unlock_Input()
{
	if (m_ePlayerType == PLAYER::END) return;

	if (m_ePlayerType == PLAYER::BATTLE)
		m_pBattlePlayer->UnLock_Input();

	if (m_ePlayerType == PLAYER::FIELD)
		m_pFieldPlayer->UnLock_Input();
}

void CPlayer::Render_GUI()
{
	if (m_pBattlePlayer)
	{
		m_pBattlePlayer->Render_GUI();
	}
}

CPlayer* CPlayer::Create()
{
	CPlayer* Instance = new CPlayer();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CPlayer::Clone(INIT_DESC* pArg)
{
	CPlayer* Instance = new CPlayer(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CPlayer::Free()
{
	Safe_Release(m_pBattlePlayer);
	Safe_Release(m_pFieldPlayer);
	__super::Free();
}
