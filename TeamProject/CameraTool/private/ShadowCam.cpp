#include "pch.h"
#include "ShadowCam.h"
#include "GameInstance.h"
#include "Light.h"

HRESULT CShadowCam::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CLight>();
	return S_OK;
}

HRESULT CShadowCam::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void CShadowCam::Awake()
{
	auto camera = Get_Component<CCamera>();
	camera->Set_ProjType(CamProjType::Orthographic);

	auto transform = Get_Component<CTransform>();
	transform->LookAt(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	_vector LightDir = transform->Dir(STATE::LOOK);
	LIGHT_DESC desc{};
	desc.vLightPosition = {};
	desc.fLightRange = {};
	desc.fLightIntensity = 1.f;
	XMStoreFloat4(&desc.vLightDirection, LightDir);
	desc.vLightDiffuse = {1.0f, 1.0f, 1.0f, 1.0f};
	desc.vLightAmbient = {0.6f, 0.6f, 0.6f, 1.0f};
	desc.vLightSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
	Get_Component<CLight>()->Set_Desc(desc, LIGHT_TYPE::DIRECTIONAL);
}

void CShadowCam::Update(_float dt)
{
	auto transform = Get_Component<CTransform>();
	transform->LookAt(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	_vector LightDir = transform->Dir(STATE::LOOK);
	LIGHT_DESC desc{};
	desc.vLightPosition  = {};
	desc.fLightRange     = {};
	desc.fLightIntensity = 1.f;
	XMStoreFloat4(&desc.vLightDirection, LightDir);
	desc.vLightDiffuse  = {1.0f, 1.0f, 1.0f, 1.0f};
	desc.vLightAmbient  = {1.0f, 1.0f, 1.0f, 1.0f};
	desc.vLightSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
	Get_Component<CLight>()->Set_Desc(desc, LIGHT_TYPE::DIRECTIONAL);
}

CShadowCam* CShadowCam::Create()
{
	CShadowCam* Instance = new CShadowCam();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CShadowCam");
		Safe_Release(Instance);
	}
	return Instance;
}

CGameObject* CShadowCam::Clone(INIT_DESC* pArg)
{
	CShadowCam* Instance = new CShadowCam(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CShadowCam");
		Safe_Release(Instance);
	}
	return Instance;
}