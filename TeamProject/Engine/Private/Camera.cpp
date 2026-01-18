#include "Engine_Defines.h"
#include "Camera.h"
#include "GameObject.h"

HRESULT CCamera::Initialize(COMPONENT_DESC* pArg)
{
	if (pArg == nullptr) {
		Set_Lens(60.f, 1600.f/900.f, 0.1f, 500.f);
		return S_OK;
	}
	CAMERA_DESC* cam = static_cast<CAMERA_DESC*>(pArg);
	Set_Lens(cam->fFov, cam->fAspect, cam->fNear, cam->fFar); 
	return S_OK;
}

Matrix CCamera::Get_ViewMatrix() const
{
	return m_pOwner->Get_Component<CTransform>()->Get_InverseWorldMatrix();
}

Matrix CCamera::Get_ProjMatrix() const
{
	switch (m_projType)
	{
	case CamProjType::Perspective:
		return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_lens.fov), m_lens.aspect, m_lens.zNear , m_lens.zFar);

	case CamProjType::Orthographic:
		const _float height = m_orthoSize * 2.f;
		const _float width  = height * m_lens.aspect;
		return XMMatrixOrthographicLH(width, height, m_lens.zNear, m_lens.zFar);
	}
	return {};
}

_vector CCamera::Get_Pos() const
{
	return m_pOwner->Get_Component<CTransform>()->Get_Pos();
}

void CCamera::Set_Lens(_float fov, _float aspect, _float zNear, _float zFar)
{
	m_lens.fov    = fov; 
	m_lens.aspect = aspect;
	m_lens.zNear  = zNear;
	m_lens.zFar   = zFar;
}

_bool CCamera::Lerp_FOV(_float dst, _float dt)
{
	dt = clamp(dt, 0.f, 1.f);
	m_lens.fov += (dst - m_lens.fov) * dt;

	if (fabsf(dst - m_lens.fov) < 0.05f)
	{
		m_lens.fov = dst;
		return true;
	}
	return false;
}

CCamera* CCamera::Create()
{
	CCamera* instance = new CCamera();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Camera Create Failed : CCamera");
		Safe_Release(instance);
	}
	return instance;
}

void CCamera::Render_GUI()
{
	__super::Render_GUI();

	ImGui::SeparatorText("Camera");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 8) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::BeginChild("##CameraChild", ImVec2{ 0, childHeight }, true);
	ImGui::Text("Field of View");
	ImGui::InputFloat("##FoV", &m_lens.fov, 1.0f, 0.0f, "%.1f");

	ImGui::Text("Near Plane");
	ImGui::InputFloat("##Near", &m_lens.zNear, 1.0f, 0.0f, "%.1f");

	ImGui::Text("Far Plane");
	ImGui::InputFloat("##Far", &m_lens.zFar, 1.0f, 0.0f, "%.1f");

	ImGui::EndChild();
}