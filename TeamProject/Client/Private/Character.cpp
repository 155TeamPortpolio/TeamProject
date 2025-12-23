#include "pch.h"
#include "Character.h"
#include "GameInstance.h"

#include "Animator3D.h"
#include "CharacterController.h"

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
	m_pAnimator = Add_Component<CAnimator3D>();
	m_pCCT = Add_Component<CCharacterController>();
	Safe_AddRef(m_pAnimator);
	Safe_AddRef(m_pCCT);
	return S_OK;
}

HRESULT CCharacter::Initialize(INIT_DESC* pArg)
{
	if (pArg == nullptr)	return S_OK;
	GAMEOBJECT_DESC* pCharacterDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
	return S_OK;
}

void CCharacter::Priority_Update(_float dt)
{
}

void CCharacter::Update(_float dt)
{
	Update_Input(dt);
	// Animator
	m_pAnimator->Update_Animation(dt);
	// CCT
	m_pCCT->Update(dt);
}

void CCharacter::Late_Update(_float dt)
{
	m_pCCT->Late_Update(dt);
}

void CCharacter::Update_Input(_float dt)
{
	// Process Input
	auto input = CGameInstance::GetInstance()->Get_InputDev();
	m_vInputDir = _vector3(0.f, 0.f, 0.f);
	if (input->Key_Hold(VK_UP)) m_vInputDir.z += 1.f;
	if (input->Key_Hold(VK_DOWN)) m_vInputDir.z -= 1.f;
	if (input->Key_Hold(VK_RIGHT)) m_vInputDir.x += 1.f;
	if (input->Key_Hold(VK_LEFT)) m_vInputDir.x -= 1.f;

	m_bIsJump = input->Key_Down('J');
}

void CCharacter::Free()
{
	__super::Free();
	Safe_Release(m_pAnimator);
	Safe_Release(m_pCCT);
}

