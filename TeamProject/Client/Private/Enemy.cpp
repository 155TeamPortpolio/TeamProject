#include "pch.h"
#include "Enemy.h"

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

void CEnemy::Free()
{
	__super::Free();
}
