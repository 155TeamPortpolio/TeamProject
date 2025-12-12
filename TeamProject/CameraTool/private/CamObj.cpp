#include "pch.h"

HRESULT CamObj::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	cam = Add_Component<CCamera>();
	game = CGameInstance::GetInstance();
	return S_OK;
}

HRESULT CamObj::Initialize(INIT_DESC* arg)
{
	__super::Initialize(arg);

	return S_OK;
}

void CamObj::Free()
{
	__super::Free();
}