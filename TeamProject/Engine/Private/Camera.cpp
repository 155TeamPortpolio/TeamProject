#include "Engine_Defines.h"
#include "Camera.h"
#include "GameObject.h"

#include "GameInstance.h"

HRESULT CCamera::Initialize(COMPONENT_DESC* pArg)
{
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
	if (m_projType == CamProjType::Perspective)
		return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_lens.fov), m_lens.aspect, m_lens.zNear, m_lens.zFar);
	else
	{
		const _float height = m_orthoSize * 2.f;
		const _float width = height * m_lens.aspect;
		return XMMatrixOrthographicLH(width, height, m_lens.zNear, m_lens.zFar);
	}
}

void CCamera::Set_Lens(_float fov, _float aspect, _float zNear, _float zFar)
{
	m_lens.fov    = fov; 
	m_lens.aspect = aspect;
	m_lens.zNear  = zNear;
	m_lens.zFar   = zFar;
}

CCamera* CCamera::Create()
{
	auto inst = new CCamera();
	if (FAILED(inst->Initialize_Prototype()))
	{
		MSG_BOX("Camera Create Failed : CCamera");
		Safe_Release(inst);
	}
	return inst;
}

void CCamera::Render_GUI()
{
	__super::Render_GUI();

	ImGui::SeparatorText("Camera");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 8) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::BeginChild("##CameraChild", ImVec2{0, childHeight}, true);

	ImGui::Text("Field of View");
	ImGui::Text("%.1f", CameraManager()->GetFov());

	ImGui::EndChild();
}
