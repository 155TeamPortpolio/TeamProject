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
	transform = m_pOwner->Get_Component<CTransform>();
	Safe_AddRef(transform);
	Set_Lens(cam->fFov, cam->fAspect, cam->fNear, cam->fFar);
	
	return S_OK;
}

_matrix CCamera::Get_ProjMatrix() const
{
	switch (projType)
	{
	case CamProjType::Perspective:
		return XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), aspect, zNear, zFar);

	case CamProjType::Orthographic:
		const _float height = orthoSize * 2.f;
		const _float width  = height * aspect;
		return XMMatrixOrthographicLH(width, height, zNear, zFar);
	}
	return {};
}

void CCamera::Set_Lens(_float _fov, _float _aspect, _float _zNear, _float _zFar)
{
	fov    = _fov; 
	aspect = _aspect;
	zNear  = _zNear;
	zFar   = _zFar;
}

_bool CCamera::Lerp_FOV(_float dst, _float dt)
{
	dt = clamp(dt, 0.f, 1.f);
	fov += (dst - fov) * dt;

	if (fabsf(dst - fov) < 0.05f)
	{
		fov = dst;
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
	Safe_Release(transform);
}

void CCamera::Render_GUI()
{
	ImGui::SeparatorText("Camera");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 8) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::BeginChild("##CameraChild", ImVec2{ 0, childHeight }, true);
	ImGui::Text("Field of View");
	ImGui::InputFloat("##FoV", &fov, 1.0f, 0.0f, "%.1f");

	ImGui::Text("Near Plane");
	ImGui::InputFloat("##Near", &zNear, 1.0f, 0.0f, "%.1f");

	ImGui::Text("Far Plane");
	ImGui::InputFloat("##Far", &zFar, 1.0f, 0.0f, "%.1f");

	ImGui::EndChild();
}