#include "pch.h"
#include "Player.h"

CPlayer::CPlayer(const CPlayer& rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CPlayer::Initialize(INIT_DESC* pArg)
{
	return E_NOTIMPL;
}

void CPlayer::Awake()
{
}

void CPlayer::Priority_Update(_float dt)
{
}

void CPlayer::Update(_float dt)
{
}

void CPlayer::Late_Update(_float dt)
{
}

void CPlayer::Render_GUI()
{
}

CPlayer* CPlayer::Create()
{
	return nullptr;
}

CGameObject* CPlayer::Clone(INIT_DESC* pArg)
{
	return nullptr;
}

void CPlayer::Free()
{
}
