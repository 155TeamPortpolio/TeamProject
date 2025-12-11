#include "Engine_Defines.h"
#include "BoneFollower.h"
#include "Animator3D.h"
#include "Transform.h"
#include "GameObject.h"
#include "Transform.h"

CBoneFollower::CBoneFollower()
{
}

CBoneFollower::CBoneFollower(const CBoneFollower& rhs)
{
}

CBoneFollower::~CBoneFollower()
{
}

HRESULT CBoneFollower::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoneFollower::Initialize(COMPONENT_DESC* pArg)
{
	XMStoreFloat4x4(&m_Offset, XMMatrixIdentity());
	return S_OK;
}

void CBoneFollower::Link_Bone(CAnimator3D* pAnimator, const string& boneName)
{
	m_pMasterAnimator = pAnimator;
	m_pMasterTransform = m_pMasterAnimator->Get_Owner()->Get_Component<CTransform>();
	FollowingBone = boneName;
}

void CBoneFollower::Sync_Transform(_float dt, CTransform* pTransform)
{
    if (!m_pMasterAnimator)
        return;

	if(!m_pMasterTransform)
		return;

	_float4x4 boneMatrix = m_pMasterAnimator->Get_BoneMatrix(FollowingBone);
    _matrix matBone = XMLoadFloat4x4(&boneMatrix);

	_float4x4* masterMatrix = m_pMasterTransform->Get_WorldMatrix_Ptr();
    _matrix matMasterWorld = XMLoadFloat4x4(masterMatrix);

    _matrix matOffset = XMLoadFloat4x4(&m_Offset);

    _matrix matWorld = matOffset * matBone* matMasterWorld;

    pTransform->TranslateMatrix(matWorld);
}

void CBoneFollower::Set_Offset(_fmatrix Offset)
{
	XMStoreFloat4x4(&m_Offset, Offset);
}

void CBoneFollower::Dettach()
{
	 m_pMasterAnimator = { nullptr };
	 m_pMasterTransform = { nullptr };
	 FollowingBone = {};
	 m_Offset = {};
}

CBoneFollower* CBoneFollower::Create()
{
	CBoneFollower* instance = new CBoneFollower();
	if (FAILED(instance->Initialize_Prototype())) {
		Safe_Release(instance);
	}
	return instance;
}

CComponent* CBoneFollower::Clone()
{
	return new CBoneFollower(*this);
}


void CBoneFollower::Free()
{
	__super::Free();
}
