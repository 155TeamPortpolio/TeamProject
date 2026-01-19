#include "pch.h"
#include "CamObject.h"
#include "Light.h"
#include "GameInstance.h"

HRESULT CCamObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CCamera>();
	Add_Component<CLight>();
	return S_OK;
}

HRESULT CCamObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	LIGHT_DESC desc{};
	desc.vLightPosition = {};
	desc.fLightRange = {};
	desc.fLightIntensity = 10.f;
	desc.vLightDirection = {0.0f, -1.0f, 1.0f, 0.0f};
	desc.vLightDiffuse = {1.0f, 1.0f, 1.0f, 1.0f};
	desc.vLightAmbient = {0.6f, 0.6f, 0.6f, 1.0f};
	desc.vLightSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
	Get_Component<CLight>()->Set_Desc(desc, LIGHT_TYPE::DIRECTIONAL);

	return S_OK;
}