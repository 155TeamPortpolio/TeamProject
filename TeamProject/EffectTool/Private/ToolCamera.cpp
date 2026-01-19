#include "pch.h"
#include "ToolCamera.h"
#include "Camera.h"

#include "GameInstance.h"
#include "IRenderService.h"
#include "IInputService.h"

#include "Light.h"

CToolCamera::CToolCamera()
{
}

CToolCamera::CToolCamera(const CToolCamera& rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolCamera::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CCamera>();
	Add_Component<CLight>();
	return S_OK;
}

HRESULT CToolCamera::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	m_pTransform->LookAt({ 0,0,0 });
	return S_OK;
}

void CToolCamera::Priority_Update(_float dt)
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

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_SHIFT))
		m_pTransform->Translate(WorldUp * dt * -m_fSpeed);

	_float Yaw = {}; //Y축 회전
	_float Pitch = {};//X축 회전

	if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Down(MOUSE_BTN::RB)) {
		Yaw = CGameInstance::GetInstance()->Get_InputDev()->Mouse_DeltaX() * dt * m_fMouseSensitive * -1.f;
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

void CToolCamera::Update(_float dt)
{
}

void CToolCamera::Late_Update(_float dt)
{
}

CToolCamera* CToolCamera::Create()
{
	CToolCamera* instance = new CToolCamera();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CToolCamera");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CToolCamera::Clone(INIT_DESC* pArg)
{
	CToolCamera* instance = new CToolCamera(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CToolCamera");
		Safe_Release(instance);
	}

	return instance;
}

void CToolCamera::Free()
{
	__super::Free();
}