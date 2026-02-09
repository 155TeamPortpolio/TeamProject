#include "pch.h"
#include "Miyabi_Ghost.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"


CMiyabi_Ghost::CMiyabi_Ghost()
	: CGameObject()
{
}

CMiyabi_Ghost::CMiyabi_Ghost(const CMiyabi_Ghost& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiyabi_Ghost::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CAnimator3D>();
	Add_Component<CRigidBody>();

	Get_Component<CModel>()->Link_Model(G_GlobalLevelKey, "Miyabi_Ghost.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Miyabi_Ghost.mat");

	return S_OK;
}

HRESULT CMiyabi_Ghost::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMiyabi_Ghost::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Miyabi_Ghost.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Miyabi_Ghost_Meta.json");
	pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
	pAnimator->Set_Animation("Avatar_Female_Size02_Unagi_Ghost_Ani_Idle")
		.Loop(true)
		.Apply();
}

void CMiyabi_Ghost::Priority_Update(_float dt)
{
}

void CMiyabi_Ghost::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	FollowTarget(dt);
}

void CMiyabi_Ghost::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CMiyabi_Ghost::FollowTarget(_float dt)
{
	_vector vTargetPos = m_pTargetTransform->Get_WorldPos();
	_vector vTargetRight = m_pTargetTransform->Dir(STATE::RIGHT);
	_vector vTargetLook = m_pTargetTransform->Dir(STATE::LOOK);

	_vector vDesired = vTargetPos
		+ vTargetRight * m_vOffset.x
		+ XMVectorSet(0.f, m_vOffset.y, 0.f, 0.f)
		+ vTargetLook * m_vOffset.z;

	_vector vCurrentPos = m_pTransform->Get_WorldPos();
	_float fDist = XMVectorGetX(XMVector3Length(vDesired - vCurrentPos));

	_float fSpeed = m_fFollowSpeed + fDist * m_fDistanceScale;
	_float t = 1.f - expf(-fSpeed * dt);
	_vector vNewPos = XMVectorLerp(vCurrentPos, vDesired, t);

	m_pTransform->Set_vectorPos(vNewPos);
	m_pTransform->Set_Look(vTargetLook);
}

CMiyabi_Ghost* CMiyabi_Ghost::Create()
{
	CMiyabi_Ghost* instance = new CMiyabi_Ghost();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CMiyabi_Ghost");
	}

	return instance;
}

CGameObject* CMiyabi_Ghost::Clone(INIT_DESC* pArg)
{
	CMiyabi_Ghost* instance = new CMiyabi_Ghost(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CMiyabi_Ghost");
	}

	return instance;
}

void CMiyabi_Ghost::Free()
{
	__super::Free();
}
