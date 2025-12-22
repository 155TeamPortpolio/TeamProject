#include "ModelEditor_Defines.h"
#include "RayCaster.h"
#include "GameInstance.h"

CRayCaster::CRayCaster()
{
}

CRayCaster::CRayCaster(const CRayCaster& rhs)
	:CGameObject(rhs)
{
}

HRESULT CRayCaster::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CRayCaster::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	m_pViewMat = CGameInstance::GetInstance()->Get_CameraMgr()->Get_ViewMatrix();
	m_pProjMat = CGameInstance::GetInstance()->Get_CameraMgr()->Get_ProjMatrix();
	m_tRay.fMaxDistance = 1550.f;
	return S_OK;
}

void CRayCaster::Awake()
{
}

void CRayCaster::Priority_Update(_float dt)
{
}

void CRayCaster::Update(_float dt)
{
	Create_Ray();
}

void CRayCaster::Late_Update(_float dt)
{
	//		m_pRayManager->Get_FrontRayHit()->pObject;

}

void CRayCaster::Render_GUI()
{
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 4 + 2) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::SeparatorText("RayCaster");
	ImGui::BeginChild("##RayCaster", ImVec2{ 0, childHeight }, true);
	_int MousePose[2] = { static_cast<_int>(m_MousePt.x) , static_cast<_int>(m_MousePt.y) };

	ImGui::Text("Mouse Pos");
	ImGui::InputInt2("##MousePos", MousePose,ImGuiInputTextFlags_ReadOnly);
	ImGui::Text("Ray Distance");
	ImGui::DragFloat("##RayDistance", &m_tRay.fMaxDistance);

	ImGui::EndChild();
}

void CRayCaster::Create_Ray()
{
	GetCursorPos(&m_MousePt);
	ScreenToClient(g_hWnd, &m_MousePt);

	/*마우스 좌표 변환*/
	_vector vMouseOrigin = XMVectorSet(
		static_cast<float>(m_MousePt.x),
		static_cast<float>(m_MousePt.y),
		0.f,
		1.f

	);

	/*마우스 레이 변환*/
	XMVECTOR raySrc = XMVector3Unproject(
		vMouseOrigin,
		0.0f, 0.0f,
		static_cast<float>(ModelEdit::g_iWinSizeX),
		static_cast<float>(ModelEdit::g_iWinSizeY),
		0.0f, 1.0f,
		XMLoadFloat4x4(m_pProjMat),
		XMLoadFloat4x4(m_pViewMat),
		XMMatrixIdentity()
	);

	/*마우스 목적지 좌표*/
	_vector vMouseDest = XMVectorSet(
		static_cast<float>(m_MousePt.x),
		static_cast<float>(m_MousePt.y),
		1.f,
		1.f
	);

	XMVECTOR rayDest = XMVector3Unproject(
		vMouseDest,
		0.0f, 0.0f,
		static_cast<float>(ModelEdit::g_iWinSizeX),
		static_cast<float>(ModelEdit::g_iWinSizeY),
		0.0f, 1.0f,
		XMLoadFloat4x4(m_pProjMat),
		XMLoadFloat4x4(m_pViewMat),
		XMMatrixIdentity()
	);

	XMVECTOR rayOrigin = raySrc;
	XMVECTOR rayDir = XMVector3Normalize(rayDest - raySrc);

	XMStoreFloat3(&m_tRay.vRayDirection, rayDir);
	XMStoreFloat3(&m_tRay.vRayOrigin, rayOrigin);
}


CRayCaster* CRayCaster::Create()
{
	CRayCaster* instance = new CRayCaster();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CRayCaster");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CRayCaster::Clone(INIT_DESC* pArg)
{
	CRayCaster* instance = new CRayCaster(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CRayCaster");
		Safe_Release(instance);
	}

	return instance;
}

void CRayCaster::Free()
{
	__super::Free();
}