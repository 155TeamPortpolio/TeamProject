#include "pch.h"
#include "UI_ZeroEntranceLogo.h"

#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"

CUI_ZeroEntranceLogo::CUI_ZeroEntranceLogo()
	:CGameObject()
{
}

CUI_ZeroEntranceLogo::CUI_ZeroEntranceLogo(const CUI_ZeroEntranceLogo& rhs)
	:CGameObject(rhs)
{
}

HRESULT CUI_ZeroEntranceLogo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CStaticModel>()->Link_Model("Test_Level", "UI_3DBillboard_Zero.model");
	Add_Component<CMaterial>()->Link_Material("Test_Level", "UI_3DBillboard_Zero.mat");
	Add_Component<CCollider>();
 
	return S_OK;
}

HRESULT CUI_ZeroEntranceLogo::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	auto pModel = Get_Component<CStaticModel>();
	pModel->Set_RenderType(RENDER_PASS_TYPE::RENDER_3DUI);
	pModel->ShadowCast(false);

	m_pTransform->Set_Pos(_float4(0.f, -4.f, 0.f, 1.f));

	return S_OK;
}

void CUI_ZeroEntranceLogo::Awake()
{
	auto pCollider = Get_Component<CCollider>();
	_float3 vPos = {};
	XMStoreFloat3(&vPos, m_pTransform->Get_Pos());
	pCollider->Set_Center(vPos);
	pCollider->Set_Size({ 2.5f, 2.5f, 2.5f });
	pCollider->Set_Trigger(true);
}

void CUI_ZeroEntranceLogo::Priority_Update(_float dt)
{
}

void CUI_ZeroEntranceLogo::Update(_float dt)
{
	// Y축 고정 빌보드
	_vector vCamPos = CGameInstance::GetInstance()->Get_CameraMgr()->Get_CameraPos();
	m_pTransform->Set_Look(XMVector3Normalize(XMVectorSetY(vCamPos - m_pTransform->Get_Pos(), 0.f)));
}

void CUI_ZeroEntranceLogo::Late_Update(_float dt)
{
}

CGameObject* CUI_ZeroEntranceLogo::Create()
{
	CUI_ZeroEntranceLogo* pInstance = new CUI_ZeroEntranceLogo();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_ZeroEntranceLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_ZeroEntranceLogo::Clone(INIT_DESC* pArg)
{
	CUI_ZeroEntranceLogo* pInstance = new CUI_ZeroEntranceLogo(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_ZeroEntranceLogo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_ZeroEntranceLogo::Free()
{
	__super::Free();
}