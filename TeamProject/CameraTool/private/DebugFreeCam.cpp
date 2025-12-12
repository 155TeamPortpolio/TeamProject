#include "pch.h"
#include "DebugFreeCam.h"

HRESULT DebugFreeCam::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	camType = CamType::Debug;
	rigType = CamRigType::Free;
	return S_OK;
}

HRESULT DebugFreeCam::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	transform->LookAt({ 0.f, 0.f, 0.f });
	return S_OK;
}

void DebugFreeCam::Priority_Update(_float dt)
{
	auto input = game->Get_InputDev();

	_vector4 look  = transform->Dir(STATE::LOOK);
	_vector4 right = transform->Dir(STATE::RIGHT);
	_vector4 worldUp{ 0.f, 1.f, 0.f, 0.f };

	_float speed = moveSpeed * dt;

	if (input->Key_Down('W'))     
		transform->Translate(look * speed);

	if (input->Key_Down('S'))      
		transform->Translate(look * -speed);

	if (input->Key_Down('D'))     
		transform->Translate(right * speed);

	if (input->Key_Down('A'))     
		transform->Translate(right * -speed);

	if (input->Mouse_Down(MOUSE_BTN::RB))
	{
		const float deltaX = input->Mouse_DeltaX();
		const float deltaY = input->Mouse_DeltaY();

		rotDeg.x += deltaX * sensitivity;
		rotDeg.y += deltaY * sensitivity;
		rotDeg.y = clamp(rotDeg.y, -89.f, 89.f);
	}
	ApplyRotation();
}

void DebugFreeCam::Update(_float dt)
{
}

void DebugFreeCam::Late_Update(_float dt)
{
}

void DebugFreeCam::ApplyRotation()
{
	const _float yawRad   = XMConvertToRadians(rotDeg.x);
	const _float pitchRad = XMConvertToRadians(rotDeg.y);

	_vector3 forward = { cosf(pitchRad) * sinf(yawRad), -sinf(pitchRad), cosf(pitchRad) * cosf(yawRad) };
	_vector4 pos = transform->Get_Pos(); 
	_vector3 target =
	{
		pos.x + forward.x,
		pos.y + forward.y,
		pos.z + forward.z
	};
	transform->LookAt(target);
}

DebugFreeCam* DebugFreeCam::Create()
{
	auto inst = new DebugFreeCam();
	if (FAILED(inst->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDebugFreeCam");
		Safe_Release(inst);
	}
	return inst;
}

CGameObject* DebugFreeCam::Clone(INIT_DESC* pArg)
{
	auto inst = new DebugFreeCam(*this);
	if (FAILED(inst->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDebugFreeCam");
		Safe_Release(inst);
	}
	return inst;
}

void DebugFreeCam::Free()
{
	__super::Free();
}
