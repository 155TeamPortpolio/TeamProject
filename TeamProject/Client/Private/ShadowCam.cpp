#include "pch.h"
#include "ShadowCam.h"
#include "GameInstance.h"

#include "EventListener.h"
#include "Light.h"

#include "FieldSystem.h"

HRESULT CShadowCam::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CLight>();
	Add_Component<CEventListener>();
	return S_OK;
}

HRESULT CShadowCam::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);   
	Get_Component<CEventListener>()->Add_Listener<DAYPHASE_DESC>([&](const DAYPHASE_DESC& desc)
		{
			Update_LightByTime(desc);
		});
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
	desc.vLightPosition  = {};
	desc.fLightRange     = {};
	desc.fLightIntensity = 1.f;
	XMStoreFloat4(&desc.vLightDirection, LightDir);
	desc.vLightDiffuse   = { 1.0f,  1.0f, 1.0f, 1.0f };
	desc.vLightAmbient   = { 0.6f,  0.6f, 0.6f, 1.0f };
	desc.vLightSpecular  = { 1.0f,  1.0f, 1.0f, 1.0f };
	Get_Component<CLight>()->Set_Desc(desc, LIGHT_TYPE::DIRECTIONAL);
}

void CShadowCam::Update(_float dt)
{
	auto transform = Get_Component<CTransform>();
	transform->LookAt(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	_vector LightDir = transform->Dir(STATE::LOOK);
	auto Desc = Get_Component<CLight>()->SnapShot_Desc();
	XMStoreFloat4(&Desc.vLightDirection, LightDir);
	Get_Component<CLight>()->Set_Desc(Desc, LIGHT_TYPE::DIRECTIONAL);
	
}

void CShadowCam::Update_LightByTime(DAYPHASE_DESC Desc)
{
	LIGHT_DESC desc{};
	desc.fLightRange = {};
	desc.fLightIntensity = 1.f;
	auto transform = Get_Component<CTransform>();
	switch (Desc.dayPhase)
	{
	case DayPhase::EarlyMorning: 
		transform->Set_Pos(_float4(100.f, 50.f, 0.f, 1.f)); 
		desc.vLightDiffuse = { 1.0f, 0.7f, 0.5f, 1.0f };
		desc.vLightAmbient = { 0.5f, 0.4f, 0.5f, 1.0f };
		desc.vLightSpecular = { 1.0f, 0.8f, 0.6f, 1.0f };
		desc.fLightIntensity = 0.7f;
		break;

	case DayPhase::Morning: 
		transform->Set_Pos(_float4(70.f, 100.f, 0.f, 1.f)); 
		desc.vLightDiffuse = { 1.0f, 0.95f, 0.9f, 1.0f };
		desc.vLightAmbient = { 0.7f, 0.75f, 0.85f, 1.0f };
		desc.vLightSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };
		desc.fLightIntensity = 1.0f;
		break;

	case DayPhase::Afternoon:
		transform->Set_Pos(_float4(-100.f, 50.f, 0.f, 1.f));
		desc.vLightDiffuse = { 1.0f, 0.5f, 0.3f, 1.0f };
		desc.vLightAmbient = { 0.6f, 0.3f, 0.4f, 1.0f };
		desc.vLightSpecular = { 1.0f, 0.6f, 0.4f, 1.0f };
		desc.fLightIntensity = 0.8f;
		break;

	case DayPhase::LateNight: 
		transform->Set_Pos(_float4(0.f, 100.f, 50.f, 1.f ));  
		desc.vLightDiffuse = { 0.4f, 0.5f, 0.7f, 1.0f };
		desc.vLightAmbient = { 0.2f, 0.25f, 0.35f, 1.0f };
		desc.vLightSpecular = { 0.3f, 0.4f, 0.5f, 1.0f };
		desc.fLightIntensity = 0.4f;
		break;
	}

	_vector LightDir = transform->Dir(STATE::LOOK);
	XMStoreFloat4(&desc.vLightDirection, LightDir);

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