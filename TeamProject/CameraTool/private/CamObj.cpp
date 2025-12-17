#include "pch.h"
#include "Light.h"
#include "CamSequencePlayer.h"

HRESULT CCamObj::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	cam = Add_Component<CCamera>();
	light = Add_Component<CLight>();
	Add_Component<CCamSequencePlayer>();
	game = CGameInstance::GetInstance();
	return S_OK;
}

HRESULT CCamObj::Initialize(INIT_DESC* arg)
{
	__super::Initialize(arg);
	LIGHT_DESC desc{};
	desc.vLightPosition  = {};
	desc.fLightRange     = {};
	desc.vLightDirection = { 0.0f, -1.0f, 1.0f, 0.0f };
	desc.vLightDiffuse   = { 1.0f,  1.0f, 1.0f, 1.0f };
	desc.vLightAmbient   = { 0.6f,  0.6f, 0.6f, 1.0f };
	desc.vLightSpecular  = { 1.0f,  1.0f, 1.0f, 1.0f };
	light->Set_Desc(desc, LIGHT_TYPE::DIRECTIONAL);

	return S_OK;
}

void CCamObj::Free()
{
	__super::Free();
}