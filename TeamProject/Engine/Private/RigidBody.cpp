#pragma once
#include "Engine_Defines.h"
#include "RigidBody.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"

#ifdef USINGPHYSICS 

CRigidBody::CRigidBody()
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs)
	: CComponent(rhs)
	, m_bStatic(rhs.m_bStatic)
	, m_bKinematic(rhs.m_bKinematic)
	, m_bGravity(rhs.m_bGravity)
	, m_fMass(rhs.m_fMass)
	, m_bLockX(rhs.m_bLockX)
	, m_bLockY(rhs.m_bLockY)
	, m_bLockZ(rhs.m_bLockZ)
	, m_pPhysicsSystem(nullptr)
	, m_pOwnerTransform(nullptr)
	, m_pActor(nullptr)
{
}

HRESULT CRigidBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRigidBody::Initialize(COMPONENT_DESC* pArg)
{
	m_pPhysicsSystem = CGameInstance::GetInstance()->Get_PhysicsSystem();
	if (!m_pPhysicsSystem)
	{
		MSG_BOX("CRigidBody::Initialize : PhysicsSystem is nullptr");
		return E_FAIL;
	}

	m_pOwnerTransform = m_pOwner->Get_Component<CTransform>();
	if (!m_pOwnerTransform)
	{
		MSG_BOX("CRigidBody::Initialize : Owner's Transform is nullptr");
		return E_FAIL;
	}

	PxPhysics* pPhysics = m_pPhysicsSystem->Get_Physics();
	PxScene* pScene = m_pPhysicsSystem->Get_Scene();
	if (!pPhysics || !pScene) return E_FAIL;

	if (pArg)
	{
		RIGIDBODY_DESC* pDesc = static_cast<RIGIDBODY_DESC*>(pArg);
		m_bStatic = pDesc->isStatic;
		m_bKinematic = pDesc->isKinematic;
		m_bGravity = pDesc->bEnableGravity;
		m_fMass = pDesc->fMass;

		// 초기 Lock 설정
		m_bLockX = pDesc->bLockX;
		m_bLockY = pDesc->bLockY;
		m_bLockZ = pDesc->bLockZ;

		m_pMaterial = m_pPhysicsSystem->Get_Material(pDesc->strMaterialTag);
	}
	else
	{
		m_pMaterial = m_pPhysicsSystem->Get_DefaultMaterial();
	}

	// 초기 위치
	_vector vPos = m_pOwnerTransform->Get_WorldPos();
	_matrix mWorldMat = XMLoadFloat4x4(m_pOwnerTransform->Get_WorldMatrix_Ptr());
	_vector vScale, vRot, vTrans;
	XMMatrixDecompose(&vScale, &vRot, &vTrans, mWorldMat);
	PxTransform initialPose(ToPxVec3(vPos), ToPxQuat(vRot));

	if(m_bStatic)
		m_pActor = pPhysics->createRigidStatic(initialPose);
	else
	{
		PxRigidDynamic* pDynamic = pPhysics->createRigidDynamic(initialPose);

		pDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, m_bKinematic);
		pDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !m_bGravity);
		pDynamic->setMass(m_fMass);

		pDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, m_bLockX);
		pDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, m_bLockY);
		pDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, m_bLockZ);

		m_pActor = pDynamic;
	}
	m_pActor->userData = m_pOwner;
	pScene->addActor(*m_pActor);

	return S_OK;
}

void CRigidBody::Update(_float dt)
{
	Update_RigidBody();
}

PxShape* CRigidBody::Attach_Shape(const PxGeometry& geometry, const string& strMaterialName)
{
	if (!m_pActor) return nullptr;

	PxMaterial* pUseMaterial = m_pMaterial;
	if (strMaterialName != "") {
		PxMaterial* pFoundMat = m_pPhysicsSystem->Get_Material(strMaterialName);
		if (pFoundMat) pUseMaterial = pFoundMat;
	}

	PxShape* pShape = PxRigidActorExt::createExclusiveShape(*m_pActor, geometry, *pUseMaterial);

	if (pShape && !m_bStatic)
	{
		Update_Inertia();
	}
	return pShape;
}

void CRigidBody::Add_Force(_fvector vForce, PxForceMode::Enum eMode)
{
	if (m_pActor && !m_bStatic && !m_bKinematic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->addForce(ToPxVec3(vForce), eMode);
	}
}

void CRigidBody::Add_Torque(_fvector vTorque, PxForceMode::Enum eMode)
{
	if (m_pActor && !m_bStatic && !m_bKinematic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->addTorque(ToPxVec3(vTorque), eMode);
	}
}

void CRigidBody::Set_Velocity(_fvector vVelocity)
{
	if (m_pActor && !m_bStatic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->setLinearVelocity(ToPxVec3(vVelocity));
	}
}

void CRigidBody::Set_AngularVelocity(_fvector vAngVelocity)
{
	if (m_pActor && !m_bStatic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->setAngularVelocity(ToPxVec3(vAngVelocity));
	}
}

_vector CRigidBody::Get_Velocity()
{
	if (m_pActor && !m_bStatic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) {
			_vector vVel = ToDxVec(pDynamic->getLinearVelocity());
			return XMVectorSetW(vVel, 0.f);
		}
	}
	return XMVectorZero();
}

void CRigidBody::Set_GlobalPos(_fvector vPos, _fvector vQuat)
{
	if (m_pActor)
	{
		PxTransform pose(ToPxVec3(vPos), ToPxQuat(vQuat));
		m_pActor->setGlobalPose(pose);
		Set_Velocity(XMVectorZero());	// 안정성을 위한 속도 초기화
	}
}

void CRigidBody::Set_Gravity(_bool bEnable)
{
	m_bGravity = bEnable;
	if (m_pActor)
	{
		m_pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !m_bGravity);
	}
}

void CRigidBody::Set_RotationLock(_bool bLockX, _bool bLockY, _bool bLockZ)
{
	m_bLockX = bLockX;
	m_bLockY = bLockY;
	m_bLockZ = bLockZ;

	if (m_pActor && !m_bStatic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic)
		{
			pDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, m_bLockX);
			pDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, m_bLockY);
			pDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, m_bLockZ);
		}
	}
}

void CRigidBody::Set_Mass(_float fMass)
{
	m_fMass = fMass;
	Update_Inertia();
}

void CRigidBody::Set_Kinematic(_bool bKinematic)
{
	m_bKinematic = bKinematic;
	if (m_pActor && !m_bStatic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, m_bKinematic);
	}
}

void CRigidBody::Update_RigidBody()
{
	if (!m_pActor || m_bStatic) return;
	if (m_bKinematic)
	{
		// 로직이 위치를 제어 : Transform -> Physics
		_vector vPos = m_pOwnerTransform->Get_WorldPos();
		_matrix worldMat = XMLoadFloat4x4(m_pOwnerTransform->Get_WorldMatrix_Ptr());
		_vector vScale, vRot, vTrans;
		XMMatrixDecompose(&vScale, &vRot, &vTrans, worldMat);
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic)
		{
			pDynamic->setKinematicTarget(PxTransform(ToPxVec3(vPos), ToPxQuat(vRot)));
		}
	}
	else
	{
		// 시뮬레이션 결과가 트랜스폼을 덮어씀 :  Physics -> Transform
		PxTransform globalPose = m_pActor->getGlobalPose();
		m_pOwnerTransform->Set_WorldPos(ToDxVec(globalPose.p));
		m_pOwnerTransform->Set_WorldQuaternion(ToDxQuat(globalPose.q));
	}
}

void CRigidBody::Update_Inertia()
{
	if (!m_pActor || m_bStatic) return;
	if (m_pActor->getNbShapes() == 0) return;

	PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
	if (!pDynamic) return;

	// 밀도 1.0을 기준으로 질량 중심 계산(회전축, 비율)
	PxRigidBodyExt::updateMassAndInertia(*pDynamic, 1.0f);
	float calculatedMass = pDynamic->getMass();		// 밀도 1일 때 질량

	// (목표 질량 / 계산된 질량) 비율만큼 스케일링 -> 무게(m_fMass)에 맞는 회전
	if (calculatedMass > 0.0f)
	{
		PxVec3 vInertia = pDynamic->getMassSpaceInertiaTensor();
		vInertia *= (m_fMass / calculatedMass);
		pDynamic->setMassSpaceInertiaTensor(vInertia);
	}
	pDynamic->setMass(m_fMass);
}

PxVec3 CRigidBody::ToPxVec3(_fvector vVec)
{
	_float3 v;
	XMStoreFloat3(&v, vVec);
	return PxVec3(v.x, v.y, v.z);
}

PxQuat CRigidBody::ToPxQuat(_fvector vQuat)
{
	_float4 v;
	XMStoreFloat4(&v, vQuat);
	return PxQuat(v.x, v.y, v.z, v.w);
}

_vector CRigidBody::ToDxVec(const PxVec3& vVec)
{
	return XMVectorSet(vVec.x, vVec.y, vVec.z, 1.f);
}

_vector CRigidBody::ToDxQuat(const PxQuat& vQuat)
{
	return XMVectorSet(vQuat.x, vQuat.y, vQuat.z, vQuat.w);
}

CRigidBody* CRigidBody::Create()
{
	CRigidBody* instance = new CRigidBody();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("RigidBody Create Failed : CRigidBody");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CRigidBody::Clone()
{
	CRigidBody* instance = new CRigidBody(*this);
	return instance;
}

void CRigidBody::Free()
{
	if (m_pActor)
	{
		PxScene* pScene = m_pActor->getScene();
		if (pScene)
			pScene->removeActor(*m_pActor);

		m_pActor->release();
		m_pActor = nullptr;
	}
	__super::Free();
}
#endif
