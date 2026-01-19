#include "pch.h"
#include "FreeCam.h"

HRESULT CFreeCam::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CFreeCam::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	m_pTransform->LookAt({});

	_vector4 look4 = m_pTransform->Dir(STATE::LOOK);
	_vector3 forward{ look4.x, look4.y, look4.z };
	forward.Normalize();

	const float yawRad   = atan2f(forward.x, forward.z);
	const float pitchRad = asinf(-forward.y);
	 
	targetRotDeg.x = XMConvertToDegrees(yawRad);
	targetRotDeg.y = XMConvertToDegrees(pitchRad);
	targetRotDeg.y = clamp(targetRotDeg.y, -89.f, 89.f);

	targetRot = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);
	curRot    = targetRot;

	return S_OK;
}

void CFreeCam::Priority_Update(_float dt)
{
	if (!controlEnabled)
	{
		SyncRotation();
		return;
	}

	auto input = KEY;
	const float ad = (input->Key_Down('D') ? 1.f : 0.f) + (input->Key_Down('A') ? -1.f : 0.f);
	const float ws = (input->Key_Down('W') ? 1.f : 0.f) + (input->Key_Down('S') ? -1.f : 0.f);

	if (moveConstraint == CamMoveConstraint::Orbit)
	{
		if (!orbit.lookAtCenter)
		{
			if (input->Mouse_Down(MOUSE_BTN::RB))
			{
				const float deltaX = input->Mouse_DeltaX();
				const float deltaY = input->Mouse_DeltaY();

				targetRotDeg.x += deltaX * sensitivity;
				targetRotDeg.y += deltaY * sensitivity;
				targetRotDeg.y = clamp(targetRotDeg.y, -89.f, 89.f);
			}
			ApplyRotation(dt);
		}

		_vector4 pos4 = m_pTransform->Get_Pos();
		_vector3 curPos{ pos4.x, pos4.y, pos4.z };

		if (!orbit.initialized)
		{
			if (orbit.distance < 0.1f) orbit.distance = 5.f;

			if (!orbit.useCustomCenter)
			{
				if (orbit.lookAtCenter)
				{
					orbit.center.x = orbit.targetPos.x;
					orbit.center.z = orbit.targetPos.z;
					orbit.center.y = curPos.y;
				}
				else
				{
					_vector4 look4 = m_pTransform->Dir(STATE::LOOK);
					_vector3 look{ look4.x, 0.f, look4.z };
					look.Normalize();

					orbit.center.x = curPos.x + look.x * orbit.distance;
					orbit.center.z = curPos.z + look.z * orbit.distance;
					orbit.center.y = curPos.y;
				}
			}

			const float rx = curPos.x - orbit.center.x;
			const float rz = curPos.z - orbit.center.z;

			const float dXZ = sqrtf(rx * rx + rz * rz);
			orbit.angleDeg = XMConvertToDegrees(atan2f(rz, rx));
			orbit.initialized = true;
		}

		if (!orbit.useCustomCenter && orbit.lookAtCenter)
		{
			orbit.center.x = orbit.targetPos.x;
			orbit.center.z = orbit.targetPos.z;
		}

		orbit.angleDeg += ad * orbit.angularSpeedDeg * dt;

		if (ws != 0.f)
		{
			orbit.distance += ws * orbit.distanceSpeed * dt;
			if (orbit.distance < 0.1f) orbit.distance = 0.1f;
		}

		const float rad = XMConvertToRadians(orbit.angleDeg);
		const float c = cosf(rad);
		const float s = sinf(rad);

		_vector3 nextPos;
		nextPos.x = orbit.center.x + c * orbit.distance;
		nextPos.z = orbit.center.z + s * orbit.distance;
		nextPos.y = curPos.y;

		m_pTransform->Set_Pos(_vector4{ nextPos.x, nextPos.y, nextPos.z, 1.f });

		if (orbit.lookAtCenter)
		{
			_vector3 lookAtPos = orbit.targetPos + _vector3{ 0.f, orbit.offsetY, 0.f };
			m_pTransform->LookAt(lookAtPos);
			SyncRotation();
		}

		return;
	}

	if (input->Mouse_Down(MOUSE_BTN::RB))
	{
		const float deltaX = input->Mouse_DeltaX();
		const float deltaY = input->Mouse_DeltaY();

		targetRotDeg.x += deltaX * sensitivity;
		targetRotDeg.y += deltaY * sensitivity;
		targetRotDeg.y = clamp(targetRotDeg.y, -89.f, 89.f);
	}

	ApplyRotation(dt);

	_vector4 look4 = m_pTransform->Dir(STATE::LOOK);
	_vector4 right4 = m_pTransform->Dir(STATE::RIGHT);

	_vector3 look{ look4.x, look4.y, look4.z };
	_vector3 right{ right4.x, right4.y, right4.z };

	const _float speed = moveSpeed * dt;

	_vector3 move{ 0.f, 0.f, 0.f };

	if (moveConstraint == CamMoveConstraint::X)
		move.x = ad * speed;
	else if (moveConstraint == CamMoveConstraint::Y)
		move.y = ws * speed;
	else if (moveConstraint == CamMoveConstraint::Z)
		move.z = ws * speed;
	else if (moveConstraint == CamMoveConstraint::XY)
	{
		move.x = ad * speed;
		move.y = ws * speed;
	}
	else if (moveConstraint == CamMoveConstraint::XZ)
	{
		move.x = ad * speed;
		move.z = ws * speed;
	}
	else if (moveConstraint == CamMoveConstraint::YZ)
	{
		move.y = ws * speed;
		move.z = ad * speed;
	}
	else
	{
		if (input->Key_Down('W')) move += look  *  speed;
		if (input->Key_Down('S')) move += look  * -speed;
		if (input->Key_Down('D')) move += right *  speed;
		if (input->Key_Down('A')) move += right * -speed;
	}

	if (move.LengthSquared() > 1e-8f)
		m_pTransform->Translate(_vector4{ move.x, move.y, move.z, 0.f });
}

void CFreeCam::SetControlEnabled(_bool enabled)
{
	if (controlEnabled == enabled) return;

	controlEnabled = enabled;

	if (controlEnabled)
		SyncRotation();
}

void CFreeCam::SetMoveConstraint(CamMoveConstraint mode)
{
	if (moveConstraint == mode) return;

	moveConstraint = mode;

	if (moveConstraint == CamMoveConstraint::Orbit)
		orbit.initialized = false;
}

void CFreeCam::SetOrbitState(const CamOrbitState& next)
{
	orbit = next;
	orbit.initialized = false;
}

void CFreeCam::ApplyRotation(_float dt)
{
	const _float yawRad = XMConvertToRadians(targetRotDeg.x);
	const _float pitchRad = XMConvertToRadians(targetRotDeg.y);

	targetRot = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

	float alpha = 1.f - expf(-rotSmoothSpeed * dt);
	alpha = clamp(alpha, 0.f, 1.f);

	curRot = Quaternion::Slerp(curRot, targetRot, alpha);
	curRot.Normalize();

	const _vector4 q4{curRot.x, curRot.y, curRot.z, curRot.w };
	m_pTransform->Set_Quaternion(q4);
}

void CFreeCam::SyncRotation()
{
	_vector4 look4 = m_pTransform->Dir(STATE::LOOK);
	_vector3 forward{ look4.x, look4.y, look4.z };
	forward.Normalize();

	const float yawRad = atan2f(forward.x, forward.z);
	const float pitchRad = asinf(-forward.y);

	targetRotDeg.x = XMConvertToDegrees(yawRad);
	targetRotDeg.y = XMConvertToDegrees(pitchRad);
	targetRotDeg.y = clamp(targetRotDeg.y, -89.f, 89.f);

	targetRot = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);
	curRot = targetRot;
}

CFreeCam* CFreeCam::Create()
{
	auto inst = new CFreeCam();
	if (FAILED(inst->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDebugFreeCam");
		Safe_Release(inst);
	}
	return inst;
}

CGameObject* CFreeCam::Clone(INIT_DESC* pArg)
{
	auto inst = new CFreeCam(*this);
	if (FAILED(inst->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDebugFreeCam");
		Safe_Release(inst);
	}
	return inst;
}

void CFreeCam::Render_GUI()
{
	__super::Render_GUI();

	if (ImGui::CollapsingHeader(u8"DebugFreeCam", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushID("DebugFreeCam_RenderGUI");

		ImGui::DragFloat(u8"이동 속도",        &moveSpeed, 0.1f, 0.f, 5000.f);
		ImGui::DragFloat(u8"마우스 감도",      &sensitivity, 0.001f, 0.f, 5.f);
		ImGui::DragFloat(u8"회전 스무딩 속도", &rotSmoothSpeed, 0.1f, 0.f, 60.f);

		ImGui::Separator();

		ImGui::DragFloat2(u8"회전 목표(도)", &targetRotDeg.x, 0.05f);
		targetRotDeg.y = clamp(targetRotDeg.y, -89.f, 89.f);

		if (ImGui::Button(u8"회전 목표 초기화", ImVec2(160.f, 0.f)))
			targetRotDeg = _vector2{ 0.f, 0.f };

		ImGui::SameLine();

		if (ImGui::Button(u8"현재 회전 즉시 적용", ImVec2(160.f, 0.f)))
			curRot = targetRot;

		ImGui::PopID();
	}
}