#include "pch.h"
#include "FieldCharacter.h"

#include "Animator3D.h"
#include "CharacterController.h"

#include "GameInstance.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "ObjectContainer.h"

#include "IInteract.h"

CFieldCharacter::CFieldCharacter(const CFieldCharacter& rhs)
	:CGameObject(rhs)
{
}

void CFieldCharacter::Active_Character()
{
	m_pCCT->Set_CompActive(true);
	SetRenderLayer(RENDER_LAYER::Default);
}

void CFieldCharacter::DeActive_Character()
{
	m_pCCT->Set_CompActive(false);
	SetRenderLayer(RENDER_LAYER::None);
}

void CFieldCharacter::Process_RootMotion(_float dt, const ROOTMOTION_DESC& desc)
{
	auto pTransform = Get_Component<CTransform>();
	_vector3 vRootDelta = m_pAnimator->Get_RootBoneMoveDelta();
	_vector4 vQuatDelta = m_pAnimator->Get_RootBoneQuatDelta();
	_vector3 vInputDir = Get_InputDir();

	if ((desc.iModeMask & ENUM(ROOTMOTION_MASK::QUATERNION)) != 0)
	{
		if (desc.fRotateWeight >= 0.99f) pTransform->Add_Quaternion(vQuatDelta);
		else if (desc.fRotateWeight > 0.01f)
		{
			_quaternion qWeighted = _quaternion::Slerp(_quaternion::Identity, vQuatDelta, desc.fRotateWeight);
			pTransform->Add_Quaternion(qWeighted);
		}
	}
	else
	{
		if (vInputDir.Length() > 0.01f)
		{
			vInputDir.Normalize();
			Rotate(vInputDir);
		}
	}

	if ((desc.iModeMask & ENUM(ROOTMOTION_MASK::MOVE)) != 0)
	{
		if (vRootDelta.x != 0.f || vRootDelta.z != 0.f)
		{
			_vector3 vWeightedDelta = vRootDelta * desc.fMoveWeight;
			_quaternion qRot = pTransform->Get_QuaternionRotate();
			m_pCCT->Move_RootMotion(vWeightedDelta, qRot, dt);
		}
	}
	else
	{
		if (vInputDir.Length() > 0.01f)
		{
			vInputDir.Normalize();
			m_pCCT->Move_Direction(vInputDir, desc.fMoveSpeed, dt);
		}
	}
}

void CFieldCharacter::Process_RootMotion(_float dt, _uint iModeMask)
{
	ROOTMOTION_DESC desc;
	desc.iModeMask = iModeMask;
	Process_RootMotion(dt, desc);
}

HRESULT CFieldCharacter::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CAnimator3D>();
	Add_Component<CCharacterController>();
	return S_OK;
}

HRESULT CFieldCharacter::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	m_pAnimator = Get_Component<CAnimator3D>();
	m_pCCT = Get_Component<CCharacterController>();
	Safe_AddRef(m_pAnimator);
	Safe_AddRef(m_pCCT);

	if (pArg == nullptr) return S_OK;
	GAMEOBJECT_DESC* pCharacterDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
	return S_OK;
}

void CFieldCharacter::Awake()
{
	//auto pMaterial = Get_Component<CMaterial>();
	//auto MaterialInstances = pMaterial->Get_MaterialInstances();
	//for (auto& Instance : MaterialInstances)
	//{
	//	pMaterial->Add_MaterialData(Instance, "fDissolveProgress", { &m_fDissolveProgress, "float", sizeof(_float) });
	//	pMaterial->Add_MaterialData(Instance, "fDissolveTiling", { &m_fDissolveTiling, "float", sizeof(_float) });
	//}
}

void CFieldCharacter::Priority_Update(_float dt)
{
}

void CFieldCharacter::Update(_float dt)
{
	//Get_Component<CObjectContainer>()->UpdateChild(dt);
	m_pAnimator->Update_Animation(dt);
	m_pCCT->Update(dt);
	if (m_bIsRotating)	Update_Rotation(dt);
}

void CFieldCharacter::Late_Update(_float dt)
{
	m_pCCT->Late_Update(dt);
}

void CFieldCharacter::OnCollisionExit(CGameObject* pOther)
{
}

void CFieldCharacter::OnTriggerEnter(CGameObject* pOther)
{
	if (m_bCanInteract == false) return;
	Process_Interact(pOther);
}

void CFieldCharacter::OnTriggerStay(CGameObject* pOther)
{
	if (m_bCanInteract == false) return;
	Process_Interact(pOther);
}

void CFieldCharacter::OnTriggerExit(CGameObject* pOther)
{

}

void CFieldCharacter::Reset_State()
{
	m_bCanInteract = false;
}

void CFieldCharacter::On_Move(const InputInfo& inputInfo)
{
	if (false == m_bCanMove)	
		return;
	_bool prevResetMove = m_inputInfo.resetMove; 

	m_inputInfo = inputInfo;
	m_inputInfo.resetMove = prevResetMove; 

	if (inputInfo.direction.LengthSquared() > 0.01f)
	{
		_vector3 dir = inputInfo.direction;
		dir.Normalize();
		Rotate(dir);
	}
}

void CFieldCharacter::On_Interact()
{
	m_bCanInteract = true;
}

void CFieldCharacter::Process_Interact(CGameObject* pObject)
{
	auto pInteract = dynamic_cast<IInteract*>(pObject);

	if (pInteract != nullptr)
	{
		pInteract->Interact();
	}
}

_bool CFieldCharacter::Is_OppositeInput() const
{
	if (m_inputInfo.curMoveX == 0 && m_inputInfo.curMoveZ == 0) return false;
	if (m_inputInfo.prevMoveX == 0 && m_inputInfo.prevMoveZ == 0) return false;

	_vector2 vPrev((_float)m_inputInfo.prevMoveX, (_float)m_inputInfo.prevMoveZ);
	_vector2 vCur((_float)m_inputInfo.curMoveX, (_float)m_inputInfo.curMoveZ);
	vPrev.Normalize();
	vCur.Normalize();

	_float fDot = vPrev.Dot(vCur);
	_float fAngle = XMConvertToDegrees(acosf(fDot));

	return fAngle >= TURNBACK_ANGLE_THRESHOLD;
}

void CFieldCharacter::Update_Rotation(_float dt)
{
	_float fSpeed = 20.f;
	if (m_qCurrentRot.Dot(m_qTargetRot) > 0.99f)
	{
		m_pTransform->Set_Quaternion(m_qTargetRot);
		m_bIsRotating = false;
		return;
	}

	m_qCurrentRot = _quaternion::Slerp(m_qCurrentRot, m_qTargetRot, dt * fSpeed);
	m_pTransform->Set_Quaternion(m_qCurrentRot);
}

void CFieldCharacter::Rotate(_vector3 vDirection)
{
	_vector3 dir = vDirection;
	dir.y = 0.f;
	dir.Normalize();
	const _float lenSq = dir.LengthSquared();
	const _float yaw = atan2f(dir.x, dir.z);

	m_qTargetRot = _quaternion::CreateFromAxisAngle(_vector3::Up, yaw);
	m_qCurrentRot = m_pTransform->Get_QuaternionRotate();
	m_bIsRotating = true;
}

void CFieldCharacter::Stop_Rotation()
{
	m_bIsRotating = false;
	m_qTargetRot = m_pTransform->Get_QuaternionRotate();
}

void CFieldCharacter::Free()
{
	__super::Free();
	Safe_Release(m_pAnimator);
	Safe_Release(m_pCCT);
}
