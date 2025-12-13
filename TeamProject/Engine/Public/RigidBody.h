#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CRigidBody final : public CComponent
{
#ifdef USINGPHYSICS 
private:
	CRigidBody();
	CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() override DEFAULT;

public:
	PxRigidActor* Get_Body() { return m_pActor; }

public:																
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(COMPONENT_DESC* pArg) override;
	void	Update(_float dt);

public:
	PxShape*	Attach_Shape(const PxGeometry& geometry, const string& strMaterialName);
	void		Add_Force(_fvector vForce, PxForceMode::Enum eMode = PxForceMode::eFORCE);
	void		Add_Torque(_fvector vTorque, PxForceMode::Enum eMode = PxForceMode::eFORCE);
	void		Set_Velocity(_fvector vVelocity);
	void		Set_AngularVelocity(_fvector vAngVelocity);
	_vector		Get_Velocity();
	void		Set_GlobalPos(_fvector vPos, _fvector vQuat);	// 위치 임의 설정
	void		Set_Gravity(_bool bEnable);
	void		Set_Kinematic(_bool bKinematic);
	void		Set_RotationLock(_bool bLockX, _bool bLockY, _bool bLockZ);
	void		Set_Mass(_float fMass);

private:
	void	Update_RigidBody();
	void	Update_Inertia();
	// DX <-> PX
	PxVec3		ToPxVec3(_fvector vVec);
	PxQuat		ToPxQuat(_fvector vQuat);
	_vector		ToDxVec(const PxVec3& vVec);
	_vector		ToDxQuat(const PxQuat& vQuat);

private:
	class IPhysicsService* m_pPhysicsSystem = { nullptr }; // 시스템 참조
	class CTransform*	   m_pOwnerTransform = { nullptr };

	PxRigidActor*		   m_pActor = { nullptr };         // 물리 객체
	PxMaterial*			   m_pMaterial = { nullptr };
	_bool				   m_bStatic = { false };
	_bool				   m_bKinematic = { false };
	_bool				   m_bGravity = { true };
	_bool				   m_bLockX = { true };
	_bool				   m_bLockY = { false };
	_bool				   m_bLockZ = { true };
	_float				   m_fMass = { 1.0f };

public:
	static CRigidBody* Create();
	virtual CComponent* Clone();
	virtual void Free() override;
#endif
};

NS_END