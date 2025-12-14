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

	_vector4 look4 = transform->Dir(STATE::LOOK);
	_vector3 forward{ look4.x, look4.y, look4.z };

	if (forward.LengthSquared() <= 1e-8f)
		forward = _vector3{ 0.f, 0.f, 1.f };
	else
		forward.Normalize();

	const float yawRad = atan2f(forward.x, forward.z);
	const float pitchRad = asinf(-forward.y);

	rotDegTarget.x = XMConvertToDegrees(yawRad);
	rotDegTarget.y = XMConvertToDegrees(pitchRad);
	rotDegTarget.y = clamp(rotDegTarget.y, -89.f, 89.f);

	rotQuatTarget = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);
	rotQuatCurrent = rotQuatTarget;

	return S_OK;
}

void DebugFreeCam::Priority_Update(_float dt)
{
	auto input = game->Get_InputDev();

	if (input->Mouse_Down(MOUSE_BTN::RB))
	{
		const float deltaX = input->Mouse_DeltaX();
		const float deltaY = input->Mouse_DeltaY();

		rotDegTarget.x += deltaX * sensitivity;
		rotDegTarget.y += deltaY * sensitivity;
		rotDegTarget.y = clamp(rotDegTarget.y, -89.f, 89.f);
	}

	ApplyRotation(dt);

	_vector4 look = transform->Dir(STATE::LOOK);
	_vector4 right = transform->Dir(STATE::RIGHT);

	const _float speed = moveSpeed * dt;

	if (input->Key_Down('W')) transform->Translate(look * speed);
	if (input->Key_Down('S')) transform->Translate(look * -speed);
	if (input->Key_Down('D')) transform->Translate(right * speed);
	if (input->Key_Down('A')) transform->Translate(right * -speed);
}

void DebugFreeCam::Update(_float dt)
{
}

void DebugFreeCam::Late_Update(_float dt)
{
}

void DebugFreeCam::Render_GUI()
{
	__super::Render_GUI();

	if (ImGui::CollapsingHeader(u8"DebugFreeCam", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushID("DebugFreeCam_RenderGUI");

		ImGui::DragFloat(u8"이동 속도", &moveSpeed, 0.1f, 0.f, 5000.f);
		ImGui::DragFloat(u8"마우스 감도", &sensitivity, 0.001f, 0.f, 5.f);
		ImGui::DragFloat(u8"회전 스무딩 속도", &rotSmoothSpeed, 0.1f, 0.f, 60.f);

		ImGui::Separator();

		ImGui::DragFloat2(u8"회전 목표(도)", &rotDegTarget.x, 0.05f);
		rotDegTarget.y = clamp(rotDegTarget.y, -89.f, 89.f);

		if (ImGui::Button(u8"회전 목표 초기화", ImVec2(160.f, 0.f)))
		{
			rotDegTarget = _vector2{ 0.f, 0.f };
		}

		ImGui::SameLine();

		if (ImGui::Button(u8"현재 회전 즉시 적용", ImVec2(160.f, 0.f)))
		{
			rotQuatCurrent = rotQuatTarget;
		}

		ImGui::PopID();
	}
}

void DebugFreeCam::ApplyRotation(_float dt)
{
	const _float yawRad = XMConvertToRadians(rotDegTarget.x);
	const _float pitchRad = XMConvertToRadians(rotDegTarget.y);

	rotQuatTarget = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

	float alpha = 1.f - expf(-rotSmoothSpeed * dt);
	alpha = clamp(alpha, 0.f, 1.f);

	rotQuatCurrent = Quaternion::Slerp(rotQuatCurrent, rotQuatTarget, alpha);
	rotQuatCurrent.Normalize();

	_vector3 forwardVector = _vector3::Transform(_vector3(0.f, 0.f, 1.f), rotQuatCurrent);

	_vector3 forward{ forwardVector.x, forwardVector.y, forwardVector.z };
	if (forward.LengthSquared() <= 1e-8f)
		forward = _vector3{ 0.f, 0.f, 1.f };
	else
		forward.Normalize();

	_vector4 pos4 = transform->Get_Pos();
	_vector3 pos{ pos4.x, pos4.y, pos4.z };

	transform->LookAt(pos + forward);
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
