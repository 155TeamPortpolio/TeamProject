#include "Camera.h"
#include "GameObject.h"

CCamera::CCamera()
{
}

CCamera::CCamera(const CCamera& rhs)
	:CComponent(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(COMPONENT_DESC* pArg)
{
	if (pArg == nullptr)
		return S_OK;

	CAMERA_DESC* Camera = static_cast<CAMERA_DESC*>(pArg);
	m_pTransform = m_pOwner->Get_Component<CTransform>();
	Safe_AddRef(m_pTransform); 

	m_fFov = Camera->fFov;
	m_fFar = Camera->fFar;
	m_fNear = Camera->fNear;
	m_fAspect = Camera->fAspect;
	
	return S_OK;
}

_float4x4 CCamera::Get_ViewMatrix()
{

	return m_pTransform->Get_InverseWorldMatrix();
}

_matrix CCamera::Get_PureViewMatrix()
{
	_float3     vUpDir = { 0.f, 1.f, 0.f };

	//	return XMMatrixLookAtLH(
	//		XMVectorSetW(m_pTransform->Get_WorldPos(), 1.f),
	//		XMVectorSetW(m_pTransform->Get_WorldPos()+m_pTransform->Dir(STATE::LOOK), 1.f),
	//		XMLoadFloat3(&vUpDir));
	return XMLoadFloat4x4(m_pTransform->Get_InverseWorldMatrix_Ptr());
}

_matrix CCamera::Get_ProjMatrix()
{
	return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFov), m_fAspect,m_fNear,m_fFar);
}

_bool CCamera::Lerp_FOV(_float dst, _float dt)
{
	if (dt < 0.f) dt = 0.f;
	if (dt > 1.f) dt = 1.f;

	m_fFov = m_fFov + (dst - m_fFov) * dt;

	if (fabsf(dst - m_fFov) < 0.05f)
	{
		m_fFov = dst;
		return true;
	}

	return false;
}

void CCamera::Render_GUI()
{
	ImGui::SeparatorText("Camera");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 8) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::BeginChild("##CameraChild", ImVec2{ 0, childHeight }, true);
	ImGui::Text("Field of View");
	ImGui::InputFloat("##FoV", &m_fFov, 1.0f, 0.0f, "%.1f");

	ImGui::Text("Near Plane");
	ImGui::InputFloat("##Near", &m_fNear, 1.0f, 0.0f, "%.1f");

	ImGui::Text("Far Plane");
	ImGui::InputFloat("##Far", &m_fFar, 1.0f, 0.0f, "%.1f");

	ImGui::EndChild();
}

CCamera* CCamera::Create()
{
	CCamera* instance = new CCamera();
	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("Camera Create Failed : CCamera");
		Safe_Release(instance);
	}
	return instance;
}

CComponent* CCamera::Clone()
{
	CCamera* instance = new CCamera(*this);
	return instance;
}

void CCamera::Free()
{
	__super::Free();
	Safe_Release(m_pTransform);
}
