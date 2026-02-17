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
#include "BoneFollower.h"
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
#include "WaterWaves.h"

#include "AudioSource.h"

#include "UI_DamageText.h"
#include "WaterWave.h"
#include "CamDirector.h"
#include "UI_EnemyStatus.h"

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
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_Env_Water", CWaterWaves::Create());
	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_WaterWave", CWaterWave::Create());

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
	Get_Component<CCharacterController>()->Set_GravityEnabled(false);
	m_BaseY = _vector3(Get_Component<CCharacterController>()->Get_FootPosition()).y;

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
	Create_MeshPyramid();

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

	ManageGroggy(dt);
	Update_States(dt);

	if (InputDevice()->Key_Tap('H')) {
		SummonWave();
	}
}

void CDefiler::Update(_float dt)
{
	if (!m_BlackBoard.LockTarget) {
		m_BlackBoard.vTargetPos = m_tTargetingInfo.vTargetPos;
		m_BlackBoard.vTargetDir = m_tTargetingInfo.vDirToTarget;
	}

	auto animatorPtr = Get_Component<CAnimator3D>();
	animatorPtr->Update_Animation(dt);


	Route_AnimEvent(animatorPtr);
	
	
	Update_Dissolve(dt);
	Get_Component<CCharacterController>()->Update(dt);
	Get_Component<CObjectContainer>()->UpdateChild(dt);

	Get_Component<CAudioSource>()->Set_AudioPos(Get_BipedPos(), 
		Get_Component<CCharacterController>()->Get_Velocity());
	m_pStateMachine->Update(dt);

	MoveByTraceMode(dt);
	RotateToTarget(dt, 4.f);
}

void CDefiler::Late_Update(_float dt)
{
	if (m_isRecovering && m_tStatus.iNowHP < m_tStatus.iMaxHP) {
		m_tStatus.iNowHP += dt* m_tStatus.iMaxHP;
		if (m_tStatus.iMaxHP <= m_tStatus.iNowHP) {
			m_tStatus.iNowHP = m_tStatus.iMaxHP;
			m_isRecovering = false;
		}
	}

	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CDefiler::Render_GUI()
{
	__super::Render_GUI();
#ifdef _USING_GUI
	Client::DefilerDebugGUI::Render(m_BlackBoard);
#endif
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
void CDefiler::Release_AttackCollider()
{
	for (auto pair : m_BattleColliderChildrenIndex)
	{
		SetBattleColliderObject(pair.first,BATTLE_COLTYPE::ATTACK ,false);
	};

}

void CDefiler::ChainParry(_bool OnStart)
{
	BattleSystem()->SetChainParryToPlayer(OnStart);
}

void CDefiler::HideHUD(_bool hide)
{
	if (m_BoneHUD.isValid())
		m_BoneHUD.Get()->Set_Alive(!hide);
}

void CDefiler::Hide_MeshGroup(const string& mesh)
{
	Get_Component<CSkeletalModel>()->Hide_MehsByName(mesh);
}

void CDefiler::Show_MeshGroup(const string& mesh)
{
	Get_Component<CSkeletalModel>()->Show_MehsByName(mesh);
}

void CDefiler::Set_Alive(_bool alive)
{
	__super::Set_Alive(alive);
	if (m_BoneHUD.isValid())
		m_BoneHUD.Get()->Set_Alive(alive);
}

void CDefiler::Set_CCTPos(_vector3 pos)
{
	auto controller= Get_Component<CCharacterController>();
	if (!controller)
		return;
	pos.y = m_BaseY;
	controller->Set_FootPosition(pos);
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

FOUR_DIR CDefiler::Get_FourDirection()
{
	_vector3 shapePos = Math::NormalizeSafeXZ( Get_BipedPos());              
	_vector3 targetPos = Math::NormalizeSafeXZ(m_tTargetingInfo.vTargetPos); 

	_vector3 toTarget = targetPos - shapePos;
	if (toTarget.LengthSquared() <= 1e-6f)
		return FOUR_DIR::FRONT;

	toTarget.Normalize();

	_vector3 forward = Math::NormalizeSafeXZ(m_pTransform->Dir(STATE::LOOK));
	if (forward.Length() <= 1e-6f) forward = _vector3(0.f, 0.f, 1.f);
	else forward.Normalize();

	_vector3 right = Math::NormalizeSafeXZ(m_pTransform->Dir(STATE::RIGHT));
	if (right.Length() <= 1e-6f) right = _vector3(1.f, 0.f, 0.f);
	else right.Normalize();

	const _float forwardDot = toTarget.Dot(forward); 
	const _float rightDot = toTarget.Dot(right);	

	if (fabsf(forwardDot) >= fabsf(rightDot))
		return (forwardDot >= 0.f) ? FOUR_DIR::FRONT : FOUR_DIR::BACK;
	return (rightDot >= 0.f) ? FOUR_DIR::RIGHT : FOUR_DIR::LEFT;
}

void CDefiler::Parried()
{
	m_tStatus.iGroggyValue += 1;
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

	const _vector3 beforePos = Get_BipedPos();

	const TraceFlag traceFlags = m_BlackBoard.eTraceFlag;
	const _bool stopAtTarget = HasFlag(traceFlags, TraceFlag::StopAtTarget);
	const _bool allowThrough = HasFlag(traceFlags, TraceFlag::AllowThroughTarget);
	const _bool ignoreTarget = HasFlag(traceFlags, TraceFlag::IgnoreTarget);

	const _vector3 rootDeltaLocal = animator->Get_RootBoneMoveDelta();
	const _quaternion rootQuatLocal = animator->Get_RootBoneQuatDelta();

	_vector3 rootDeltaPlanar = rootDeltaLocal;
	rootDeltaPlanar.y = 0.f;

	if (ignoreTarget)
	{
		const _vector3 velocityWorld = rootDeltaPlanar / dt;
		controller->Move_Velocity(velocityWorld, dt);
		m_pTransform->Add_Quaternion(rootQuatLocal);

		// ���� �̵� ���� ĳ��(�����ϸ�)
		if (velocityWorld.Length() > 1e-5f)
		{
			m_lastMoveDir = velocityWorld;
			m_lastMoveDir.y = 0.f;
			if (m_lastMoveDir.Length() > 1e-6f) m_lastMoveDir.Normalize();
			m_hasLastMoveDir = true;
		}

		return;
	}

	const _vector3 nowPos = beforePos;
	const _vector3 targetPos = m_BlackBoard.vTargetPos;

	const _float targetSwitchThreshold = 0.25f;
	if ((targetPos - m_lastTargetPos).Length() > targetSwitchThreshold)
	{
		m_lastTargetPos = targetPos;

		m_hasPassDir = false;
		m_passDampTime = 0.f;
		m_bDirLockedNear = false;

		m_prevPassedTarget = false;
		m_passArmed = false;
	}

	_vector3 toTarget = targetPos - nowPos;
	toTarget.y = 0.f;

	const _float distToTarget = toTarget.Length();

	const _bool hasValidToTarget = (distToTarget > 1e-6f);

	_vector3 dirToTarget = m_BlackBoard.CurrentDir;
	dirToTarget.y = 0.f;

	if (hasValidToTarget)
	{
		dirToTarget = toTarget / distToTarget;
	}
	else
	{
		// Ÿ���� ���ưų� �ʹ� �����
		if (m_hasLastMoveDir) dirToTarget = m_lastMoveDir;
		if (dirToTarget.Length() <= 1e-6f) dirToTarget = _vector3(0.f, 0.f, 1.f);
		if (dirToTarget.Length() > 1e-6f) dirToTarget.Normalize();
	}

	// ���� �Ұ��� ȸ����
	if (distToTarget <= 1.f && !allowThrough)
	{
		m_pTransform->Add_Quaternion(rootQuatLocal);
		return;
	}

	if (distToTarget <= 2.f && !allowThrough)
		return;

	const _float lockDist = 1.f;
	if (distToTarget <= lockDist && stopAtTarget && !allowThrough)
	{
		m_BlackBoard.CurrentDir = dirToTarget;
		m_pTransform->Add_Quaternion(rootQuatLocal);
		return;
	}

	_vector3 currentDir = m_BlackBoard.CurrentDir;
	currentDir.y = 0.f;

	if (currentDir.Length() <= 1e-6f)
		currentDir = dirToTarget;
	else
		currentDir.Normalize();

	_vector3 passTestDir = currentDir;
	if (m_hasLastMoveDir)
		passTestDir = m_lastMoveDir;

	passTestDir.y = 0.f;
	if (passTestDir.Length() > 1e-6f)
		passTestDir.Normalize();
	else
		passTestDir = dirToTarget;

	const _float alongValue = toTarget.Dot(passTestDir);
	const _bool hasPassedTarget = (alongValue < 0.f);

	const _float unlockDist = allowThrough ? 10.f : 4.f;

	if (allowThrough)
	{
		if (!m_passArmed)
		{
			if (distToTarget <= lockDist)
				m_passArmed = true;
		}
		else
		{
			if (distToTarget >= unlockDist)
				m_passArmed = false;
		}
	}
	else
	{
		m_passArmed = false;
	}

	const _bool passEvent = (allowThrough && m_passArmed && hasPassedTarget && !m_prevPassedTarget);
	m_prevPassedTarget = hasPassedTarget;

	if (passEvent)
	{
		m_passDir = passTestDir; // "���� �̵� ����"�� �״�� ����
		m_passDir.y = 0.f;

		if (m_passDir.Length() > 1e-6f) m_passDir.Normalize();
		else m_passDir = _vector3(0.f, 0.f, 1.f);

		m_hasPassDir = true;
		m_passDampTime = 0.85f;
	}

	{
		m_BlackBoard.CurrentDir.y = 0.f;
		if (m_BlackBoard.CurrentDir.Length() <= 1e-6f)
			m_BlackBoard.CurrentDir = dirToTarget;
		else
			m_BlackBoard.CurrentDir.Normalize();

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

			if (m_bDirLockedNear && hasPassedTarget && m_passDampTime <= 0.f)
				m_bDirLockedNear = false;

			const _vector3 desiredDir =
				(hasPassedTarget && m_hasPassDir) ? m_passDir : dirToTarget;

			if (!m_bDirLockedNear)
			{
				const _float dampSpeed = 10.f;
				const _bool blockFlip = (m_passDampTime > 0.f);
				const _float alignValue = m_BlackBoard.CurrentDir.Dot(desiredDir);

				if (!blockFlip || alignValue > 0.f)
					m_BlackBoard.CurrentDir = Math::DampVector(m_BlackBoard.CurrentDir, desiredDir, dt, dampSpeed);
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
	_float moveLenSigned = rootDeltaPlanar.Dot(localForward);

	if (moveLenSigned > 0.f)
		moveLenSigned *= moveScale;

	if (moveLenSigned > 0.f && stopAtTarget && !allowThrough)
		moveLenSigned = min(moveLenSigned, distToTarget);

	_float distScale = 1.f;

	if (moveLenSigned > 0.f)
	{
		const _float farDistance = 3.f;
		const _float maxScale = 4.0f;

		_float farRatio01 = clamp(distToTarget / farDistance, 0.f, 1.f);
		_float accel01 = Math::ApplyEase(EaseType::OutCubic, farRatio01);
		_float accelScale = 1.f + (maxScale - 1.f) * accel01;

		if (stopAtTarget && !allowThrough)
		{
			const _float kneeDistance = 1.4f;
			const _float kneeExponent = 7.f;
			const _float minNearMultiplier = 0.3f;

			_float kneeRatio01 = clamp(distToTarget / kneeDistance, 0.f, 1.f);
			_float kneePowValue = powf(kneeRatio01, kneeExponent);

			_float kneeMultiplier =
				minNearMultiplier + (1.f - minNearMultiplier) * kneePowValue;

			accelScale *= kneeMultiplier;
		}

		if (allowThrough && m_passDampTime > 0.f)
			accelScale *= 1.5f;

		distScale = accelScale;
	}

	_float moveLenFinal = moveLenSigned * distScale;

	if (moveLenFinal > 0.f && stopAtTarget && !allowThrough)
		moveLenFinal = min(moveLenFinal, distToTarget);

	const _vector3 displacementWorld = m_BlackBoard.CurrentDir * moveLenFinal;
	controller->Move_RootMotion(displacementWorld, rootQuatLocal, dt);

	const _vector3 afterPos = Get_BipedPos();
	_vector3 actualDelta = afterPos - beforePos;
	actualDelta.y = 0.f;

	if (actualDelta.Length() > 1e-5f)
	{
		m_lastMoveDir = actualDelta;
		m_lastMoveDir.Normalize();
		m_hasLastMoveDir = true;
	}
}
void CDefiler::RotateToTarget(_float dt, _float rotateSpeed)
{
	const _bool IgnoreRotation = HasFlag(m_BlackBoard.eTraceFlag, TraceFlag::IgnoreRotation);
	if (IgnoreRotation)
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
	if (InputDevice()->Key_Tap('P'))
	{
		m_tStatus.iNowHP = m_tStatus.iMaxHP * 0.02f;
		m_tStatus.iGroggyValue = 99;
	}
	if ("Groggy" != m_pStateMachine->Get_CurrentStateName()
		&& "Death" != m_pStateMachine->Get_CurrentStateName() 
		&& m_tStatus.isGroggy)
		m_pStateMachine->Change_State("Groggy");
}

void CDefiler::Route_AnimEvent(CAnimator3D* animator)
{
	auto Bus = animator->Get_EventBus();

	for (EVENT_INST& instance : Bus)
	{
		switch (instance.Type)
		{
		case CLIP_EVENT_TYPE::NOTIFY:
			if (instance.Tag == "ShowMesh_Pro")
				Show_MeshGroup("Pro");
			else if (instance.Tag == "ParrySign")
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

void CDefiler::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName)
{
	BattleSystem()->HitVFX(eDamageType);

	_float fTakeDamage = fDamage;
	if (m_tStatus.isGroggy)
		fTakeDamage *= 1.5f;
	else
		m_tStatus.iGroggyValue += 1;

	if(!m_isRecovering)
		m_tStatus.iNowHP -= fTakeDamage;

	if (0 >= m_tStatus.iNowHP) {
		m_tStatus.iNowHP = 0.f;
		m_BlackBoard.BloodPhase++;
		if (m_BlackBoard.BloodPhase > 2) {
			m_pStateMachine->Set_Trigger("Death");
		}
		else {
			m_isRecovering = true;
		}
	}

	if (m_pStateMachine->Get_CurrentState()->Get_StateName() == "Idle") {
		FOUR_DIR dir = Get_FourDirection();
		string dirStr = { "Front" };
		if (dir == FOUR_DIR::LEFT || dir == FOUR_DIR::RIGHT)
			dirStr = "Back";
		Get_Component<CAnimator3D>()
			->Set_Animation(1, "Monster_IsoldetheDefiler_Ani_Hit_H_"+dirStr)
			.LayerBlend(.8f, 0.1f, .1f, EaseType::InOutQuint)
			.Loop(false)
			.Apply();
	}
	if (m_pStateMachine->Get_CurrentState()->Get_StateName() == "Groggy"){
		FOUR_DIR dir = Get_FourDirection();
		string dirStr = { "Up" };
		if (dir == FOUR_DIR::LEFT || dir == FOUR_DIR::RIGHT)
			dirStr = "Down";

		Get_Component<CAnimator3D>()
			->Set_Animation(1, "Monster_IsoldetheDefiler_Ani_Stun_Hit_L_Front_"+ dirStr)
			.LayerBlend(.8f, 0.1f, .1f, EaseType::InOutQuint)
			.Loop(false)
			.Apply();
	}

	_vector3 vWorldPosition = Get_BipedPos("Bip001");
	auto pEffect = Builder::Create_Object({ G_GlobalLevelKey,"Proto_GameObject_BasicHitEffect" })
		.Position(vWorldPosition)
		.FromPool()
		.Build("BasicHit");

	if (eDamageType == DAMAGE_TYPE::NORMAL) {
		CameraManager()->AddImpact(ENUM(CamShakeType::HitLight), ENUM(CamZoomType::HitLight));
	}
	else {
		CameraManager()->AddImpact(ENUM(CamShakeType::HitCrit), ENUM(CamZoomType::HitCrit), 1.3);
	}
	ObjectManager()->Add_Object(pEffect, { Get_Level(),"Effect_Layer" });
	Send_DamageText(fDamage, charaName);
}

void CDefiler::Send_DamageText(_float damage, CHARACTER charaName)
{
	DAMAGE_DESC desc{};
	damage = Helper::Get_Random_Int(1000, 10000); // �ӽ�
	desc.damage = damage;
	desc.followHandle = Get_Handle();
	desc.followOffset = Calc_WorldOffsetWithBip();
	desc.isEnemy = true;
	desc.charaName = charaName;
	UIDirector()->Request_DamageText(desc);
}


void CDefiler::ResetAllFlags()
{
	m_isOnAttack = false;
	m_isParryEnable = false;
	m_BlackBoard.LockTarget = false;
	m_BlackBoard.LockRotate = false;
}

void CDefiler::Start_WaveTime()
{
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
	else if (event == "Tsunami") {
		SummonWave();
	}
}

void CDefiler::SummonWave()
{
	/*20�� ����*/
	string nowLevelKey = LevelManager()->Get_NowLevelKey();
	CWaterWave::WaterWaveDesc* waveDesc = new CWaterWave::WaterWaveDesc;

	waveDesc->fFoamAmount = 1.0f;
	waveDesc->fRoughness = 1.0f;
	waveDesc->fTintStrength = 0.f;
	waveDesc->fTsunamiHeight = 50.f;

	waveDesc->vWaterTint = _float3(1.f, 1.f, 1.f);
	waveDesc->fNoiseOffset = _float2(0.f, 0.f);

	waveDesc->fPeakTime = 0.90f;			// 0.82 -> 0.88~0.92 (��ũ�� �ڷ�)
	waveDesc->fRiseWidth = 0.85f;			// 0.55 -> 0.75~1.0 (õõ��/�а� ���)
	waveDesc->fFallWidth = 0.05f;			// 0.12 -> 0.04~0.07 (�޶�)

	waveDesc->fCurlStartRatio = 0.95f;		// 0.878 -> 0.93~0.97 (���� ������ ���缭 ����Ƽ�� �R��)
	waveDesc->fCurlDuration = 0.08f;		// 0.15 -> 0.06~0.10 (������ ª�� ���ϰ�)

	waveDesc->fFadeInEnd = 0.12f;			// 0.08 -> 0.10~0.15 (õõ�� ���� ����)
	waveDesc->fFadeOutStart = 0.995f;		// 0.98 -> 0.99~0.995 (������ ���ܵα�)

	waveDesc->fCurlForward = 45.f;			// 35 -> 40~55
	waveDesc->fMaxCurlAngle = 3.8f;			// 6.456 -> 7.0~9.0 (�� ������)

	CGameObject* WaterWave = 
		Builder::Create_Object({ "Zero_Level", "Proto_GameObject_WaterWave" })
		.Add_ObjDesc(waveDesc)
		.Scale({0.1f,1.f,6.f})
		.Position({ -55.f,-4,0.f })
		.Build("WaterWave1");
	ObjectManager()->Add_Object(WaterWave, { nowLevelKey , "Enemy_Layer" });
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

void CDefiler::Update_Dissolve(_float deltaTime)
{
	const _float durationSeconds = m_Dissolve.fDissolveDuration;

	if (durationSeconds <= 0.f)
	{
		m_fDissolveProgress =
			(m_Dissolve.eDissolveState == DefilerDissolve::DISAPPEAR) ? 1.05f : 0.f;
		return;
	}

	m_Dissolve.fDissolveElapsedTime = min(m_Dissolve.fDissolveElapsedTime + deltaTime, durationSeconds);

	const _float ratio01 = clamp(m_Dissolve.fDissolveElapsedTime / durationSeconds, 0.f, 1.f);

	const _float disappearEndOvershoot = 1.05f;
	const _float appearEndUndershoot = -0.05f;

	switch (m_Dissolve.eDissolveState)
	{
	case DefilerDissolve::DISAPPEAR:
		m_fDissolveProgress = Math::Lerp(0.f, disappearEndOvershoot, ratio01);
		break;

	case DefilerDissolve::APPEAR:
		m_fDissolveProgress = Math::Lerp(disappearEndOvershoot, appearEndUndershoot, ratio01);
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
	m_pStateMachine->Register_State("Groggy", CDefilerState_Groggy::Create());
	m_pStateMachine->Register_State("Death", CDefilerState_Death::Create());

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
	/* �¾ �� -> ���� IDLE*/
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CDefiler>::CONDITION_ANIMATION_END);

	/* IDLE -> ATK or IDLE -> WALK */
	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CDefiler>::CONDITION_TRIGGER, "Idle_To_Attack");

	// Attack
	m_pStateMachine->Register_AnyStateTransition("Idle",
		CStateMachine<CDefiler>::CONDITION_TRIGGER, "Idle");

	// Attack
	m_pStateMachine->Register_AnyStateTransition("Death",
		CStateMachine<CDefiler>::CONDITION_TRIGGER, "Death");

	return S_OK;
}


HRESULT CDefiler::Initialize_Effects()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	auto pObjectContainer = Get_Component<CObjectContainer>();
	Create_AttackSign("Bip001_Head");

	/* Laser */
	for (_uint i = 0; i < 3; ++i)
	{
		auto pLaser = Builder::Create_Object({ "Zero_Level","Proto_GameObject_DefilerLaser" })
			.Build("Defiler_Laser_" + to_string(i));

		pLaser->Get_Component<CBoneFollower>()->Link_Bone(pAnimator, "Ctr_M_Weapon_01");
		pObjectContainer->Add_Child(pLaser, false);
	}

	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_laser_charge_normal0.json")
			.Build("Defiler_Laser_Charge_Normal");

		_smatrix offsetMatrix = _smatrix::Identity;
		offsetMatrix.Translation(_vector3(2.f, 0.f, 0.f));

		pEffect->Stop();
		pEffect->AttachBone(pAnimator, "Ctr_M_Weapon_01", offsetMatrix);
		pObjectContainer->Add_Child(pEffect, false);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_laser_charge_strong0.json")
			.Build("Defiler_Laser_Charge_Strong");

		_smatrix offsetMatrix = _smatrix::Identity;
		offsetMatrix.Translation(_vector3(2.f, 0.f, 0.f));

		pEffect->Stop();
		pEffect->AttachBone(pAnimator, "Ctr_M_Weapon_01", offsetMatrix);
		pObjectContainer->Add_Child(pEffect, false);
	}

	/* Hit Ground */
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_hit_ground0.json")
			.Build("Defiler_HitGround0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect, false);
	}

	/* Axe Spin*/
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_axe_spin.json")
			.Build("Defiler_Axe_Spin");

		pEffect->Set_Alive(false);
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}

	/* Axe Shot */
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_axe_shot.json")
			.Build("Defiler_Axe_Shot");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect, false);
	}

	/* Normal Slash */
	for (_uint i = 0; i < 2; ++i)
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_slash0.json")
			.Build("Defiler_Slash0_" + to_string(i));
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}
	for (_uint i = 0; i < 2; ++i)
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_slash1.json")
			.Build("Defiler_Slash1_" + to_string(i));
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
	for (_uint i = 0; i < 5; ++i)
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_slash3.json")
			.Build("Defiler_Slash3_" + to_string(i));
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

	/* Dash Trail */
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_dash_trail.json")
			.Build("Defiler_Dash_Trail");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}

	/* Flare, Light */
	{
		_smatrix offsetMatrix = _smatrix::Identity;
		offsetMatrix.Translation(_vector3(0.5f, 0.f, 0.f));

		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_axe_explode.json")
			.Build("Defiler_Axe_Explode0");
		pEffect->Stop();
		pEffect->AttachBone(pAnimator, "Ctr_M_Weapon_01", offsetMatrix);
		pObjectContainer->Add_Child(pEffect, false);
	}
	{
		_smatrix offsetMatrix = _smatrix::Identity;
		offsetMatrix.Translation(_vector3(0.5f, 0.f, 0.f));

		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_axe_charge0.json")
			.Build("Defiler_Axe_Charge0");
		pEffect->Stop();
		pEffect->AttachBone(pAnimator, "Ctr_M_Weapon_01", offsetMatrix);
		pObjectContainer->Add_Child(pEffect, false);
	}
	{
		_smatrix offsetMatrix = _smatrix::Identity;
		offsetMatrix.Translation(_vector3(0.5f, 0.f, 0.f));

		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_axe_light0.json")
			.Build("Defiler_Axe_Light0");
		pEffect->Stop();
		pEffect->AttachBone(pAnimator, "Ctr_M_Weapon_01", offsetMatrix);
		pObjectContainer->Add_Child(pEffect, false);
	}
	{
		_smatrix offsetMatrix = _smatrix::Identity;
		offsetMatrix.Translation(_vector3(0.5f, 0.f, 0.f));

		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_axe_light1.json")
			.Build("Defiler_Axe_Light1");
		pEffect->Stop();
		pEffect->AttachBone(pAnimator, "Ctr_M_Weapon_01", offsetMatrix);
		pObjectContainer->Add_Child(pEffect, false);
	}
	{
		_smatrix offsetMatrix = _smatrix::Identity;
		offsetMatrix.Translation(_vector3(1.f, 0.f, 0.f));

		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_axe_light2.json")
			.Build("Defiler_Axe_Light2");
		pEffect->Stop();
		pEffect->AttachBone(pAnimator, "Ctr_M_Weapon_01", offsetMatrix);
		pObjectContainer->Add_Child(pEffect, false);
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
		WeaponDesc.vAttackSize = _float3{ 4.f,2.5f,2.5f };

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

void CDefiler::Create_UIEnemyStatus(string boneTag)
{

	// 월드 행렬 포인터 
	if (!m_pTransform)
		return;

	const _float4x4* pParentWorld = m_pTransform->Get_WorldMatrix_Ptr();
	if (!pParentWorld)
		return;

	// 본 로컬 행렬 포인터
	const _float4x4* pBoneLocal = Get_Component<CAnimator3D>()->Get_BoneMatrixPtr(CAnimator3D::BoneSpace::COMBINED, boneTag);
	if (!pBoneLocal)
		return;

	// ENEMYSTATUS_DESC 생성
	CUI_EnemyStatus::ENEMYSTATUS_DESC* pDesc = new CUI_EnemyStatus::ENEMYSTATUS_DESC;
	pDesc->pParentWorld = pParentWorld;
	pDesc->pBoneLocal = pBoneLocal;
	pDesc->pMonsterStatus = &m_tStatus;
	pDesc->tOwnerHandle = Get_Handle();

	// EnemyStatus UI 생성
	const string& strLevelKey = LevelManager()->Get_NowLevelKey();
	auto pEnemyStatus = Builder::Create_UIObject({ G_GlobalLevelKey,"Proto_GameObject_EnemyStatus" })
		.Add_UIDesc(pDesc)
		.Build("EnemyStatus");

	// UI Mgr에 등록
	CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pEnemyStatus, strLevelKey);

	m_BoneHUD = pEnemyStatus->Get_Handle();
}
