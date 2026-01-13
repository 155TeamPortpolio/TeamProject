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

void CCharacter::Update_DissolveProgress(_float dt)
{
	m_fDissolveProgress += dt;
}

void CCharacter::Reset_DissolveProgress()
{
	m_fDissolveProgress = 0.f;
	SetRenderLayer(RENDER_LAYER::None);
}

void CCharacter::Process_HP(_float fHP, UI_STATUS_OWNER owner)
{
	UI_STATUS_DESC desc = {};
	desc.eOwner = owner;
	desc.eType = UI_STATUS_TYPE::HP;
	desc.value.fCurValue = fHP;
	desc.value.fMaxValue = m_fMaxHP;
	EventSystem()->Broadcast<UI_STATUS_DESC>({desc});

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
}

void CCharacter::Update(_float dt)
{
	m_pAnimator->Update_Animation(dt);
	m_pCCT->Update(dt);
	Update_Evade(dt);
	if (m_bIsRotating)	Update_Rotation(dt);
	Update_Gauge(dt);
}

void CCharacter::Late_Update(_float dt)
{
	m_pCCT->Late_Update(dt);
	m_bIsAttack = false;
	m_bIsEvade = false;
	m_bEvadeBuffer = false;
}

void CCharacter::OnCollisionExit(CGameObject* pOther)
{
	//MSG_BOX("Exit");
}

void CCharacter::OnTriggerEnter(CGameObject* pOther)
{
	CCollider* pCollider = pOther->Get_Component<CCollider>();
	if (pCollider->Get_Group() == COLLISION_GROUP::MONSTER_PARRY)
	{
		m_ParryableTargets.insert(pOther);
	}
	//MSG_BOX("OnTriggerEnter");
}

void CCharacter::OnTriggerStay(CGameObject* pOher)
{
	//MSG_BOX("OnTriggerStay");
}

void CCharacter::OnTriggerExit(CGameObject* pOther)
{
	m_ParryableTargets.erase(pOther);
}

void CCharacter::On_Move(const InputInfo& inputInfo)
{
	_bool prevResetMove = m_inputInfo.resetMove;  // 기존 값 백업

	m_inputInfo = inputInfo;
	m_inputInfo.resetMove = prevResetMove;  // 복원

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
	Use_Evade();
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

_bool CCharacter::Use_EvadeBuffer()
{
	if (m_bEvadeBuffer)
	{
		m_bEvadeBuffer = false;
		return true;
	}
	return false;
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

_bool CCharacter::Can_Parry() const
{
	if (m_ParryableTargets.empty())	return false;
	return true;
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

void CCharacter::Update_Gauge(_float dt)
{
	if (m_tGauge.fCurrentGauge >= MAX_SPECIALGAUGE)
	{
		m_tGauge.fCurrentGauge = MAX_SPECIALGAUGE;
		return;
	}
	m_tGauge.fCurrentGauge += m_tGauge.fGaugeWeight * dt;
}

void CCharacter::Free()
{
	__super::Free();
	Safe_Release(m_pAnimator);
	Safe_Release(m_pCCT);
}