#include "Engine_Defines.h"
#include "Camera.h"
#include "GameObject.h"

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(COMPONENT_DESC* pArg)
{
	if (!pArg) return S_OK;

	CAMERA_DESC* cam = static_cast<CAMERA_DESC*>(pArg);
	m_transform = m_pOwner->Get_Component<CTransform>();
	Safe_AddRef(m_transform);
	Set_Lens(cam->fFov, cam->fAspect, cam->fNear, cam->fFar);
	return S_OK;
}

Matrix CCamera::Get_ProjMatrix() const
{
	switch (m_projType)
	{
	case CamProjType::Perspective:
		return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fov), m_aspect, m_zNear, m_zFar);

	case CamProjType::Orthographic:
		const _float height = m_orthoSize * 2.f;
		const _float width  = height * m_aspect;
		return XMMatrixOrthographicLH(width, height, m_zNear, m_zFar);
	}
	return {};
}

void CCamera::Set_Lens(_float _fov, _float _aspect, _float _zNear, _float _zFar)
{
	m_fov    = _fov; 
	m_aspect = _aspect;
	m_zNear  = _zNear;
	m_zFar   = _zFar;
}

_bool CCamera::Lerp_FOV(_float dst, _float dt)
{
	dt = clamp(dt, 0.f, 1.f);
	m_fov += (dst - m_fov) * dt;

	if (fabsf(dst - m_fov) < 0.05f)
	{
		m_fov = dst;
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

CComponent* CCamera::Clone()
{
	return new CCamera(*this);
}

void CCamera::Free()
{
	__super::Free();
	Safe_Release(m_transform);
}

void CCamera::Render_GUI()
{
	ImGui::SeparatorText("Camera");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 8) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::BeginChild("##CameraChild", ImVec2{ 0, childHeight }, true);
	ImGui::Text("Field of View");
	ImGui::InputFloat("##FoV", &m_fov, 1.0f, 0.0f, "%.1f");

	ImGui::Text("Near Plane");
	ImGui::InputFloat("##Near", &m_zNear, 1.0f, 0.0f, "%.1f");

	ImGui::Text("Far Plane");
	ImGui::InputFloat("##Far", &m_zFar, 1.0f, 0.0f, "%.1f");

	ImGui::EndChild();
}