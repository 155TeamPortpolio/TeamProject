#include "pch.h"
#include "Character.h"
#include "GameInstance.h"

#include "Animator3D.h"
#include "CharacterController.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "ObjectContainer.h"

CCharacter::CCharacter()
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
	, m_fMaxHP(rhs.m_fMaxHP)
	, m_fCurrentHP(rhs.m_fCurrentHP)
	, m_fMaxEnergy(rhs.m_fMaxEnergy)
	, m_fCurrentEnergy(rhs.m_fCurrentEnergy)
	, m_fAttackPower(rhs.m_fAttackPower)
	, m_fDefense(rhs.m_fDefense)
	, m_fMoveSpeed(rhs.m_fMoveSpeed)
	, m_fJumpSpeed(rhs.m_fJumpSpeed)
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
	// Animator
	if(m_pAnimator)
		m_pAnimator->Update_Animation(dt);
	// CCT
	if(m_pCCT)
		m_pCCT->Update(dt);
}

void CCharacter::Late_Update(_float dt)
{
	if(m_pCCT)
		m_pCCT->Late_Update(dt);
}

void CCharacter::Rotate(_vector3 vDirection)
{
	_vector3 vDir = vDirection;
	vDir.y = 0.f;

	if (vDir.Length() < 0.001f)
		return;

	vDir.Normalize();

	_vector3 vUp = _vector3::Up;
	_vector3 vRight = vDir.Cross(vUp);
	vRight.Normalize();

	_smatrix mRot = _smatrix::Identity;
	mRot.Right(vRight);
	mRot.Up(vUp);
	mRot.Forward(vDir);

	_quaternion qRot = _quaternion::CreateFromRotationMatrix(mRot);
	m_pTransform->Set_Quaternion(qRot);
}

void CCharacter::Update_Input(_float dt)
{
	// Process Input
	auto input = CGameInstance::GetInstance()->Get_InputDev();
	m_vInputDir = _vector3(0.f, 0.f, 0.f);
	if (input->Key_Hold(VK_UP))    m_vInputDir.z += 1.f;
	if (input->Key_Hold(VK_DOWN))  m_vInputDir.z -= 1.f;
	if (input->Key_Hold(VK_RIGHT)) m_vInputDir.x += 1.f;
	if (input->Key_Hold(VK_LEFT))  m_vInputDir.x -= 1.f;

	m_bIsMove = (m_vInputDir.x != 0.f || m_vInputDir.z != 0.f);
	m_bIsJump = input->Key_Down('J');
}

void CCharacter::Free()
{
	__super::Free();
	Safe_Release(m_pAnimator);
	Safe_Release(m_pCCT);
}

