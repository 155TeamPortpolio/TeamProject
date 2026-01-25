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
	, m_bKinematic(rhs.m_bKinematic)
	, m_bGravity(rhs.m_bGravity)
	, m_fMass(rhs.m_fMass)
	, m_bLockX(rhs.m_bLockX)
	, m_bLockY(rhs.m_bLockY)
	, m_bLockZ(rhs.m_bLockZ)
	, m_fLinearDamping(rhs.m_fLinearDamping)
	, m_fAngularDamping(rhs.m_fAngularDamping)
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

	RIGIDBODY_DESC desc;
	if (pArg)
		desc = *static_cast<RIGIDBODY_DESC*>(pArg);

	m_bKinematic = desc.isKinematic;
	m_bGravity = desc.bEnableGravity;
	m_fMass = desc.fMass;
	m_bLockX = desc.bLockX;
	m_bLockY = desc.bLockY;
	m_bLockZ = desc.bLockZ;
	m_fLinearDamping = desc.fLinearDamping;
	m_fAngularDamping = desc.fAngularDamping;

	_vector vPos = m_pOwnerTransform->Get_WorldPos();
	_smatrix mWorldMat = m_pOwnerTransform->Get_WorldMatrix();
	_vector vScale, vRot, vTrans;
	XMMatrixDecompose(&vScale, &vRot, &vTrans, mWorldMat);

	PxTransform initialPose(ToPxVec3(vPos), ToPxQuat(vRot));
	m_pActor = pPhysics->createRigidDynamic(initialPose);
	if (!m_pActor) return E_FAIL;

	m_pActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, m_bKinematic);
	m_pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !m_bGravity);
	m_pActor->setMass(m_fMass);
	m_pActor->setLinearDamping(m_fLinearDamping);
	m_pActor->setAngularDamping(m_fAngularDamping);
	m_pActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, m_bLockX);
	m_pActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, m_bLockY);
	m_pActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, m_bLockZ);
	m_pActor->setSolverIterationCounts(8, 4);

	if (!m_bKinematic)
	{
		m_pActor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		m_pActor->setMinCCDAdvanceCoefficient(0.15f);
	}

	m_pActor->setMaxDepenetrationVelocity(1.0f);
	m_pActor->setActorFlag(PxActorFlag::eSEND_SLEEP_NOTIFIES, true);
	m_pActor->userData = m_pOwner;

	pScene->addActor(*m_pActor);

	return S_OK;
}

HRESULT CRigidBody::ReInitialize(COMPONENT_DESC* pArg)
{
	RIGIDBODY_DESC desc;
	if (pArg)
		desc = *static_cast<RIGIDBODY_DESC*>(pArg);

	// Pose
	_vector vPos = m_pOwnerTransform->Get_WorldPos();
	_smatrix mWorldMat = m_pOwnerTransform->Get_WorldMatrix();
	_vector vScale, vRot, vTrans;
	XMMatrixDecompose(&vScale, &vRot, &vTrans, mWorldMat);

	PxTransform pose(ToPxVec3(vPos), ToPxQuat(vRot));
	m_pActor->setGlobalPose(pose);

	// Velocity Reset
	PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
	if (pDynamic)
	{
		pDynamic->setLinearVelocity(PxVec3(0.f));
		pDynamic->setAngularVelocity(PxVec3(0.f));
	}

	// Kinematic
	if (m_bKinematic != desc.isKinematic)
		Set_Kinematic(desc.isKinematic);

	// Gravity
	if (m_bGravity != desc.bEnableGravity)
		Set_Gravity(desc.bEnableGravity);

	// Mass
	if (m_fMass != desc.fMass)
		Set_Mass(desc.fMass);

	// Damping
	if (m_fLinearDamping != desc.fLinearDamping)
		Set_LinearDamping(desc.fLinearDamping);

	if (m_fAngularDamping != desc.fAngularDamping)
		Set_AngularDamping(desc.fAngularDamping);

	// Rotation Lock
	if (m_bLockX != desc.bLockX || m_bLockY != desc.bLockY || m_bLockZ != desc.bLockZ)
		Set_RotationLock(desc.bLockX, desc.bLockY, desc.bLockZ);

	// Wake Up
	if (pDynamic && pDynamic->isSleeping())
		pDynamic->wakeUp();

	return S_OK;
}

void CRigidBody::Late_Update(_float dt)
{
	if (!m_pActor) return;
	Update_RigidBody();
}

void CRigidBody::Render_GUI()
{
	if (!Get_CompActive()) return;

	ImGui::SeparatorText("RigidBody");

	if (ImGui::BeginChild("##RigidBodyChild", ImVec2(0, 350), true))
	{
		ImGui::Text("Type: %s", m_bKinematic ? "Kinematic" : "NonKinematic");


		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic)
		{
			ImGui::Separator();
			ImGui::Text("Properties");

			if (ImGui::DragFloat("Mass", &m_fMass, 0.1f, 0.1f, 1000.0f))
			{
				Set_Mass(m_fMass);
			}

			_bool bGravity = m_bGravity;
			if (ImGui::Checkbox("Gravity", &bGravity))
			{
				Set_Gravity(bGravity);
			}

			_bool bKinematic = m_bKinematic;
			if (ImGui::Checkbox("Kinematic", &bKinematic))
			{
				Set_Kinematic(bKinematic);
			}

			ImGui::Separator();
			ImGui::Text("Damping");
			if (ImGui::DragFloat("Linear Damping", &m_fLinearDamping, 0.01f, 0.0f, 10.0f))
			{
				pDynamic->setLinearDamping(m_fLinearDamping);
			}
			if (ImGui::DragFloat("Angular Damping", &m_fAngularDamping, 0.01f, 0.0f, 10.0f))
			{
				pDynamic->setAngularDamping(m_fAngularDamping);
			}

			ImGui::Separator();
			ImGui::Text("Rotation Lock");
			_bool bLockX = m_bLockX;
			_bool bLockY = m_bLockY;
			_bool bLockZ = m_bLockZ;

			if (ImGui::Checkbox("Lock X", &bLockX) |
				ImGui::Checkbox("Lock Y", &bLockY) |
				ImGui::Checkbox("Lock Z", &bLockZ))
			{
				Set_RotationLock(bLockX, bLockY, bLockZ);
			}

			ImGui::Separator();
			PxVec3 vel = pDynamic->getLinearVelocity();
			PxVec3 angVel = pDynamic->getAngularVelocity();

			ImGui::Text("Linear Velocity");
			ImGui::Text("X: %.2f | Y: %.2f | Z: %.2f", vel.x, vel.y, vel.z);
			ImGui::Text("Speed: %.2f m/s", vel.magnitude());

			ImGui::Separator();
			ImGui::Text("Angular Velocity");
			ImGui::Text("X: %.2f | Y: %.2f | Z: %.2f", angVel.x, angVel.y, angVel.z);

			if (ImGui::Button("Reset Velocity"))
			{
				Set_Velocity(XMVectorZero());
				Set_AngularVelocity(XMVectorZero());
			}
		}

		if (m_pActor)
		{
			ImGui::Separator();
			PxTransform pose = m_pActor->getGlobalPose();
			ImGui::Text("Position");
			ImGui::Text("X: %.2f | Y: %.2f | Z: %.2f", pose.p.x, pose.p.y, pose.p.z);

			ImGui::Text("Rotation (Quat)");
			ImGui::Text("X: %.2f | Y: %.2f | Z: %.2f | W: %.2f",
				pose.q.x, pose.q.y, pose.q.z, pose.q.w);

			ImGui::Separator();
			ImGui::Text("Attached Shapes: %d", m_pActor->getNbShapes());
		}
	}
	ImGui::EndChild();
}

PxShape* CRigidBody::Attach_Shape(const PxGeometry& geometry, const string& strMaterialName)
{
	if (!m_pActor) return nullptr;
	PxMaterial* pMaterial = m_pPhysicsSystem->Get_Material(strMaterialName);
	PxShape* pShape = PxRigidActorExt::createExclusiveShape(*m_pActor, geometry, *pMaterial);
	if (pShape)
	{
		Update_Inertia();
	}
	return pShape;
}

void CRigidBody::Add_Force(_fvector vForce, PxForceMode::Enum eMode)
{
	if (m_pActor && !m_bKinematic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->addForce(ToPxVec3(vForce), eMode);
	}
}

void CRigidBody::Add_Torque(_fvector vTorque, PxForceMode::Enum eMode)
{
	if (m_pActor && !m_bKinematic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->addTorque(ToPxVec3(vTorque), eMode);
	}
}

void CRigidBody::Set_Velocity(_fvector vVelocity)
{
	if (m_pActor && !m_bKinematic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->setLinearVelocity(ToPxVec3(vVelocity));
	}
}

void CRigidBody::Set_AngularVelocity(_fvector vAngVelocity)
{
	if (m_pActor && !m_bKinematic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->setAngularVelocity(ToPxVec3(vAngVelocity));
	}
}

_vector CRigidBody::Get_Velocity()
{
	if (m_pActor)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) {
			_vector vVel = ToDxVec(pDynamic->getLinearVelocity());
			return XMVectorSetW(vVel, 0.f);
		}
	}
	return XMVectorZero();
}

_vector CRigidBody::Get_AngularVelocity()
{
	if (m_pActor)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) {
			return XMVectorSetW(ToDxVec(pDynamic->getAngularVelocity()), 0.f);
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

	if (m_pActor)
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
	if (m_pActor)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, m_bKinematic);
	}
}

void CRigidBody::Set_LinearDamping(_float fDamping)
{
	m_fLinearDamping = fDamping;
	if (m_pActor)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->setLinearDamping(m_fLinearDamping);
	}
}

void CRigidBody::Set_AngularDamping(_float fDamping)
{
	m_fAngularDamping = fDamping;
	if (m_pActor)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->setAngularDamping(m_fAngularDamping);
	}
}

void CRigidBody::Wake_Up()
{
	if (m_pActor && !m_bKinematic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->wakeUp();
	}
}

void CRigidBody::Put_ToSleep()
{
	if (m_pActor && !m_bKinematic)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) pDynamic->putToSleep();
	}
}

_bool CRigidBody::Is_Sleeping()
{
	if (m_pActor)
	{
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic) return pDynamic->isSleeping();
	}
	return false;
}

void CRigidBody::Update_RigidBody()
{
	if (!m_pActor) return;

	if (m_bKinematic)	// Transform -> Physics
	{
		_vector vPos = m_pOwnerTransform->Get_WorldPos();
		_smatrix worldMat = m_pOwnerTransform->Get_WorldMatrix();
		_vector vScale, vRot, vTrans;
		XMMatrixDecompose(&vScale, &vRot, &vTrans, worldMat);

		//m_pActor->setKinematicTarget(PxTransform(ToPxVec3(vPos), ToPxQuat(vRot)));
		PxRigidDynamic* pDynamic = m_pActor->is<PxRigidDynamic>();
		if (pDynamic)
			pDynamic->setGlobalPose(PxTransform(ToPxVec3(vPos), ToPxQuat(vRot)));
	}
	else				// Physics -> Transform
	{
		PxTransform globalPose = m_pActor->getGlobalPose();
		m_pOwnerTransform->Set_WorldPos(ToDxVec(globalPose.p));
		m_pOwnerTransform->Set_WorldQuaternion(ToDxQuat(globalPose.q));
	}
}

void CRigidBody::Update_Inertia()
{
	if (!m_pActor) return;
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
		if (m_pActor->getScene())
		{
			m_pActor->getScene()->removeActor(*m_pActor);
		}
		m_pActor->release();
		m_pActor = nullptr;
	}
	__super::Free();
}
#endif
