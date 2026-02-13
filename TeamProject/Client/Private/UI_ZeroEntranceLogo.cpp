#include "pch.h"
#include "UI_ZeroEntranceLogo.h"

#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"

#include "UI_NameIndicator.h"

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

	Add_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "UI_3DBillboard_Zero.model");
	Add_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "UI_3DBillboard_Zero.mat");
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

	//m_pTransform->Set_Pos(_float4(0.f, -4.f, 0.f, 1.f));
	//m_pTransform->Scale(_float3(0.5f, 0.5f, 0.5));

	return S_OK;
}

void CUI_ZeroEntranceLogo::Awake()
{
	Ready_Collider();
	Ready_NameIndicator();
}

void CUI_ZeroEntranceLogo::Priority_Update(_float dt)
{
}

void CUI_ZeroEntranceLogo::Update(_float dt)
{
	Update_YBillboard();
}

void CUI_ZeroEntranceLogo::Late_Update(_float dt)
{
}

void CUI_ZeroEntranceLogo::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	Update_UI_Interaction(true);
}

void CUI_ZeroEntranceLogo::OnTriggerStay(CGameObject* pOher)
{
}

void CUI_ZeroEntranceLogo::OnTriggerExit(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	Update_UI_Interaction(false);
}

void CUI_ZeroEntranceLogo::Interact(CGameObject* pObject)
{
	LevelManager()->Request_ChangeLevel("Zero_Level", true);
}

OBJECT_HANDLE CUI_ZeroEntranceLogo::Get_InteractHandle()
{
	return this->Get_Handle();
}

void CUI_ZeroEntranceLogo::Ready_Collider()
{
	auto pCollider = Get_Component<CCollider>();

	_float3 vPos = {};
	XMStoreFloat3(&vPos, m_pTransform->Get_Pos());

	//pCollider->Set_Center(vPos);
	//pCollider->Set_Size({ 4.f, 4.f, 4.f });
	//pCollider->Set_Trigger(true);
}

void CUI_ZeroEntranceLogo::Ready_NameIndicator()
{
	CUI_NameIndicator::INDICATOR_DESC* pDesc = new CUI_NameIndicator::INDICATOR_DESC;
	pDesc->strName = m_strName;
	auto tMinMaxInfo = Get_Component<CStaticModel>()->Get_LocalBoundingBox().TransformBox_8Corner(m_pTransform->Get_WorldMatrix());
	_float fHeight = tMinMaxInfo.vMax.y - tMinMaxInfo.vMin.y;
	XMStoreFloat3(&pDesc->vPosition, m_pTransform->Get_Pos() + XMVectorSet(0.f, fHeight * 1.2f, 0.f, 0.f));

	auto pUI = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_NameIndicator" })
		.Add_UIDesc(pDesc)
		.Build("nameIndicator");

	UIManager()->Add_UIObject(pUI, LevelManager()->Get_NowLevelKey());
}

void CUI_ZeroEntranceLogo::Update_UI_Interaction(_bool bInteract)
{
	UI_INTERACTABLE_DESC desc;
	desc.isInteractable = bInteract;
	desc.strName = m_strName;
	EventSystem()->Broadcast<UI_INTERACTABLE_DESC>({ desc });
}

void CUI_ZeroEntranceLogo::Update_YBillboard()
{
	_vector vCamPos = CameraManager()->Get_CameraPos();
	m_pTransform->Set_Look(XMVector3Normalize(XMVectorSetY(vCamPos - m_pTransform->Get_Pos(), 0.f)));
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