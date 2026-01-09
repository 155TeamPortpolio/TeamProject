#include "pch.h"
#include "Character.h"
#include "GameInstance.h"
#include "CamObject.h"

#include "Animator3D.h"
#include "CharacterController.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "ObjectContainer.h"

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
	, m_fMaxHP(rhs.m_fMaxHP)
	, m_fCurrentHP(rhs.m_fCurrentHP)
	, m_fMaxEnergy(rhs.m_fMaxEnergy)
	, m_fCurrentEnergy(rhs.m_fCurrentEnergy)
	, m_fAttackPower(rhs.m_fAttackPower)
	, m_fDefense(rhs.m_fDefense)
	, m_fMoveSpeed(rhs.m_fMoveSpeed)
{
}

void CCharacter::Process_HP(_float fHP)
{

	Set_HP(fHP);
}

void CCharacter::Process_RootMotion(_float dt, const ROOTMOTION_DESC& desc)
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

void CCharacter::Process_RootMotion(_float dt, _uint iModeMask)
{
	ROOTMOTION_DESC desc;
	desc.iModeMask = iModeMask;
	Process_RootMotion(dt, desc);
}

HRESULT CCharacter::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();
	Add_Component<CAnimator3D>();
	Add_Component<CCharacterController>();
	return S_OK;
}

HRESULT CCharacter::Initialize(INIT_DESC* pArg)
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

void CCharacter::Priority_Update(_float dt)
{
}

void CCharacter::Update(_float dt)
{
	m_pAnimator->Update_Animation(dt);
	m_pCCT->Update(dt);
	Update_Evade(dt);
	if (m_bIsRotating)	Update_Rotation(dt);
}

void CCharacter::Late_Update(_float dt)
{
	m_pCCT->Late_Update(dt);
}

void CCharacter::Rotate(_vector3 vDirection)
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


_bool CCharacter::Can_Evade() const
{
	if (m_fEvadeCooldown > 0.f) return false;
	return true;
}

void CCharacter::Use_Evade()
{
	++m_iEvadeCount;
	m_fEvadeTimer = EVADE_COOLDOWN;

	if (m_iEvadeCount >= EVADE_MAX_COUNT)
	{
		m_fEvadeCooldown = EVADE_COOLDOWN;
		m_iEvadeCount = 0;
		m_fEvadeTimer = 0.f;
	}
}

_bool CCharacter::Is_OppositeInput() const
{
	if (m_input.previousMove.IsZero() || m_input.currentMove.IsZero())
		return false;

	_vector2 vPrev((_float)m_input.previousMove.x, (_float)m_input.previousMove.z);
	_vector2 vCur((_float)m_input.currentMove.x, (_float)m_input.currentMove.z);
	vPrev.Normalize();
	vCur.Normalize();

	_float fDot = vPrev.Dot(vCur);
	_float fAngle = XMConvertToDegrees(acosf(fDot));

	return fAngle >= TURNBACK_ANGLE_THRESHOLD;
}

void CCharacter::Update_Input(_float dt)
{
	m_input.prevDirection = m_input.direction;
	m_input.previous = m_input.current;

	KeyInput key;
	if (InputDevice()->Key_Hold('W'))  key.z += 1;
	if (InputDevice()->Key_Hold('S'))  key.z -= 1;
	if (InputDevice()->Key_Hold('D'))  key.x += 1;
	if (InputDevice()->Key_Hold('A'))  key.x -= 1;

	m_input.current = key;

	if (!key.IsZero())
	{
		if (m_input.lastValid.IsZero() || m_input.bufferTimer <= 0.f)
			m_input.lastValid = key;

		m_input.bufferTimer = KEY_BUFFER_TIME;

		if (key != m_input.currentMove)
		{
			m_input.previousMove = m_input.currentMove;
			m_input.currentMove = key;
		}
	}
	else
	{
		m_input.bufferTimer -= dt;
		if (m_input.bufferTimer < 0.f)
		{
			m_input.bufferTimer = 0.f;
			m_input.lastValid.Reset();
			m_input.previousMove.Reset();
			m_input.currentMove.Reset();
		}
	}

	m_input.direction = {};
	if (!key.IsZero())
	{
		auto cam = CameraManager()->Get_ActiveCam();
		auto camTf = cam->Get_Owner()->Get_Component<CTransform>();
		_vector3 look = camTf->Dir(STATE::LOOK);
		look.y = 0.f;
		look.Normalize();
		_vector3 right = _vector3::Up.Cross(look);
		right.Normalize();
		m_input.direction = look * (float)key.z + right * (float)key.x;
		m_input.direction.Normalize();
	}

	m_bIsAttack = InputDevice()->Mouse_Tap(MOUSE_BTN::LB);
	m_bIsEvade = InputDevice()->Mouse_Tap(MOUSE_BTN::RB) && Can_Evade();
	m_bIsMove = m_input.IsMoving();
	m_bIsInput = m_bIsAttack || m_bIsMove || m_bIsEvade;

	if (InputDevice()->Key_Down(VK_F1)) m_bTest = !m_bTest;
}

void CCharacter::Update_Rotation(_float dt)
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

void CCharacter::Update_Evade(_float dt)
{
	if (m_fEvadeCooldown > 0.f)
	{
		m_fEvadeCooldown -= dt;
		if (m_fEvadeCooldown <= 0.f)
		{
			m_fEvadeCooldown = 0.f;
			m_iEvadeCount = 0;
		}
	}

	if (m_fEvadeTimer > 0.f)
	{
		m_fEvadeTimer -= dt;
		if (m_fEvadeTimer <= 0.f)
		{
			m_fEvadeTimer = 0.f;
			m_iEvadeCount = 0;
		}
	}
}

void CCharacter::Free()
{
	__super::Free();
	Safe_Release(m_pAnimator);
	Safe_Release(m_pCCT);
}