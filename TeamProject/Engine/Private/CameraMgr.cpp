#include "Engine_Defines.h"
#include "CameraMgr.h"
#include "Camera.h"
#include "IAudioService.h"
#include "GameInstance.h"
#include"GameObject.h"

CCameraMgr::CCameraMgr()
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_InversedProjMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_InversedViewMatrix, XMMatrixIdentity());
}

void CCameraMgr::Set_MainCam(CCamera* pCamCom)
{
	if(m_pMainCam)
		Safe_Release(m_pMainCam);
	m_pMainCam = pCamCom;

	Safe_AddRef(m_pMainCam);
}

void CCameraMgr::Set_ShadowCam(CCamera* pCamCom)
{
	if (m_pShadowCam)
		Safe_Release(m_pShadowCam);
	m_pShadowCam = pCamCom;
	Safe_AddRef(m_pShadowCam);
}

void CCameraMgr::Update(_float dt)
{
	if (m_pMainCam) {
		m_ViewMatrix = m_pMainCam->Get_ViewMatrix();
		XMStoreFloat4x4(&m_ProjMatrix, m_pMainCam->Get_ProjMatrix());
		XMStoreFloat4x4(&m_InversedViewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ViewMatrix)));
		XMStoreFloat4x4(&m_InversedProjMatrix, XMMatrixInverse(nullptr, m_pMainCam->Get_ProjMatrix()));
		XMStoreFloat4(&m_vCamPos, m_pMainCam->Get_Owner()->Get_Component<CTransform>()->Get_Pos());
	}

	if (m_pShadowCam) {
		XMStoreFloat4x4(&m_ShadowViewMatrix, m_pShadowCam->Get_PureViewMatrix());
		XMStoreFloat4x4(&m_ShadowProjMatrix, m_pShadowCam->Get_ProjMatrix());
		XMStoreFloat4(&m_vShadowCamPos, m_pShadowCam->Get_Owner()->Get_Component<CTransform>()->Get_Pos());
		XMStoreFloat4x4(&m_ShadowInversedViewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ShadowViewMatrix)));
		XMStoreFloat4x4(&m_ShadowInversedProjMatrix, XMMatrixInverse(nullptr, m_pShadowCam->Get_ProjMatrix()));
	}
}


const _float4 CCameraMgr::Get_CameraPos()
{
	return m_vCamPos;
}

const _float CCameraMgr::Get_Far()
{
	if (m_pMainCam) {
		return m_pMainCam->Get_Far();
	}
	return 0.f;
}

const _float4 CCameraMgr::Get_ShadowCameraPos()
{
	return m_vShadowCamPos;
}

const _float CCameraMgr::Get_ShadowFar()
{
	if (m_pShadowCam) {
		return m_pShadowCam->Get_Far();
	}
	return 0.f;
}

CCameraMgr* CCameraMgr::Create()
{
	return new CCameraMgr;
}

void CCameraMgr::Free()
{
	Safe_Release(m_pMainCam);
	Safe_Release(m_pShadowCam);
}
