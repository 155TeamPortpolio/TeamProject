#include "pch.h"
#include "UI_EntranceLogo.h"

#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"

#include "UI_NameIndicator.h"

CUI_EntranceLogo::CUI_EntranceLogo()
	:CGameObject()
{
}

CUI_EntranceLogo::CUI_EntranceLogo(const CUI_EntranceLogo& rhs)
	:CGameObject(rhs)
{
}

HRESULT CUI_EntranceLogo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_EntranceLogo::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	auto pModel = Get_Component<CStaticModel>();
	pModel->Set_RenderType(RENDER_PASS_TYPE::RENDER_3DUI);
	pModel->ShadowCast(false);

	return S_OK;
}

void CUI_EntranceLogo::Awake()
{
	Ready_Collider();
	Ready_NameIndicator();
}

void CUI_EntranceLogo::Priority_Update(_float dt)
{
}

void CUI_EntranceLogo::Update(_float dt)
{
	Update_YBillboard();
}

void CUI_EntranceLogo::Late_Update(_float dt)
{
}

void CUI_EntranceLogo::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	Update_UI_Interaction(true);
}

void CUI_EntranceLogo::OnTriggerStay(CGameObject* pOher)
{
}

void CUI_EntranceLogo::OnTriggerExit(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	Update_UI_Interaction(false);
}

void CUI_EntranceLogo::Interact(CGameObject* pObject)
{
}

OBJECT_HANDLE CUI_EntranceLogo::Get_InteractHandle()
{
	return this->Get_Handle();
}

void CUI_EntranceLogo::Ready_Collider()
{
	auto pCollider = Get_Component<CCollider>();

	_float3 vPos = {};
	XMStoreFloat3(&vPos, m_pTransform->Get_Pos());
}

void CUI_EntranceLogo::Ready_NameIndicator()
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

void CUI_EntranceLogo::Update_UI_Interaction(_bool bInteract)
{
	UI_INTERACTABLE_DESC desc;
	desc.isInteractable = bInteract;
	desc.strName = m_strName;
	EventSystem()->Broadcast<UI_INTERACTABLE_DESC>({ desc });
}

void CUI_EntranceLogo::Update_YBillboard()
{
	_vector vCamPos = CameraManager()->Get_CameraPos();
	m_pTransform->Set_Look(XMVector3Normalize(XMVectorSetY(vCamPos - m_pTransform->Get_Pos(), 0.f)));
}

void CUI_EntranceLogo::Free()
{
	__super::Free();
}