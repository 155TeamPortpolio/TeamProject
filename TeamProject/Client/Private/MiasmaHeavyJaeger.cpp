#include "pch.h"
#include "MiasmaHeavyJaeger.h"

#include "BattleSystem.h"
#include "GameInstance.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"
#include "ObjectContainer.h"
#include "Texture.h"

#include "StateMachine.h"
#include "MiasmaJaegerState.h"

#include "Helper_Func.h"
#include "UIDirector.h"
#include "UI_DamageText.h"

#include "MiasmaProjectile.h"
#include "Character.h"
#include "AudioSource.h"

#include "MiasmaDummyUnit.h"
CMiasmaHeavyJaeger::CMiasmaHeavyJaeger()
	: CEnemy()
{
}

CMiasmaHeavyJaeger::CMiasmaHeavyJaeger(const CMiasmaHeavyJaeger& rhs)
	:CEnemy(rhs)
{
}

HRESULT CMiasmaHeavyJaeger::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>()->Link_Model("Zero_Level", "MiasmaJaeger.model");
	Add_Component<CMaterial>()->Link_Material("Zero_Level", "MiasmaJaeger.mat");
	Add_Component<CCharacterController>();
	Add_Component<CObjectContainer>();
	Add_Component<CAnimator3D>();
	Add_Component<CAudioSource>();

	return S_OK;
}

HRESULT CMiasmaHeavyJaeger::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model("Zero_Level", "MiasmaJaeger.model");
	pAnimator->Link_MetaData("Zero_Level", "MiasmaJaeger_Meta.json");
	pAnimator->Resize_Layer(2);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);
	pAnimator->Set_Animation("HeavyJaeger_Ani_Idle").Loop(true).Apply();

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Hide_MehsByName("Grenadier");

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	if (FAILED(Initialize_Effects()))
		return E_FAIL;

	Get_Component<CAudioSource>()->SoundFolder("Zero_Level", "../Bin/Resources/Zero/Enemy/MiasmaJaeger/Sound/");
	m_vCurrentDir = {0,0,1};

	{
		BATTLE_COLLIDER_DESC WeaponDesc{};

		WeaponDesc.tagName = "Bip001";
		WeaponDesc.isAttachBone = true;
		WeaponDesc.tagBone = "RootNode";
		WeaponDesc.pOwnerAnimator3D = pAnimator;
		WeaponDesc.eAttackColliderType = COLLIDER_TYPE::BOX;
		WeaponDesc.vAttackSize = _float3{ 4.f,2.5f,1.5f };

		if (FAILED(AttachBattleColliderObject(&WeaponDesc)))
			return E_FAIL;
	}
	Get_Component<CCharacterController>()->Set_CompActive(true);
	return S_OK;
}

void CMiasmaHeavyJaeger::Awake()
{
	m_vRimLightColor = _float3(0.127, 0.029, 0.070);
	m_fRimLightPower = 2.2f;
	m_fDissolveTilling = 9.f;

	auto pMaterial = Get_Component<CMaterial>();
	auto& materialInstances = pMaterial->Get_MaterialInstances();
	auto dissolveTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Dissolve.png");

	for (const auto& instance : materialInstances)
	{
		instance->Set_Param("NoiseTexture", { dissolveTexture->Get_SRV(),"Texture2D",0 });
		instance->Set_Param("vRimLightColor", { &m_vRimLightColor,"float3",sizeof(_float3) });
		instance->Set_Param("fRimLightPower", { &m_fRimLightPower,"float",sizeof(_float) });
		instance->Set_Param("fDissolveProgress", { &m_fDissolveProgress,"float",sizeof(_float) });
		instance->Set_Param("fDissolveTiling", { &m_fDissolveTilling,"float",sizeof(_float) });
	}
	;
	m_Dissolve.DisAppear(0.f);


}

void CMiasmaHeavyJaeger::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CMiasmaHeavyJaeger::Update(_float dt)
{
	if (!m_LockedOn) {
		m_PlayerCharacterInfos.clear();
		m_PlayerCharacterInfos = BattleSystem()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::PLAYER);
		ComputeTargetingInfo();
	}

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->Update_Animation(dt);
	Route_AnimEvent(pAnimator);
	m_pStateMachine->Update(dt);
	RotateToTarget(dt, 6.f);
	MoveByAnim(dt, 1.f);
	Get_Component<CCharacterController>()->Update(dt);
	Get_Component<CObjectContainer>()->UpdateChild(dt);
	Update_Dissolve(dt);
}

void CMiasmaHeavyJaeger::Late_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CMiasmaHeavyJaeger::Render_GUI()
{
	__super::Render_GUI();
}

void CMiasmaHeavyJaeger::OnPooledAcquire(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model("Zero_Level", "MiasmaJaeger.model");
	pAnimator->Link_MetaData("Zero_Level", "MiasmaJaeger_Meta.json");
	pAnimator->Resize_Layer(2);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);
	pAnimator->Set_Animation("HeavyJaeger_Ani_Idle").Loop(true).Apply();
	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Hide_MehsByName("Grenadier");
	m_vCurrentDir = { 0,0,1 };
	Get_Component<CCharacterController>()->Set_CompActive(true);
}

void CMiasmaHeavyJaeger::OnPooledRelease()
{
	LockOn(false);
	m_isOnAttack = false;
	Get_Component<CCharacterController>()->Set_CompActive(false);
}

CMiasmaHeavyJaeger* CMiasmaHeavyJaeger::Create()
{
	CMiasmaHeavyJaeger* instance = new CMiasmaHeavyJaeger();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CMiasmaHeavyJaeger");
	}

	return instance;
}

CGameObject* CMiasmaHeavyJaeger::Clone(INIT_DESC* pArg)
{
	CMiasmaHeavyJaeger* instance = new CMiasmaHeavyJaeger(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CMiasmaHeavyJaeger");
	}

	return instance;
}

void CMiasmaHeavyJaeger::Free()
{
	__super::Free();
	Safe_Release(m_pStateMachine);
}

void CMiasmaHeavyJaeger::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;
	auto pEnemy = dynamic_cast<CCharacter*>(pOther);
	if (nullptr != pEnemy)
	{
		m_pStateMachine->Set_Trigger("Parried");

	}
}

void CMiasmaHeavyJaeger::Parried()
{
	m_pStateMachine->Set_Trigger("Parried");
}

void CMiasmaHeavyJaeger::SpawnChild()
{
	const string levelKey = LevelManager()->Get_NowLevelKey();
	_vector3 right = {1,0,0};

	for (size_t i = 1; i < 3; i++)
	{
		auto dummy = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_MiasmaDummy" })
			.Position(right*i)
			.Build("MiasmaUDummy");

		Get_Component<CObjectContainer>()->Add_Child(dummy,true);
	}
	for (size_t i = 1; i < 3; i++)
	{
		auto dummy = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_MiasmaDummy" })
			.Position(- right * i)
			.Build("MiasmaUDummy");
		Get_Component<CObjectContainer>()->Add_Child(dummy, true);
	}
}

void CMiasmaHeavyJaeger::Update_Dissolve(_float dt)
{

	if (m_Dissolve.fDissolveElapsedTime < m_Dissolve.fDissolveDuration)
	{
		m_Dissolve.fDissolveElapsedTime += dt;
		_float t = m_Dissolve.fDissolveElapsedTime / m_Dissolve.fDissolveDuration;

		switch (m_Dissolve.eDissolveState)
		{
		case DefilerDissolve::DISAPPEAR:
		{
			m_fDissolveProgress = t;
		}break;
		case DefilerDissolve::DISSOLVE_STATE::APPEAR:
		{
			m_fDissolveProgress = 1.f - t;
		}break;
		case DefilerDissolve::DISSOLVE_STATE::NONE:
			break;
		default:
			break;
		}
	}
	else
	{
		if (DefilerDissolve::DISAPPEAR == m_Dissolve.eDissolveState)
			m_fDissolveProgress = 1.01f;
		else
			m_fDissolveProgress = 0.f;
	}
}

void CMiasmaHeavyJaeger::RotateToTarget(_float dt, _float rotateSpeed)
{
	_vector3 vPosition = m_pTransform->Get_Pos();
	_vector3 vCurrDir = m_pTransform->Dir(STATE::LOOK);
	_vector3 vTargetDir = m_tTargetingInfo.vDirToTarget;
	vCurrDir.Normalize();
	vTargetDir.Normalize();

	if (vCurrDir.Dot(vTargetDir) >= 0.99f)
		return;

	vCurrDir = _vector3::Lerp(vCurrDir, vTargetDir, dt * rotateSpeed);
	m_pTransform->Set_Look(vCurrDir);
}

void CMiasmaHeavyJaeger::Dissolve(_bool appear)
{
	appear ? m_Dissolve.Appear(0.5f) : m_Dissolve.DisAppear(0.5f);

	for (auto child : Get_Children())
	{
		auto* dummy = dynamic_cast<CMiasmaDummyUnit*>(child);
		if (dummy) {
			auto& dissolve = dummy->Get_Dissolve();
			appear? dissolve.Appear(0.5f) :dissolve.DisAppear(0.5f);
		}
	}
}

void CMiasmaHeavyJaeger::Route_AnimEvent(CAnimator3D* animator)
{
	auto Bus = animator->Get_EventBus();

	for (EVENT_INST& instance : Bus)
	{
		switch (instance.Type)
		{
		case CLIP_EVENT_TYPE::NOTIFY:
			if (instance.Tag == "ParrySign")
			{
				LockOn(true);
				m_isParryEnable = true;
				m_isOnAttack = true;
				Active_AttackSign(true);
				HitDesc		HitDesc = {};
				HitDesc.eHitType = HIT_TYPE::ONCE;
				HitDesc.eDamageType = DAMAGE_TYPE::NORMAL;
				HitDesc.fDamage = 0.f;
				HitDesc.fInterval = 0.f;
				HitDesc.iMaxCount = 1;
				SetBattleColliderObject("Bip001", CEnemy::BATTLE_COLTYPE::ATTACK,
					true, HitDesc);
			}
			break;

		case CLIP_EVENT_TYPE::SOUND:
			Get_Component<CAudioSource>()->Slot(instance.Tag).Attribute3D(true).Volume(0.2f).Play();
			break;
		}
	}
}

void CMiasmaHeavyJaeger::MoveByAnim(_float dt, _float moveScale)
{
	auto* animator = Get_Component<CAnimator3D>();
	auto* transform = Get_Component<CTransform>();
	auto* controller = Get_Component<CCharacterController>();

	if (!animator || !transform || !controller || dt <= 0.f)
		return;

	const _vector3    rootDeltaLocal = animator->Get_RootBoneMoveDelta();
	const _quaternion rootQuatLocal = animator->Get_RootBoneQuatDelta();

	/*ÀÌµ¿·®(yÁ¦¿Ü)*/
	_vector3 rootDeltaH = rootDeltaLocal;
	rootDeltaH.y = 0.f;

	// Å¸°Ù º¤ÅÍ
	const _vector3 nowPos = transform->Get_WorldPos();
	const _vector3 targetPos = m_tTargetingInfo.vTargetPos;

	_vector3 toTarget = targetPos - nowPos;
	toTarget.y = 0.f;

	const _float distToTarget = toTarget.Length();
	if (distToTarget <= 1e-6f)
		return;

	const _vector3 dirToTarget = toTarget / distToTarget;
	const _float lockDist = 1e-6f;
	m_vCurrentDir = dirToTarget;
	if (distToTarget <= lockDist)
	{
		m_pTransform->Add_Quaternion(rootQuatLocal);
		return;
	}
	const _vector3 localForward(0.f, 0.f, 1.f);
	_float moveLenSigned = rootDeltaH.Dot(localForward);

	const _vector3 moveWorld = m_vCurrentDir * moveLenSigned;
	const _float passed = m_vCurrentDir.Dot(dirToTarget);
	const _bool hasPassedTarget = (passed < 0.f);


	_float distScale = 1.f;

	if (moveLenSigned > 0.f)
	{
		distScale = 1.f + distToTarget;
	}

	const _vector3 velocityWorld = (moveWorld)*distScale;
	controller->Move_RootMotion(velocityWorld, rootQuatLocal, dt);
}

void CMiasmaHeavyJaeger::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName)
{
	BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::HIT);
}

HRESULT CMiasmaHeavyJaeger::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CMiasmaHeavyJaeger>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Appear");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CMiasmaHeavyJaeger::Initialize_States()
{
	m_pStateMachine->Register_State("Appear", CMiasmaHeavyJaeger_Appear::Create());
	m_pStateMachine->Register_State("Attack1", CMiasmaHeavyJaeger_Attack1::Create());
	m_pStateMachine->Register_State("Attack2", CMiasmaHeavyJaeger_Attack2::Create());
	m_pStateMachine->Register_State("DisAppear", CMiasmaHeavyJaeger_DisAppear::Create());

	return S_OK;
}

HRESULT CMiasmaHeavyJaeger::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Appear", "Attack1",
		CStateMachine<CMiasmaHeavyJaeger>::CONDITION_TRIGGER, "Appear_To_Attack");
	m_pStateMachine->Register_Transition("Attack1", "Attack2",
		CStateMachine<CMiasmaHeavyJaeger>::CONDITION_TRIGGER, "Attack_To_Attack");
	m_pStateMachine->Register_Transition("Attack2", "DisAppear",
		CStateMachine<CMiasmaHeavyJaeger>::CONDITION_TRIGGER, "Attack_To_DisAppear");
	m_pStateMachine->Register_AnyStateTransition("DisAppear",
		CStateMachine<CMiasmaHeavyJaeger>::CONDITION_TRIGGER, "Parried");

	return S_OK;
}

HRESULT CMiasmaHeavyJaeger::Initialize_Effects()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();
	Create_AttackSign("Bip001_Head");
	return S_OK;
}
