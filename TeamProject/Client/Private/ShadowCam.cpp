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
    transform->Set_Pos(_float4(100.f, 50.f, 0.f, 1.f));
	transform->LookAt(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	_vector LightDir = transform->Dir(STATE::LOOK);
	LIGHT_DESC desc{};
	desc.vLightPosition  = {};
	desc.fLightRange     = {};
	desc.fLightIntensity = 0.7f;
	XMStoreFloat4(&desc.vLightDirection, LightDir);
	desc.vLightDiffuse   = { 1.0f, 0.7f, 0.5f, 1.0f };
	desc.vLightAmbient   = { 0.5f, 0.4f, 0.5f, 1.0f };
	desc.vLightSpecular  = { 1.0f, 0.8f, 0.6f, 1.0f };
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

	Update_LightTransition(dt);
}

void CShadowCam::Update_LightByTime(DAYPHASE_DESC Desc)
{
    if (!m_bIsTransition)
    {
        auto currentDesc = Get_Component<CLight>()->SnapShot_Desc();
        auto transform = Get_Component<CTransform>();

        m_StartLight.Position = transform->Get_Pos();
        m_StartLight.Diffuse = currentDesc.vLightDiffuse;
        m_StartLight.Ambient = currentDesc.vLightAmbient;
        m_StartLight.Specular = currentDesc.vLightSpecular;
        m_StartLight.Intensity = currentDesc.fLightIntensity;
    }

    m_bIsTransition = true;
    m_fTransitionTime = 0.f;

    switch (Desc.dayPhase)
    {
    case DayPhase::EarlyMorning:
        m_TargetLight.Position = _float4(100.f, 50.f, 0.f, 1.f);
        m_TargetLight.Diffuse = { 1.0f, 0.7f, 0.5f, 1.0f };
        m_TargetLight.Ambient = { 0.5f, 0.4f, 0.5f, 1.0f };
        m_TargetLight.Specular = { 1.0f, 0.8f, 0.6f, 1.0f };
        m_TargetLight.Intensity = 0.7f;
        break;
    case DayPhase::Morning:
        m_TargetLight.Position = _float4(70.f, 100.f, 0.f, 1.f);
        m_TargetLight.Diffuse = { 1.0f, 0.95f, 0.9f, 1.0f };
        m_TargetLight.Ambient = { 0.7f, 0.75f, 0.85f, 1.0f };
        m_TargetLight.Specular = { 1.0f, 1.0f, 1.0f, 1.0f };
        m_TargetLight.Intensity = 1.0f;
        break;
    case DayPhase::Afternoon:
        m_TargetLight.Position = _float4(-100.f, 50.f, 0.f, 1.f);
        m_TargetLight.Diffuse = { 1.0f, 0.5f, 0.3f, 1.0f };
        m_TargetLight.Ambient = { 0.6f, 0.3f, 0.4f, 1.0f };
        m_TargetLight.Specular = { 1.0f, 0.6f, 0.4f, 1.0f };
        m_TargetLight.Intensity = 0.8f;
        break;
    case DayPhase::LateNight:
        m_TargetLight.Position = _float4(0.f, 100.f, 50.f, 1.f);
        m_TargetLight.Diffuse = { 0.4f, 0.5f, 0.7f, 1.0f };
        m_TargetLight.Ambient = { 0.2f, 0.25f, 0.35f, 1.0f };
        m_TargetLight.Specular = { 0.3f, 0.4f, 0.5f, 1.0f };
        m_TargetLight.Intensity = 0.4f;
        break;
    }
}

void CShadowCam::Update_LightTransition(_float dt)
{
    if (!m_bIsTransition) return;

    m_fTransitionTime += dt;
    _float lightTime = min(m_fTransitionTime / m_fTransitionDuration, 1.0f);
    _float posTime = min(m_fTransitionTime / (m_fTransitionDuration * 2.0f), 1.0f);

    _float4 currentPos;
    XMStoreFloat4(&currentPos, XMVectorLerp(
        XMLoadFloat4(&m_StartLight.Position),
        XMLoadFloat4(&m_TargetLight.Position),
        posTime
    ));
    Get_Component<CTransform>()->Set_Pos(currentPos);

    LIGHT_DESC desc{};
    desc.fLightRange = {};
    XMStoreFloat4(&desc.vLightDiffuse, XMVectorLerp(
        XMLoadFloat4(&m_StartLight.Diffuse),
        XMLoadFloat4(&m_TargetLight.Diffuse),
        lightTime
    ));
    XMStoreFloat4(&desc.vLightAmbient, XMVectorLerp(
        XMLoadFloat4(&m_StartLight.Ambient),
        XMLoadFloat4(&m_TargetLight.Ambient),
        lightTime
    ));
    XMStoreFloat4(&desc.vLightSpecular, XMVectorLerp(
        XMLoadFloat4(&m_StartLight.Specular),
        XMLoadFloat4(&m_TargetLight.Specular),
        lightTime
    ));
    desc.fLightIntensity = m_StartLight.Intensity +
        (m_TargetLight.Intensity - m_StartLight.Intensity) * lightTime;

    _vector LightDir = Get_Component<CTransform>()->Dir(STATE::LOOK);
    XMStoreFloat4(&desc.vLightDirection, LightDir);

    Get_Component<CLight>()->Set_Desc(desc, LIGHT_TYPE::DIRECTIONAL);

    if (posTime >= 1.0f && lightTime >= 1.0f)
    {
        m_bIsTransition = false;
    }
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