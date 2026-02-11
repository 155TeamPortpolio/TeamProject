#include "pch.h"
#include "Defiler.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "UIDirector.h"

#include "SkeletalModel.h"
#include "Animator3D.h"
#include "Material.h"
#include "CharacterController.h"
#include "ObjectContainer.h"
#include "EffectContainer.h"

#include "StateMachine.h"
#include "Defiler_Control.h"

#include "Engine_Math.h"
#include "MaterialInstance.h"
#include "Texture.h"

#include "MiasmaBlade.h"
#include "MiasmaGrandierJaeger.h"
#include "MiasmaSpawnBall.h"
#include "MiasmaHeavyJaeger.h"
#include "MiasmaDummyUnit.h"
#include "DefilerWeapon.h"
#include "DefilerAxe.h"
#include "DefilerWall.h"

#include "AudioSource.h"

#include "UI_DamageText.h"

CDefiler::CDefiler()
	:CEnemy()
{
}

CDefiler::CDefiler(const CDefiler& rhg)
	:CEnemy(rhg)
{
}

HRESULT CDefiler::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_MiasmaBlade", CMiasmaBlade::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_MiasmaGrandierJaeger", CMiasmaGrandierJaeger::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_MiasmaSpawnBall", CMiasmaSpawnBall::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_MiasmaHeavy", CMiasmaHeavyJaeger::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_MiasmaDummy", CMiasmaDummyUnit::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_DefilerWeapon", CDefilerWeapon::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_DefilerAxe", CDefilerAxe::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_DefilerWall", CDefilerWall::Create());

	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();
	Add_Component<CObjectContainer>();
	Add_Component<CAudioSource>();
	Get_Component<CSkeletalModel>()->Link_Model("Zero_Level", "Defiler_Isolde.model");
	Get_Component<CMaterial>()->Link_Material("Zero_Level", "Defiler_Isolde.mat");
	
	return S_OK;
}

HRESULT CDefiler::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	Get_Component<CAudioSource>()->SoundFolder("Zero_Level","../Bin/Resources/Zero/Enemy/Defiler_Isolde/Sound/");

	m_eEnemyClass = ENEMY_CLASS::BOSS;
	vector<_uint> ProMeshes = Get_Component<CSkeletalModel>()->Hide_MehsByName("Pro");
	vector<_uint> WeaponMeshes = Get_Component<CSkeletalModel>()->Show_MehsByName("Weapon");

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Defiler_Isolde.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Monster_IsoldetheDefiler_Meta.json");
	pAnimator->Resize_Layer(3);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 2);

	auto pCCT = Get_Component<CCharacterController>();

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	if (FAILED(Create_Colliders()))
		return E_FAIL;

	if (FAILED(Initialize_Effects()))
		return E_FAIL;

	Create_UIEnemyStatus("Bip001_Spine2");
	Create_UIBossHUD();
	return S_OK;
}

void CDefiler::Awake()
{
	m_vRimLightColor = _float3(0.378, 0.029, 0.070);
	m_fRimLightPower = 4.f;
	m_fDissolveTilling = 6.f;

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

}
void CDefiler::Priority_Update(_float dt)
{	
	m_PlayerCharacterInfos.clear();
	m_PlayerCharacterInfos = BattleSystem()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::PLAYER);
	ComputeTargetingInfo();

	if (InputDevice()->Key_Tap('F'))
		Control_Summon("Heavy");
}

void CDefiler::Update(_float dt)
{
	ManageGroggy(dt);

	if (!m_BlackBoard.LockTarget) {
		m_BlackBoard.vTargetPos = m_tTargetingInfo.vTargetPos;
		m_BlackBoard.vTargetDir = m_tTargetingInfo.vDirToTarget;
	}

	Update_States(dt);

	auto animatorPtr = Get_Component<CAnimator3D>();
	animatorPtr->Update_Animation(dt);

	Route_AnimEvent(animatorPtr);

	MoveByTraceMode(dt);
	RotateToTarget(dt, 4.f);
	Get_Component<CCharacterController>()->Update(dt);

	m_pStateMachine->Update(dt);
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CDefiler::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
}
void CDefiler::Render_GUI()
{
	ImGui::InputInt("Pattern number", &m_BlackBoard.patternIndex);
	for (auto& pattern : m_BlackBoard.patternTransition)
	{
		ImGui::TextUnformatted(pattern.nextPattern.c_str());
	}

	ImGui::SeparatorText("RimLight");

	_float color[3] = {
		m_vRimLightColor.x,
		m_vRimLightColor.y,
		m_vRimLightColor.z
	};

	if (ImGui::ColorEdit3("RimLightColor", color,
		ImGuiColorEditFlags_Float |
		ImGuiColorEditFlags_DisplayRGB |
		ImGuiColorEditFlags_InputRGB))
	{
		m_vRimLightColor = _float3(color[0], color[1], color[2]);
	}

	ImGui::DragFloat("RimLightPower", &m_fRimLightPower, 0.01f, 0.f, 10.f);

	__super::Render_GUI();
}

void CDefiler::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName)
{
	BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::HIT);
	_float fTakeDamage = fDamage;

	if (m_tStatus.isGroggy)
		fTakeDamage *= 1.5f;
	else
		m_tStatus.iGroggyValue += 2;

	m_tStatus.iNowHP -= fTakeDamage;

	if (0 >= m_tStatus.iNowHP)
		m_tStatus.iNowHP = 0.f;

	DAMAGE_DESC desc{};
	_int damage = Helper::Get_Random_Int(1000, 10000); // 임시

	desc.damage = damage;
	desc.followHandle = Get_Handle();
	desc.followOffset = Calc_WorldOffsetWithBip();
	desc.isEnemy = true;
	desc.charaName = charaName;

	UIDirector()->Request_DamageText(desc);
}

void CDefiler::Change_CollisionMask(_uint iMask)
{
	_uint Mask = Get_Component<CCharacterController>()->Get_CollisionMask();
	Get_Component<CCharacterController>()->Set_CollisionMask(Mask - iMask);
}

void CDefiler::Release_CollisionMask()
{
	Get_Component<CCharacterController>()->Set_CollisionMask(m_BaseMask);
}

void CDefiler::Hide_MeshGroup(const string& mesh)
{
	Get_Component<CSkeletalModel>()->Hide_MehsByName(mesh);
}

void CDefiler::Show_MeshGroup(const string& mesh)
{
	Get_Component<CSkeletalModel>()->Show_MehsByName(mesh);
}

void CDefiler::Set_CCTPos(_vector3 pos)
{
	auto controller= Get_Component<CCharacterController>();
	if (!controller)
		return;
	controller->Set_Position(pos);
}

_float3 CDefiler::Get_BipedPos(const string Bone)
{
	Matrix boneMat = Get_Component<CAnimator3D>()
		->Get_BoneMatrix(CAnimator3D::BoneSpace::COMBINED, Bone);
	Matrix WorldMat = m_pTransform->Get_WorldMatrix();
	_vector3 S, T; _quaternion R;
	(boneMat * WorldMat).Decompose(S, R, T);

	return T;
}

void CDefiler::MoveByTraceMode(_float dt, _float moveScale)
{
	if (m_passDampTime > 0.f)
		m_passDampTime = max(0.f, m_passDampTime - dt);

	auto* animator = Get_Component<CAnimator3D>();
	auto* transform = Get_Component<CTransform>();
	auto* controller = Get_Component<CCharacterController>();

	if (!animator || !transform || !controller || dt <= 0.f)
		return;

	const TraceFlag traceFlags = m_BlackBoard.eTraceFlag;
	const _bool stopAtTarget = HasFlag(traceFlags, TraceFlag::StopAtTarget);
	const _bool allowThrough = HasFlag(traceFlags, TraceFlag::AllowThroughTarget);
	const _bool ignoreTarget = HasFlag(traceFlags, TraceFlag::IgnoreTarget);

	const _vector3    rootDeltaLocal = animator->Get_RootBoneMoveDelta();
	const _quaternion rootQuatLocal = animator->Get_RootBoneQuatDelta();

	/*이동량(y제외)*/
	_vector3 rootDeltaH = rootDeltaLocal;
	rootDeltaH.y = 0.f;

	if (ignoreTarget)
	{
		const _vector3 velocityWorld = rootDeltaH / dt;
		controller->Move_Velocity(velocityWorld, dt);
		m_pTransform->Add_Quaternion(rootQuatLocal);
		return;
	}

	// 타겟 벡터
	const _vector3 nowPos = transform->Get_WorldPos();
	const _vector3 targetPos = m_BlackBoard.vTargetPos;

	_vector3 toTarget = targetPos - nowPos;
	toTarget.y = 0.f;

	const _float distToTarget = toTarget.Length();
	if (distToTarget <= 1e-6f)
		return;

	const _vector3 dirToTarget = toTarget / distToTarget;
	const _float lockDist = 2.f;
	if (distToTarget <= lockDist && stopAtTarget && !allowThrough)
	{
		m_BlackBoard.CurrentDir = dirToTarget;
		m_pTransform->Add_Quaternion(rootQuatLocal);
		return;
	}

	{
		m_BlackBoard.CurrentDir.y = 0.f;
		if (m_BlackBoard.CurrentDir.Length() <= 1e-6f)
			m_BlackBoard.CurrentDir = dirToTarget;
		else
			m_BlackBoard.CurrentDir.Normalize();

		const _float unlockDist = 5.f;

		if (allowThrough)
		{
			if (!m_bDirLockedNear)
			{
				if (distToTarget <= lockDist)
					m_bDirLockedNear = true;
			}
			else
			{
				if (distToTarget >= unlockDist)
					m_bDirLockedNear = false;
			}

			if (!m_bDirLockedNear)
			{
				const _float dampSpeed = 50.f;
				const _float align = m_BlackBoard.CurrentDir.Dot(dirToTarget);
				const _bool blockFlip = (allowThrough && m_passDampTime > 0.f); // "지나간 직후" 구간만
				if (!blockFlip || align > 0.f)
					m_BlackBoard.CurrentDir = Math::DampVector(m_BlackBoard.CurrentDir, dirToTarget, dt, dampSpeed);
			}
		}
		else
		{
			m_bDirLockedNear = false;
			m_BlackBoard.CurrentDir = dirToTarget;
		}

		m_BlackBoard.CurrentDir.y = 0.f;
		if (m_BlackBoard.CurrentDir.Length() > 1e-6f)
			m_BlackBoard.CurrentDir.Normalize();
		else
			m_BlackBoard.CurrentDir = dirToTarget;
	}

	const _vector3 localForward(0.f, 0.f, 1.f);
	/*루트 모션의 전방 진행량*/
	_float moveLenSigned = rootDeltaH.Dot(localForward);

	if (moveLenSigned > 0.f && stopAtTarget && !allowThrough)
		moveLenSigned = min(moveLenSigned, distToTarget);

	const _vector3 moveWorld = m_BlackBoard.CurrentDir * moveLenSigned;

	const _float passed = m_BlackBoard.CurrentDir.Dot(dirToTarget);
	const _bool hasPassedTarget = (passed < 0.f);

	if (allowThrough && hasPassedTarget && m_passDampTime <= 0.f)
		m_passDampTime = 0.2f;

	_float distScale = 1.f;

	if (moveLenSigned > 0.f)
	{
		if (allowThrough && m_passDampTime > 0.f)
			distScale = 0.4f;
		else
			distScale = 1.f + distToTarget * 1.2f; 
	}

	const _vector3 velocityWorld = (moveWorld) * distScale;
	controller->Move_RootMotion(velocityWorld, rootQuatLocal, dt);
	//	controller->Move_Velocity(velocityWorld, dt);
	//	m_pTransform->Add_Quaternion(rootQuatLocal);
}


void CDefiler::RotateToTarget(_float dt, _float rotateSpeed)
{
	
	const _bool ignoreTarget = HasFlag(m_BlackBoard.eTraceFlag, TraceFlag::IgnoreTarget);
	if (ignoreTarget)
		return;

	_vector3 vPosition = m_pTransform->Get_Pos();
	_vector3 vCurrDir = m_pTransform->Dir(STATE::LOOK);
	_vector3 vTargetDir = m_BlackBoard.vTargetDir;
	vCurrDir.Normalize();
	vTargetDir.Normalize();

	if (vCurrDir.Dot(vTargetDir) >= 0.99f)
		return;

	vCurrDir = _vector3::Lerp(vCurrDir, vTargetDir, dt * rotateSpeed);
	m_pTransform->Set_Look(vCurrDir);
}

void CDefiler::Update_States(_float dt)
{

}

void CDefiler::Route_AnimEvent(CAnimator3D* animator)
{

	auto Bus = animator->Get_EventBus();

	for (EVENT_INST& instance : Bus)
	{
		switch (instance.Type)
		{
		case CLIP_EVENT_TYPE::NOTIFY:
			if (instance.Tag == "ParrySign")
				UnleashAttack(CEnemy::ATTACK_SIDE::NONE, true);
			else if (instance.Tag == "EvadeSign")
				UnleashAttack(CEnemy::ATTACK_SIDE::NONE, false);
			else if (instance.Tag == "TargetLockOn")
				m_BlackBoard.LockTarget = true;
			else if (instance.Tag == "TargetLockOff")
				m_BlackBoard.LockTarget = false;
			else if (instance.Tag == "TraceType_TrackTarget")
				m_BlackBoard.TraceType_OnTarget();
			else 
				Controll_Attack(instance.Tag);
			break;

		case CLIP_EVENT_TYPE::SOUND:
			Control_Sound(instance.Tag);
			break;
		}
	}
}

void CDefiler::Control_Sound(const string& event)
{
	Get_Component<CAudioSource>()->Slot(event).Attribute3D(true).Volume(0.3f).Play();
}

void CDefiler::Controll_Attack(const string& event)
{
	auto iter = DefilerAtkData.find(event);
	if (iter == DefilerAtkData.end()) {
		Control_Summon(event);
		return;
	}

	auto AtkData = iter->second;

	HitDesc		HitDesc = {};
	HitDesc.eHitType	=	HIT_TYPE::ONCE;
	HitDesc.eDamageType =	DAMAGE_TYPE::NORMAL;
	HitDesc.fDamage		=	0.f;
	HitDesc.fInterval	=	0.f;
	HitDesc.iMaxCount	=	1;
	m_isOnAttack = AtkData.OnOff;

	if (AtkData.OnOff)
	{
		if (AtkData.AtkEvade == "Evade")
			m_isParryEnable = false;
		if (AtkData.AtkEvade == "Parry")
			m_isParryEnable = true;
	}
	else 
	{
		m_isParryEnable = false;
	}
	SetBattleColliderObject(AtkData.AtkBone, CEnemy::BATTLE_COLTYPE::ATTACK, AtkData.OnOff, HitDesc);
}

void CDefiler::Control_Summon(const string& event)
{
	string levelKey = LevelManager()->Get_NowLevelKey();
	if (event == "Blade") {
		
		m_MiasmaSpawner.Spawn(MiasmaType::Blade, 1, m_tTargetingInfo.vTargetPos, Get_BipedPos("Ctr_M_Prop_01"),
			m_tTargetingInfo.vTargetPos.y, this);
	}
	else if (event == "Grandier") {
		m_MiasmaSpawner.Spawn(MiasmaType::Grandier, 8, m_tTargetingInfo.vTargetPos, Get_BipedPos(),
			m_tTargetingInfo.vTargetPos.y,this);
	}
	else if (event == "Heavy") {
		m_MiasmaSpawner.Spawn(MiasmaType::Heavy, 1, m_tTargetingInfo.vTargetPos, Get_BipedPos("Ctr_M_Weapon_03"),
			m_tTargetingInfo.vTargetPos.y,this);
	}
	else if (event == "Weapon") {
		Hide_MeshGroup(event);
		m_MiasmaSpawner.Spawn(MiasmaType::Weapon, 1, m_tTargetingInfo.vTargetPos, Get_BipedPos("Ctr_M_Prop_01"),
			m_tTargetingInfo.vTargetPos.y,this);
	}
	else if (event == "WeaponShow") {
		Show_MeshGroup("Weapon");
	}
}

void CDefiler::Control_TargetEnable(_bool On)
{
	if (!On) {
		BattleSystem()->ExcludeBattleObject(BATTLE_OBJ_TYPE::MONSTER, this->Get_Handle());
	}
	else {
		BattleSystem()->EnterBattleObject(BATTLE_OBJ_TYPE::MONSTER, this->Get_Handle());
	}
	Get_Component<CCharacterController>()->Set_CompActive(On);
}

void CDefiler::Update_Dissolve(_float dt)
{
	const _float duration = m_Dissolve.fDissolveDuration;

	if (duration <= 0.f)
	{
		m_fDissolveProgress =
			(m_Dissolve.eDissolveState == DefilerDissolve::DISAPPEAR) ? 1.f : 0.f;
		return;
	}

	m_Dissolve.fDissolveElapsedTime =
		min(m_Dissolve.fDissolveElapsedTime + dt, duration);

	_float t = m_Dissolve.fDissolveElapsedTime / duration; 

	switch (m_Dissolve.eDissolveState)
	{
	case DefilerDissolve::DISAPPEAR:
		m_fDissolveProgress = t;
		break;

	case DefilerDissolve::APPEAR:
		m_fDissolveProgress = 1.f - t;
		break;

	default:
		break;
	}
}

void CDefiler::Play_Effect(const string& effectTag, _fvector offsetPosition, _fvector offsetQuaternion, _bool syncTransform)
{
	auto pEffect = Get_Component<CObjectContainer>()->Find_ObjectByName(effectTag);
	if (!pEffect)
		return;

	auto pEffectTransform = pEffect->Get_Component<CTransform>();
	if (syncTransform)
	{
		pEffectTransform->Set_Pos(_vector3(offsetPosition));
		pEffectTransform->Set_Quaternion(offsetQuaternion);
	}
	else
	{
		_smatrix worldMatrix = m_pTransform->Get_WorldMatrix();
		_quaternion worldQuaternion = m_pTransform->Get_QuaternionRotate();

		_vector3 vWorldPosition = _vector3::Transform(offsetPosition, worldMatrix);
		_quaternion localQuaternion(offsetQuaternion);
		localQuaternion *= worldQuaternion;

		pEffectTransform->Set_WorldPos(vWorldPosition);
		pEffectTransform->Set_WorldQuaternion(localQuaternion);
	}

	static_cast<CEffectContainer*>(pEffect)->Play();
}

void CDefiler::Stop_Effect(const string& effectTag)
{
	auto pEffect = Get_Component<CObjectContainer>()->Find_ObjectByName(effectTag);
	if (!pEffect)
		return;

	static_cast<CEffectContainer*>(pEffect)->Stop();
}

CDefiler* CDefiler::Create()
{
	CDefiler* instance = new CDefiler();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CDefiler");
	}

	return instance;
}

CGameObject* CDefiler::Clone(INIT_DESC* pArg)
{
	CDefiler* instance = new CDefiler(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CDefiler");
	}

	return instance;
}

void CDefiler::Free()
{
	__super::Free();
	Safe_Release(m_pStateMachine);
}

HRESULT CDefiler::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CDefiler>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Born");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CDefiler::Initialize_States()
{
	m_pStateMachine->Register_State("Born", CDefilerState_Born::Create());
	m_pStateMachine->Register_State("Idle", CDefilerState_Idle::Create());
	m_pStateMachine->Register_State("Attack", CDefilerState_Attack::Create());

	//m_pStateMachine->Register_State("Walk", CSacrificeState_Walk::Create());
	//m_pStateMachine->Register_State("Hit", CSacrificeState_Hit::Create());
	//m_pStateMachine->Register_State("Evade", CSacrificeState_Evade::Create());
	//m_pStateMachine->Register_State("Death", CSacrificeState_Death::Create());
	//m_pStateMachine->Register_State("ChangePhase", CSacrificeState_ChangePhase::Create());
	//m_pStateMachine->Register_State("Parry", CSacrificeState_Parry::Create());
	//m_pStateMachine->Register_State("Groggy", CSacrificeState_Groggy::Create());

	return S_OK;
}

HRESULT CDefiler::Initialize_Transitions()
{
	/* 태어난 후 -> 강제 IDLE*/
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CDefiler>::CONDITION_ANIMATION_END);

	/* IDLE -> ATK or IDLE -> WALK */
	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CDefiler>::CONDITION_TRIGGER, "Idle_To_Attack");

	// Attack
	m_pStateMachine->Register_AnyStateTransition("Idle",
		CStateMachine<CDefiler>::CONDITION_TRIGGER, "Idle");

	m_pStateMachine->Register_Transition("Idle", "Walk",
		CStateMachine<CDefiler>::CONDITION_TRIGGER, "Idle_To_Walk");

	return S_OK;
}


HRESULT CDefiler::Initialize_Effects()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	auto pObjectContainer = Get_Component<CObjectContainer>();
	Create_AttackSign("Bip001_Head");

	/* Hit Ground */
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_hit_ground0.json")
			.Build("Defiler_HitGround0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect, false);
	}

	/* Normal Slash */
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_slash0.json")
			.Build("Defiler_Slash0_0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_slash0.json")
			.Build("Defiler_Slash0_1");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_slash1.json")
			.Build("Defiler_Slash1_0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_slash2.json")
			.Build("Defiler_Slash2_0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}

	/* Axe Slash */
	{
		_smatrix offsetMatrix = _smatrix::Identity;
		offsetMatrix.Translation(_vector3(-1.f, 0.f, 0.f));

		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_axe_slash0.json")
			.Build("Defiler_Axe_Slash0_0");
		pEffect->Stop();
		pEffect->AttachBone(pAnimator, "Ctr_M_Weapon_01", offsetMatrix);
		pObjectContainer->Add_Child(pEffect, false);
	}

	/* Tail Slash */
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_tail_slash0.json")
			.Build("Defiler_Tail_Slash0_0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}

	return S_OK;
}


HRESULT CDefiler::Create_Colliders()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();
	auto pAnimator = Get_Component<CAnimator3D>();

	/* Weapon */
	{
		BATTLE_COLLIDER_DESC WeaponDesc{};

		WeaponDesc.tagName = "Weapon";
		WeaponDesc.isAttachBone = true;
		WeaponDesc.tagBone = "Ctr_M_Weapon_01";
		WeaponDesc.pOwnerAnimator3D = pAnimator;
		WeaponDesc.eAttackColliderType = COLLIDER_TYPE::BOX;
		WeaponDesc.vAttackSize = _float3{ 3.5f,1.5f,0.5f };

		if (FAILED(AttachBattleColliderObject(&WeaponDesc)))
			return E_FAIL;
	}

	/* Tail */
	{
		BATTLE_COLLIDER_DESC WeaponDesc{};

		WeaponDesc.tagName = "Tail";
		WeaponDesc.isAttachBone = true;
		WeaponDesc.tagBone = "Ctr_M_Tail_011";
		WeaponDesc.pOwnerAnimator3D = pAnimator;
		WeaponDesc.eAttackColliderType = COLLIDER_TYPE::SPHERE;
		WeaponDesc.vAttackSize = _float3{ 2.f,2.f,2.f };

		if (FAILED(AttachBattleColliderObject(&WeaponDesc)))
			return E_FAIL;
	}

	/* Area */
	{
		BATTLE_COLLIDER_DESC WeaponDesc{};

		WeaponDesc.tagName = "Area";
		WeaponDesc.isAttachBone = true;
		WeaponDesc.tagBone = "Bip001";
		WeaponDesc.pOwnerAnimator3D = pAnimator;
		WeaponDesc.eAttackColliderType = COLLIDER_TYPE::SPHERE;
		WeaponDesc.vAttackSize = _float3{ 4.f,4.f,4.f };

		if (FAILED(AttachBattleColliderObject(&WeaponDesc)))
			return E_FAIL;
	}

	return S_OK;
}

_float3 CDefiler::Calc_WorldOffsetWithBip()
{
	_vector4 pos = Get_Position();
	_vector3 BipPos = Get_BipedPos();
	_vector3 WorldPos = { pos.x, pos.y, pos.z };
	return BipPos-WorldPos;
}
