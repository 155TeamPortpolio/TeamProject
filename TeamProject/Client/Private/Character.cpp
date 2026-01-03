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
	if(m_bIsRotating)	Update_Rotation(dt);
}

void CCharacter::Late_Update(_float dt)
{
	m_pCCT->Late_Update(dt);
}

void CCharacter::Rotate(_vector3 vDirection)
{
	_vector3 vDir = vDirection;
	vDir.y = 0.f;
	if (vDir.Length() < 0.001f) return;
	vDir.Normalize();

	_vector3 vUp = _vector3::Up;
	_vector3 vRight = vDir.Cross(vUp);
	vRight.Normalize();

	_smatrix mRot = _smatrix::Identity;
	mRot.Right(vRight);
	mRot.Up(vUp);
	mRot.Forward(vDir);

	m_qTargetRot = _quaternion::CreateFromRotationMatrix(mRot);
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

void CCharacter::Update_Input(_float dt)
{
	int x = 0, z = 0;
	if (KEY->Key_Hold('W'))    z += 1;
	if (KEY->Key_Hold('S'))  z -= 1;
	if (KEY->Key_Hold('D')) x += 1;
	if (KEY->Key_Hold('A'))  x -= 1;

	m_vInputDir = {};

	if (x || z)
	{
		auto cam = CAM->Get_ActiveCam();
		auto camTf = cam->Get_Owner()->Get_Component<CTransform>();

		Vector3 look = camTf->Dir(STATE::LOOK);
		Vector3 right = camTf->Dir(STATE::RIGHT);

		look.y = 0.f;
		right.y = 0.f;

		look.Normalize();
		right.Normalize();

		m_vInputDir = right * (float)x + look * (float)z;
	}

	m_bIsAttack = KEY->Mouse_Tap(MOUSE_BTN::LB);
	m_bIsEvade = KEY->Mouse_Tap(MOUSE_BTN::RB) && Can_Evade();
	m_bIsMove = (m_vInputDir.x != 0.f || m_vInputDir.z != 0.f);
	m_bIsInput = m_bIsAttack || m_bIsMove || m_bIsEvade;

	// 테스트용(상태제어)
	if (KEY->Key_Down(VK_F1))	m_bTest = !m_bTest;
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