#include "pch.h"
#include "Character.h"
#include "GameInstance.h"
#include "CamObject.h"

#include "Animator3D.h"
#include "CharacterController.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "ObjectContainer.h"
#include "BoneFollower.h"
#include "Child.h"
#include "IInteract.h"

#include "CharacterAttackCollider.h"
#include "CharacterParryCollider.h"

#include "UI_DamageText.h"
#include "UIDirector.h"


CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
	, m_fMaxHP(rhs.m_fMaxHP)
	, m_fCurrentHP(rhs.m_fCurrentHP)
	, m_fAttackPower(rhs.m_fAttackPower)
	, m_fDefense(rhs.m_fDefense)
	, m_fMoveSpeed(rhs.m_fMoveSpeed)
{
}

void CCharacter::Update_DissolveProgress(_float dt)
{
	m_fDissolveProgress += dt;
}

void CCharacter::Active_Character()
{
	m_pCCT->Set_CompActive(true);
	SetRenderLayer(RENDER_LAYER::Default);
	m_fDissolveProgress = 0.f;
}

void CCharacter::DeActive_Character()
{
	m_pCCT->Set_CompActive(false);
	SetRenderLayer(RENDER_LAYER::None);
	m_fDissolveProgress = 0.f;
	m_iInvincibleCount = 0;
	m_inputInfo = {};
	Reset_State();
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

void CCharacter::Awake()
{
	auto pMaterial = Get_Component<CMaterial>();
	auto MaterialInstances = pMaterial->Get_MaterialInstances();
	for (auto& Instance : MaterialInstances)
	{
		pMaterial->Add_MaterialData(Instance, "vRimLightColor", { &m_vRimLightColor, "float3", sizeof(_float3) });
		pMaterial->Add_MaterialData(Instance, "fRimLightPower", { &m_fRimLightPower, "float", sizeof(_float) });
		pMaterial->Add_MaterialData(Instance, "fDissolveProgress", { &m_fDissolveProgress, "float", sizeof(_float) });
		pMaterial->Add_MaterialData(Instance, "fDissolveTiling", { &m_fDissolveTiling, "float", sizeof(_float) });
	}

	SetRenderLayer(RENDER_LAYER::None);
}

void CCharacter::Priority_Update(_float dt)
{
	if (InputDevice()->Key_Tap('T'))
		m_bTest = !m_bTest;

	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CCharacter::Update(_float dt)
{
	m_pAnimator->Update_Animation(dt);
	m_pCCT->Update(dt);
	Update_Evade(dt);
	if (m_bIsRotating)	Update_Rotation(dt);
	Update_Energy(dt);
	Update_Decibel(dt);
	Update_Invincible(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CCharacter::Late_Update(_float dt)
{
	m_pCCT->Late_Update(dt);
	m_bIsAttack = false;
	m_bIsEvade = false;
	m_bEvadeBuffer = false;

	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

void CCharacter::OnTriggerEnter(CGameObject* pOther)
{
	ICollidable* pCollidable = pOther->Get_Component<ICollidable>();
	if (!pCollidable) return;

	if (pCollidable->Get_Group() == COLLISION_GROUP::INTERACTABLE)
	{
		// Interact 활성화
		UI_ACTION_PRIMARY_DESC desc;
		desc.eMode = UI_ACTION_PRIMARY_MODE::INTERACT;
		EventSystem()->Broadcast<UI_ACTION_PRIMARY_DESC>({ desc });
		if (m_bCanInteract)
		{
			auto pInteract = dynamic_cast<IInteract*>(pOther);

			if (pInteract != nullptr)
			{
				pInteract->Interact();
				m_inputInfo = {};
				Reset_State();
			}
		}
	}

	if (Is_Invincible())	return;

	else if (pCollidable->Get_Group() == COLLISION_GROUP::MONSTER_ATTACK)
	{
		m_vHitPos = pOther->Get_Component<CTransform>()->Dir(STATE::POSITION);
	}
}

void CCharacter::OnTriggerStay(CGameObject* pOther)
{
	if(m_bCanInteract)
	{
		auto pInteract = dynamic_cast<IInteract*>(pOther);

		if (pInteract != nullptr)
		{
			pInteract->Interact();
			m_inputInfo = {};
			Reset_State();
		}
	}
	
	if (Is_Invincible())	return;
	ICollidable* pCollidable = pOther->Get_Component<ICollidable>();
	if (!pCollidable) return;

	else if (pCollidable->Get_Group() == COLLISION_GROUP::MONSTER_ATTACK)
	{
		m_vHitPos = pOther->Get_Component<CTransform>()->Dir(STATE::POSITION);
	}
}

void CCharacter::OnTriggerExit(CGameObject* pOther)
{
	ICollidable* pCollidable = pOther->Get_Component<ICollidable>();
	if (!pCollidable) return;

	if (pCollidable->Get_Group() == COLLISION_GROUP::INTERACTABLE)
	{
		// Interact 비활성화
		UI_ACTION_PRIMARY_DESC desc;
		desc.eMode = UI_ACTION_PRIMARY_MODE::ATTACK;
		EventSystem()->Broadcast<UI_ACTION_PRIMARY_DESC>({ desc });
		
		// Energy, Decibel 
		UI_ACTION_DESC actiondesc;
		actiondesc.eType = UI_ACTION_TYPE::SPECIAL;
		if (m_tEnergy.fCurrentEnergy >= m_tEnergy.fSpecialEnergy)
		{
			actiondesc.eState = UI_ACTION_STATE::AVAILABLE;
		}
		else
		{
			actiondesc.eState = UI_ACTION_STATE::ENABLE;
		}
		EventSystem()->Broadcast<UI_ACTION_DESC>({ actiondesc });

		actiondesc.eType = UI_ACTION_TYPE::ULTIMATE;
		if (m_fCurrentDecibel >= MAX_DECIBEL)
		{
			actiondesc.eState = UI_ACTION_STATE::AVAILABLE;
		}
		else
		{
			actiondesc.eState = UI_ACTION_STATE::ENABLE;
		}
		EventSystem()->Broadcast<UI_ACTION_DESC>({ actiondesc });
	}

}

void CCharacter::On_Move(const InputInfo& inputInfo)
{
	if (!m_bIsMain)
		return;
	//if (m_bCanInteract)
	//	return;
	_bool prevResetMove = m_inputInfo.resetMove;
	m_inputInfo = inputInfo;
	m_inputInfo.resetMove = prevResetMove;

	if (false == m_bCanMove) return;

	if (inputInfo.direction.LengthSquared() > 0.01f)
	{
		_vector3 dir = inputInfo.direction;
		dir.Normalize();
		Rotate(dir);
	}
}

void CCharacter::On_Attack()
{
	m_bIsAttack = true;
}

void CCharacter::On_Evade()
{
	if (!Can_Evade()) return;
	m_bIsEvade = true;
}

void CCharacter::On_Ultimate()
{
	UI_ACTION_DESC desc;
	desc.eType = UI_ACTION_TYPE::ULTIMATE;
	desc.eState = UI_ACTION_STATE::EXECUTING;
	EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
	m_fCurrentDecibel = 0.f;
}

void CCharacter::On_Interact()
{
	m_bCanInteract = true;
}

HRESULT CCharacter::Attach_AttackCollider(ATTACK_COLLIDER_DESC* pDesc)
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();

	if (nullptr == pObjectContainer)	
		return E_FAIL;
	if (nullptr == pDesc) 
		return E_FAIL;
	if (nullptr == pDesc->pOwnerAnimator) 
		return E_FAIL;

	string strLevel = LevelManager()->Get_NowLevelKey();

	RIGIDBODY_DESC rigidDesc{};
	rigidDesc.isKinematic = true;
	rigidDesc.bEnableGravity = false;
	rigidDesc.bLockX = false;
	rigidDesc.bLockZ = true;

	COLLIDER_DESC colliderDesc{};
	colliderDesc.eType = pDesc->eColliderType;
	colliderDesc.eGroup = COLLISION_GROUP::PLAYER_ATTACK;
	colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::MONSTER);
	colliderDesc.bAutoFit = false;
	colliderDesc.vCenter = pDesc->vCenter;
	colliderDesc.vSize = pDesc->vSize;
	colliderDesc.vRotation = pDesc->vRotation;
	colliderDesc.bTrigger = true;

	string strAttackName = pDesc->tagName + "_AttackCollider";

	CGameObject* pAttackCollider = Builder::Create_Object(
		{ G_GlobalLevelKey, "Proto_GameObject_CharacterAttackCollider" })
		.RigidBody(rigidDesc)
		.Collider(colliderDesc)
		.Build(strAttackName);
	if (nullptr == pAttackCollider)
		return E_FAIL;

	_int iAttackColliderIndex = { -1 };
	iAttackColliderIndex = pObjectContainer->Add_Child(pAttackCollider, false);
	pAttackCollider->Get_Component<CBoneFollower>()->Link_Bone(pDesc->pOwnerAnimator, pDesc->tagBone);

	m_AttackColliderIndex.emplace(strAttackName, iAttackColliderIndex);

	return S_OK;
}

HRESULT CCharacter::Attach_ParryCollider()
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();

	if (nullptr == pObjectContainer)
		return E_FAIL;

	string strLevel = LevelManager()->Get_NowLevelKey();

	RIGIDBODY_DESC rigidDesc{};
	rigidDesc.isKinematic = true;
	rigidDesc.bEnableGravity = false;

	COLLIDER_DESC colliderDesc{};
	colliderDesc.eType = COLLIDER_TYPE::SPHERE;
	colliderDesc.eGroup = COLLISION_GROUP::PLAYER_ATTACK;
	colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::MONSTER);
	colliderDesc.bAutoFit = false;
	colliderDesc.vCenter = { 0.f,0.f,0.f };
	colliderDesc.vSize = { 5.f,0.f,0.f };
	colliderDesc.bTrigger = true;

	CGameObject* pParryCollider = Builder::Create_Object(
		{ G_GlobalLevelKey, "Proto_GameObject_CharacterParryCollider" })
		.RigidBody(rigidDesc)
		.Collider(colliderDesc)
		.Build(m_strName + "_ParryCollider");
	if (nullptr == pParryCollider)
		return E_FAIL;

	m_iParryColliderIndex = pObjectContainer->Add_Child(pParryCollider, true);

	return S_OK;
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

void CCharacter::Stop_Rotation()
{
	m_bIsRotating = false;
	m_qTargetRot = m_pTransform->Get_QuaternionRotate();
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

	if (m_iEvadeCount >= m_iEvadeMax)
	{
		m_fEvadeCooldown = EVADE_COOLDOWN;
		m_iEvadeCount = 0;
		m_fEvadeTimer = 0.f;
	}
}

_bool CCharacter::Use_EvadeBuffer()
{
	if (m_bEvadeBuffer)
	{
		m_bEvadeBuffer = false;
		return true;
	}
	return false;
}

_bool CCharacter::Can_Ultimate()
{
	if (m_fCurrentDecibel == MAX_DECIBEL) return true;
	return false;
}

void CCharacter::Active_AttackCollider(const string& strName, _bool bActive)
{
	CCharacterAttackCollider* pCollider = Find_AttackCollider(strName);
	if (nullptr == pCollider)
		return;

	pCollider->Get_Component<CCollider>()->Set_CompActive(bActive);
}

void CCharacter::Begin_AttackCollider(const string& strName, const HitDesc& hitdesc)
{
	if (Is_Active_AttackCollider(strName))
		return;

	CCharacterAttackCollider* pCollider = Find_AttackCollider(strName);
	if (nullptr == pCollider)
		return;

	pCollider->Begin_Attack(hitdesc);
}

void CCharacter::End_AttackCollider(const string& strName)
{
	CCharacterAttackCollider* pCollider = Find_AttackCollider(strName);
	if (nullptr == pCollider)
		return;

	pCollider->End_Attack();
}

void CCharacter::End_AllAttackColliders()
{
	CObjectContainer* pContainer = Get_Component<CObjectContainer>();
	if (!pContainer) return;

	auto& children = pContainer->Get_Children();

	for (auto& Collider : m_AttackColliderIndex)
	{
		if (Collider.second < 0 || Collider.second >= static_cast<_int>(children.size()))
			continue;

		CCharacterAttackCollider* pCollider =
			static_cast<CCharacterAttackCollider*>(children[Collider.second]);
		if (nullptr == pCollider) continue;

		pCollider->End_Attack();
	}
}

_bool CCharacter::Is_Active_AttackCollider(const string& strName)
{
	CCharacterAttackCollider* pCollider = Find_AttackCollider(strName);
	if (nullptr == pCollider)
		return false;

	return pCollider->Get_Component<CCollider>()->Get_CompActive();
}

void CCharacter::Take_Damage(DAMAGE_TYPE eType, _float fDamage)
{
	if (Is_Invincible()) return;

	m_fCurrentHP -= fDamage;
	On_Hit(eType);

	CUI_DamageText::DAMAGE_DESC desc{};
	desc.pos    = m_vHitPos;
	desc.damage = (_int)fDamage;

	UIDirector()->Request_DamageText(&desc);
}

_bool CCharacter::Is_OppositeInput() const
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

_bool CCharacter::Can_Parry()
{
	CCharacterParryCollider* pParry = dynamic_cast<CCharacterParryCollider*>
		(Get_Component<CObjectContainer>()->Get_Children()[m_iParryColliderIndex]);
	if (pParry && pParry->Can_Parry())
	{
		return true;
	}
	return false;
}

void CCharacter::Update_Rotation(_float dt)
{
	if (!m_bCanRotate)	return;
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

void CCharacter::Update_Energy(_float dt)
{
	m_tEnergy.fPrevEnergy = m_tEnergy.fCurrentEnergy;
	if (m_tEnergy.fCurrentEnergy >= MAX_ENERGY)
	{
		m_tEnergy.fCurrentEnergy = MAX_ENERGY;
		m_tEnergy.fPrevEnergy = MAX_ENERGY;
		return;
	}
	if (m_tEnergy.fCurrentEnergy < 0.f)
	{
		m_tEnergy.fCurrentEnergy = 0.f;
		m_tEnergy.fPrevEnergy = 0.f;
	}
	m_tEnergy.fCurrentEnergy += dt;

	if (InputDevice()->Key_Down('P'))
		m_tEnergy.fCurrentEnergy += m_tEnergy.fEnergyWeight * dt * 10.f;
}

void CCharacter::Update_Decibel(_float dt)
{
	m_fPrevDecibel = m_fCurrentDecibel;
	if (m_fCurrentDecibel >= MAX_DECIBEL)
	{
		m_fCurrentDecibel = MAX_DECIBEL;
		m_fPrevDecibel = MAX_DECIBEL;
		return;
	}
	m_fCurrentDecibel += dt * 50.f;

	if (InputDevice()->Key_Tap('U'))
		m_fCurrentDecibel = MAX_DECIBEL;
}

void CCharacter::Update_Invincible(_float dt)
{
	if (m_fInvincibleTimer > 0.f)
		m_fInvincibleTimer -= dt;
}

OBJECT_HANDLE CCharacter::Calculate_Parry()
{
	CCharacterParryCollider* pParry = dynamic_cast<CCharacterParryCollider*>
		(Get_Component<CObjectContainer>()->Get_Children()[m_iParryColliderIndex]);
	_vector3 vPos = Get_WorldPos();
	OBJECT_HANDLE targetHandle;
	_float fMinDist = FLT_MAX;

	/* 몬스터의 트리거 콜라이더로 검사. 이후 부모 오브젝트의 Handle 저장 */
	for (auto iter : pParry->Get_Targets())
	{
		_float fDist = (vPos - iter.Get()->Get_WorldPos()).Length();
		if (fDist >= fMinDist)
			continue;
		fMinDist = fDist;
		targetHandle = iter;
	}

	_vector3 vAttackPos = {};
	_vector3 vAttackLook = {};
	if (targetHandle.isValid())
	{
		vAttackPos = targetHandle.Get()->Get_Component<CCharacterController>()->Get_FootPosition();
		vAttackLook = targetHandle.Get()->Get_Component<CTransform>()->Dir(STATE::LOOK);
	}

	m_vParryPos = vAttackPos + vAttackLook * m_fParryOffset;
	m_vParryPos.y = vAttackPos.y;	// 땅으로 꺼지거나 뜨는현상 방지
	m_vParryLook = vAttackPos - m_vParryPos;
	m_vParryLook.Normalize();
	m_vParryPos.y += 1.f;

	return targetHandle;
}

CCharacterAttackCollider* CCharacter::Find_AttackCollider(const string& strName)
{
	string ColliderName = strName + "_AttackCollider";

	auto iter = m_AttackColliderIndex.find(ColliderName);
	if (iter == m_AttackColliderIndex.end())
		return nullptr;

	return static_cast<CCharacterAttackCollider*>(
		Get_Component<CObjectContainer>()->Get_Children()[iter->second]);
}

void CCharacter::Free()
{
	__super::Free();
	Safe_Release(m_pAnimator);
	Safe_Release(m_pCCT);
}