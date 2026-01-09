#include "pch.h"
#include "Player.h"

#include "BattlePlayer.h"

#include "GameInstance.h"
#include "BattleSystem.h"

CPlayer::CPlayer(const CPlayer& rhs)
	:CGameObject(rhs)
{
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

	return S_OK;
}

void CPlayer::Awake()
{
}

void CPlayer::Priority_Update(_float dt)
{
	m_pBattlePlayer->Priority_Update(dt);
}

void CPlayer::Update(_float dt)
{
	m_pBattlePlayer->Update(dt);
}

void CPlayer::Late_Update(_float dt)
{
	m_pBattlePlayer->Late_Update(dt);
}

void CPlayer::Render_GUI()
{
}

void CPlayer::Update_KeyInput()
{
	InputDevice()->
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
	__super::Free();
}
