#include "pch.h"
#include "DemoCamera.h"
#include "Camera.h"

#include "GameInstance.h"
#include "IRenderService.h"
#include "IInputService.h"

#include "Light.h"

CDemoCamera::CDemoCamera()
{
}

CDemoCamera::CDemoCamera(const CDemoCamera& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDemoCamera::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CCamera>();
	Add_Component<CLight>();
	return S_OK;
}

HRESULT CDemoCamera::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	m_pTransform->LookAt({ 0,0,0 }); 
	LIGHT_DESC desc = {};
	desc.vLightPosition = { 0,20,0,0 };
	desc.fLightRange = 80.0f;
	desc.vLightDirection = _float4(0.f, -1.f, 1.f, 0.f);
	desc.vLightDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	desc.vLightAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	desc.vLightSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	Get_Component<CLight>()->Set_Desc(desc, LIGHT_TYPE::DIRECTIONAL);
	return S_OK;
}

void CDemoCamera::Priority_Update(_float dt)
{
	_vector LookDir = m_pTransform->Dir(STATE::LOOK);
	_vector UpDir = m_pTransform->Dir(STATE::UP);
	_vector RightDir = m_pTransform->Dir(STATE::RIGHT);
	_vector WorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector WorldRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vector WorldLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('W'))
		m_pTransform->Translate(LookDir * dt * m_fSpeed);

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('S'))
		m_pTransform->Translate(LookDir * dt * -m_fSpeed);

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('D'))
		m_pTransform->Translate(RightDir * dt * m_fSpeed);

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('A'))
		m_pTransform->Translate(RightDir * dt * -m_fSpeed);

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_SPACE))
		m_pTransform->Translate(WorldUp * dt * m_fSpeed);

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_CONTROL))
		m_pTransform->Translate(WorldUp * dt * -m_fSpeed);

	_float Yaw = {}; //Y축 회전
	_float Pitch = {};//X축 회전

	if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Down(MOUSE_BTN::RB)) {
		Yaw = CGameInstance::GetInstance()->Get_InputDev()->Mouse_DeltaX() * dt * m_fMouseSensitive;
		Pitch = CGameInstance::GetInstance()->Get_InputDev()->Mouse_DeltaY() * dt * m_fMouseSensitive;
	}

	_vector TargetRotate = { Yaw ,Pitch };
	_vector CurRotate = XMLoadFloat2(&m_vCurrentRotate);

	_vector LerpedLookAt = XMVectorLerp(CurRotate, TargetRotate, 10 * dt);
	XMStoreFloat2(&m_vCurrentRotate, LerpedLookAt);

	m_vCurrentRotate.x = max(min(m_vCurrentRotate.x, 89.9f), -89.f);

	if (abs(m_vCurrentRotate.x) > 0.003f) //Y축 회전 -> 기준 벡터는 로컬 업
		m_pTransform->Rotation({ 0, XMConvertToRadians(-m_vCurrentRotate.x),0,0 }); //-> 내 기준 업벡

	if (abs(m_vCurrentRotate.y) > 0.003f)//X축 회전 -> 기준 벡터는 월드 라이트
		m_pTransform->Rotation(WorldRight, XMConvertToRadians(m_vCurrentRotate.y));
}

void CDemoCamera::Update(_float dt)
{
}

void CDemoCamera::Late_Update(_float dt)
{
}

void CDemoCamera::Render_GUI()
{
	__super::Render_GUI();
	ImGui::DragFloat("MoveSpeed", &m_fSpeed);
}

CDemoCamera* CDemoCamera::Create()
{
	CDemoCamera* instance = new CDemoCamera();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoCamera");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDemoCamera::Clone(INIT_DESC* pArg)
{
	CDemoCamera* instance = new CDemoCamera(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoCamera");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoCamera::Free()
{
	__super::Free();
}