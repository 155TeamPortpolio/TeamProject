#include "pch.h"
#include "Light.h"
#include "CamSequencePlayer.h"

HRESULT CamObj::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	cam = Add_Component<CCamera>();
	Add_Component<CLight>();
	Add_Component<CamSequencePlayer>();
	game = CGameInstance::GetInstance();
	return S_OK;
}

HRESULT CamObj::Initialize(INIT_DESC* arg)
{
	__super::Initialize(arg);

	LIGHT_DESC desc      = {};
	desc.vLightPosition  = { 0, 20, 0, 0 };
	desc.fLightRange     = 80.0f;
	desc.vLightDirection = _float4(0.f, -1.f, 1.f, 0.f);
	desc.vLightDiffuse   = _float4(1.f, 1.f, 1.f, 1.f);
	desc.vLightAmbient   = _float4(0.6f, 0.6f, 0.6f, 1.f);
	desc.vLightSpecular  = _float4(1.f, 1.f, 1.f, 1.f);

	Get_Component<CLight>()->Set_Desc(desc, LIGHT_TYPE::DIRECTIONAL);

	return S_OK;
}

void CamObj::Free()
{
	__super::Free();
}