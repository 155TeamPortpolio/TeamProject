#include "pch.h"
#include "Sacrifice.h"
#include "GameInstance.h"
#include "ResourceMgr.h"
#include "Helper_Func.h"
#include "Texture.h"

/* Object */
#include "SacrificeHand.h"
#include "Sacrifice_Laser.h"
#include "EffectContainer.h"

/* Component */
#include "CharacterController.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "ObjectContainer.h"
#include "MaterialInstance.h"
#include "BoneFollower.h"

/* States */
#include "StateMachine.h"
#include "SacrificeState_Idle.h"
#include "SacrificeState_Walk.h"
#include "SacrificeState_Attack.h"
#include "SacrificeState_Born.h"
#include "SacrificeState_Hit.h"
#include "SacrificeState_Evade.h"
#include "SacrificeState_Death.h"
#include "SacrificeState_ChangePhase.h"                                                                                                                                                                                                                                                                                                          
#include "SacrificeState_Parry.h"
#include <AttackSign.h>

CSacrifice::CSacrifice()
	:CEnemy()
{
}

CSacrifice::CSacrifice(const CSacrifice& rhg)
	:CEnemy(rhg)
{
}

HRESULT CSacrifice::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();
	Add_Component<CCharacterController>();

	auto pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResource->Add_ResourcePath("SacrificeBringer.model", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Body/SacrificeBringer.model");
	pResource->Add_ResourcePath("SacrificeBringer.mat", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Body/SacrificeBringer.mat");
	pResource->Add_ResourcePath("SacrificeBringer_Meta.json", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Body/SacrificeBringer_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "SacrificeBringer.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "SacrificeBringer.mat");

	/* Pre load 활성화 전까지 잠시 여기서 생성 */
	{
		//==================== Effect =======================

		auto pResource = ResourceManager();

		/* Assets */
		pResource->Add_ResourcePath("test_particle.json", "../Bin/Resources/Effect/Data/test_particle.json");
		pResource->Add_ResourcePath("spawn_smoke.json", "../Bin/Resources/Effect/Data/spawn_smoke.json");
		pResource->Add_ResourcePath("fog.json", "../Bin/Resources/Effect/Data/fog.json");
		pResource->Add_ResourcePath("hit_ground_smoke.json", "../Bin/Resources/Effect/Data/hit_ground_smoke.json");
		pResource->Add_ResourcePath("hit_ground_smoke_strong.json", "../Bin/Resources/Effect/Data/hit_ground_smoke_strong.json");
		pResource->Add_ResourcePath("core.json", "../Bin/Resources/Effect/Data/core.json");
		pResource->Add_ResourcePath("rock_particle.json", "../Bin/Resources/Effect/Data/rock_particle.json");
		pResource->Add_ResourcePath("sacrifice_spark.json", "../Bin/Resources/Effect/Data/sacrifice_spark.json");
		pResource->Add_ResourcePath("sacrifice_hit_ground_flare.json", "../Bin/Resources/Effect/Data/sacrifice_hit_ground_flare.json");
		pResource->Add_ResourcePath("sacrifice_hit_ground_flare_smoke.json", "../Bin/Resources/Effect/Data/sacrifice_hit_ground_flare_smoke.json");
		pResource->Add_ResourcePath("sacrifice_smoke_trail.json", "../Bin/Resources/Effect/Data/sacrifice_smoke_trail.json");
		pResource->Add_ResourcePath("sacrifice_smoke_trail.json", "../Bin/Resources/Effect/Data/sacrifice_smoke_trail.json");
		pResource->Add_ResourcePath("sacrifice_smoke_trail_cone.json", "../Bin/Resources/Effect/Data/sacrifice_smoke_trail_cone.json");
		pResource->Add_ResourcePath("sacrifice_orb.json", "../Bin/Resources/Effect/Data/sacrifice_orb.json");
		pResource->Add_ResourcePath("sacrifice_smoke_slash.json", "../Bin/Resources/Effect/Data/sacrifice_smoke_slash.json");
		pResource->Add_ResourcePath("sacrifice_sword_slash.json", "../Bin/Resources/Effect/Data/sacrifice_sword_slash.json");
		pResource->Add_ResourcePath("sacrifice_axe_slash.json", "../Bin/Resources/Effect/Data/sacrifice_axe_slash.json");
		pResource->Add_ResourcePath("sacrifice_rush_trail.json", "../Bin/Resources/Effect/Data/sacrifice_rush_trail.json");
		pResource->Add_ResourcePath("sacrifice_axe_slash2.json", "../Bin/Resources/Effect/Data/sacrifice_axe_slash2.json");

		/* Textures */
		pResource->Add_ResourcePath("attack_sign.png", "../Bin/Resources/Effect/Texture/attack_sign.png");
		pResource->Add_ResourcePath("Eff_Particle_044.png", "../Bin/Resources/Effect/Texture/Eff_Particle_044.png");
		pResource->Add_ResourcePath("Eff_Smoke_046_LB_01.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_046_LB_01.png");
		pResource->Add_ResourcePath("Eff_Smoke_218.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_218.png");
		pResource->Add_ResourcePath("Eff_Smoke_006.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_006.png");
		pResource->Add_ResourcePath("rock0.png", "../Bin/Resources/Effect/Texture/rock0.png");
		pResource->Add_ResourcePath("lightning10.png", "../Bin/Resources/Effect/Texture/lightning10.png");
		pResource->Add_ResourcePath("lightning7.png", "../Bin/Resources/Effect/Texture/lightning7.png");
		pResource->Add_ResourcePath("Flare_UU_02.png", "../Bin/Resources/Effect/Texture/Flare_UU_02.png");
		pResource->Add_ResourcePath("Eff_Burn_LYX_28.png", "../Bin/Resources/Effect/Texture/Eff_Burn_LYX_28.png");
		pResource->Add_ResourcePath("Eff_Smoke_259.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_259.png");
		pResource->Add_ResourcePath("Eff_MeleeTrail_078_YZ_05.png", "../Bin/Resources/Effect/Texture/Eff_MeleeTrail_078_YZ_05.png");
		pResource->Add_ResourcePath("Dissolve.png", "../Bin/Resources/Effect/Texture/Dissolve.png");
		pResource->Add_ResourcePath("Eff_Noise_243_YZ_01.png", "../Bin/Resources/Effect/Texture/Eff_Noise_243_YZ_01.png");
		pResource->Add_ResourcePath("Eff_Smoke_113.png", "../Bin/Resources/Effect/Texture/Eff_Smoke_113.png");
		pResource->Add_ResourcePath("Eff_MeleeTrail_078_YZ_03.png", "../Bin/Resources/Effect/Texture/Eff_MeleeTrail_078_YZ_03.png");
		pResource->Add_ResourcePath("Dissolve.png", "../Bin/Resources/Effect/Texture/Dissolve.png");

		/* Models */
		pResource->Add_ResourcePath("Smoke_Cone2.model", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Trail/Smoke_Cone2.model");
		pResource->Add_ResourcePath("Smoke_Cone2.mat", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Trail/Smoke_Cone2.mat");
		pResource->Add_ResourcePath("Sacrifice_Orb.model", "../Bin/Resources/Effect/Model/Sacrifice_Orb/Sacrifice_Orb.model");
		pResource->Add_ResourcePath("Sacrifice_Orb.mat", "../Bin/Resources/Effect/Model/Sacrifice_Orb/Sacrifice_Orb.mat");
		pResource->Add_ResourcePath("Sacrifice_Smoke_Slash5.model", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Slash5/Sacrifice_Smoke_Slash5.model");
		pResource->Add_ResourcePath("Sacrifice_Smoke_Slash5.mat", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Slash5/Sacrifice_Smoke_Slash5.mat");
		pResource->Add_ResourcePath("Sacrifice_Smoke_Slash6.model", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Slash6/Sacrifice_Smoke_Slash6.model");
		pResource->Add_ResourcePath("Sacrifice_Smoke_Slash6.mat", "../Bin/Resources/Effect/Model/Sacrifice_Smoke_Slash6/Sacrifice_Smoke_Slash6.mat");
		pResource->Add_ResourcePath("Sacrifice_Sword_Slash2.model", "../Bin/Resources/Effect/Model/Sacrifice_Sword_Slash2/Sacrifice_Sword_Slash2.model");
		pResource->Add_ResourcePath("Sacrifice_Sword_Slash2.mat", "../Bin/Resources/Effect/Model/Sacrifice_Sword_Slash2/Sacrifice_Sword_Slash2.mat");
		pResource->Add_ResourcePath("Sacrifice_Axe_Slash.model", "../Bin/Resources/Effect/Model/Sacrifice_Axe_Slash/Sacrifice_Axe_Slash.model");
		pResource->Add_ResourcePath("Sacrifice_Axe_Slash.mat", "../Bin/Resources/Effect/Model/Sacrifice_Axe_Slash/Sacrifice_Axe_Slash.mat");

	}


	return S_OK;
}

HRESULT CSacrifice::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pMaterial = Get_Component<CMaterial>();

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "SacrificeBringer.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "SacrificeBringer_Meta.json");
	pAnimator->Resize_Layer(3);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 2);

	auto pCCT = Get_Component<CCharacterController>();

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	auto pModel = Get_Component<CSkeletalModel>();
	m_PartMeshIndices.resize(ENUM(PARTS::END));
	m_PartMeshIndices[ENUM(PARTS::ICE)] = 7;
	m_PartMeshIndices[ENUM(PARTS::WEAPON_AXE)] = 8;
	m_PartMeshIndices[ENUM(PARTS::WEAPON_SWORD)] = 9;
	m_PartMeshIndices[ENUM(PARTS::WEAPON_WHIP)] = 10;
	for (_uint i = 0; i < m_PartMeshIndices.size(); ++i)
		pModel->SetDrawable(m_PartMeshIndices[i], false);

	/* Child Object */
	Create_Children();

	if (FAILED(Create_Colliders()))
		return E_FAIL;

	return S_OK;
}

void CSacrifice::Awake()
{
	m_vRimLightColor = _float3(1.f, 0.2f, 0.f);
	m_fRimLightPower = 2.f;
	m_fDissolveTilling = 5.f;

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

void CSacrifice::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CSacrifice::Update(_float dt)
{
	__super::Update(dt);

	Update_States(dt);
	m_pStateMachine->Update(dt);

	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);
}

void CSacrifice::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

void CSacrifice::Render_GUI()
{
	__super::Render_GUI();

	ImGui::Text("Distance to target : %lf", m_tTargetingInfo.fDistance);
}

CSacrifice* CSacrifice::Create()
{
	CSacrifice* instance = new CSacrifice();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CSacrifice");
	}

	return instance;
}

CGameObject* CSacrifice::Clone(INIT_DESC* pArg)
{
	CSacrifice* instance = new CSacrifice(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSacrifice");
	}

	return instance;
}

void CSacrifice::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}

void CSacrifice::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage)
{
	Get_Component<CAnimator3D>()->Set_Animation(1, "SacrificeBringer_Ani_P1_Hit_Stay")
		.LayerBlend(0.8f, 0.f, 0.1f, EaseType::InQuint)
		.Loop(false)
		.Apply();
}

void CSacrifice::RotateToTarget(_float dt, _float rotateSpeed)
{
	_vector3 vPosition = m_pTransform->Get_Pos();
	_vector3 vCurrDir = m_pTransform->Dir(STATE::LOOK);
	_vector3 vTargetDir = m_tTargetingInfo.vDirToTarget;
	vCurrDir.Normalize();
	vTargetDir.Normalize();

	if (vCurrDir.Dot(vTargetDir) >= 0.99f)
		return;

	vCurrDir = _vector3::Lerp(vCurrDir, vTargetDir, dt * rotateSpeed);
	_vector3 vAt = vPosition + vCurrDir;

	m_pTransform->LookAt(vAt);
}

void CSacrifice::MoveByRootMotion(_float dt, _float moveScale)
{
	auto pAnimator = Get_Component<CAnimator3D>();
	auto pCCT = Get_Component<CCharacterController>();

	_vector3 vDeltaMove = pAnimator->Get_RootBoneMoveDelta();
	_vector4 vDeltaQuat = pAnimator->Get_RootBoneQuatDelta();
	_vector4 vQuaternion = m_pTransform->Get_QuaternionRotate();

	Get_Component<CTransform>()->Add_Quaternion(vDeltaQuat);
	pCCT->Move_RootMotion(vDeltaMove * moveScale, vQuaternion, dt);
}

void CSacrifice::ActiveSword()
{
	Get_Component<CSkeletalModel>()->SetDrawable(m_PartMeshIndices[ENUM(PARTS::WEAPON_SWORD)], true);
}

void CSacrifice::DeactiveSword()
{
	Get_Component<CSkeletalModel>()->SetDrawable(m_PartMeshIndices[ENUM(PARTS::WEAPON_SWORD)], false);
}

void CSacrifice::ActiveAxe()
{
	Get_Component<CSkeletalModel>()->SetDrawable(m_PartMeshIndices[ENUM(PARTS::WEAPON_AXE)], true);
}

void CSacrifice::DeactiveAxe()
{
	Get_Component<CSkeletalModel>()->SetDrawable(m_PartMeshIndices[ENUM(PARTS::WEAPON_AXE)], false);
}

void CSacrifice::ActiveWhip()
{
	Get_Component<CSkeletalModel>()->SetDrawable(m_PartMeshIndices[ENUM(PARTS::WEAPON_WHIP)], true);
}

void CSacrifice::DeactiveWhip()
{
	Get_Component<CSkeletalModel>()->SetDrawable(m_PartMeshIndices[ENUM(PARTS::WEAPON_WHIP)], false);
}

void CSacrifice::Idle()
{
	m_RequestIdle = true;
}

void CSacrifice::Evade()
{
	m_pStateMachine->Change_State("Evade");
}

void CSacrifice::ChangePhase()
{
	if (PHASE::PHASE1 == m_eCurrPhase)
		m_pStateMachine->Change_State("ChangePhase");
}

void CSacrifice::Phase1Attack()
{
	auto pHand = Get_Component<CObjectContainer>()->Find_ObjectByName("Sacrifice_Hand");
	static_cast<CSacrificeHand*>(pHand)->Phase1Attack();

	auto pHandTransform = pHand->Get_Component<CTransform>();

	_vector3 vCurrPosition = m_pTransform->Get_Pos();
	_vector3 vTargetPosition = m_tTargetingInfo.vTargetPos;
	pHandTransform->Set_Pos(vCurrPosition);
	_vector3 vDir = vTargetPosition - vCurrPosition;
	vDir.y = 0.f;
	vDir.Normalize();
	pHandTransform->Set_Look(vDir);

	_vector3 vHandLook = pHandTransform->Dir(STATE::LOOK);
	_vector3 vPosition = vTargetPosition - vHandLook * 8.f;

	pHandTransform->Set_Pos(vPosition);
}

void CSacrifice::Phase2Attack()
{
	auto pHand = Get_Component<CObjectContainer>()->Find_ObjectByName("Sacrifice_Hand");
	static_cast<CSacrificeHand*>(pHand)->Phase2Attack();

	_vector3 vPosition = m_pTransform->Get_WorldPos();
	_vector3 vLook = m_pTransform->Dir(STATE::LOOK);
	vPosition += vLook * 8.f;
	_vector4 vQuaternion = m_pTransform->Get_QuaternionRotate();

	auto pHandTransform = pHand->Get_Component<CTransform>();
	pHandTransform->Set_WorldPos(vPosition);
	pHandTransform->Set_Quaternion(vQuaternion);
}

void CSacrifice::OverDrive_Start()
{
	auto pHand = Get_Component<CObjectContainer>()->Find_ObjectByName("Sacrifice_Hand");
	static_cast<CSacrificeHand*>(pHand)->OverDrive_Start();

	_vector3 vPosition = m_pTransform->Get_WorldPos();
	_vector4 vQuaternion = m_pTransform->Get_QuaternionRotate();

	auto pHandTransform = pHand->Get_Component<CTransform>();
	pHandTransform->Set_Pos(vPosition);
	pHandTransform->Set_Quaternion(vQuaternion);
}

void CSacrifice::OverDrive_Attack1()
{
	auto pHand = Get_Component<CObjectContainer>()->Find_ObjectByName("Sacrifice_Hand");
	static_cast<CSacrificeHand*>(pHand)->OverDrive_Attack1();

	_vector3 vPosition = m_pTransform->Get_WorldPos();
	_vector4 vQuaternion = m_pTransform->Get_QuaternionRotate();

	auto pHandTransform = pHand->Get_Component<CTransform>();
	pHandTransform->Set_Pos(vPosition);
	pHandTransform->Set_Quaternion(vQuaternion);
}

void CSacrifice::OverDrive_Attack2()
{
	auto pHand = Get_Component<CObjectContainer>()->Find_ObjectByName("Sacrifice_Hand");
	static_cast<CSacrificeHand*>(pHand)->OverDrive_Attack2();

	_vector3 vPosition = m_pTransform->Get_WorldPos();
	_vector4 vQuaternion = m_pTransform->Get_QuaternionRotate();

	auto pHandTransform = pHand->Get_Component<CTransform>();
	pHandTransform->Set_Pos(vPosition);
	pHandTransform->Set_Quaternion(vQuaternion);
}

void CSacrifice::OverDrive_Attack3()
{
	auto pHand = Get_Component<CObjectContainer>()->Find_ObjectByName("Sacrifice_Hand");
	static_cast<CSacrificeHand*>(pHand)->OverDrive_Attack3();

	_vector3 vPosition = m_pTransform->Get_WorldPos();
	_vector4 vQuaternion = m_pTransform->Get_QuaternionRotate();

	auto pHandTransform = pHand->Get_Component<CTransform>();
	pHandTransform->Set_Pos(vPosition);
	pHandTransform->Set_Quaternion(vQuaternion);
}

void CSacrifice::ActiveLaser(_uint mode)
{
	auto pLaser = Get_Component<CObjectContainer>()->Find_ObjectByName("Sacrifice_Laser");
	static_cast<CSacrifice_Laser*>(pLaser)->ActiveLaser(mode);
	
	HitDesc desc{};
	SetBattleColliderObject("Hand_Laser", BATTLE_COLTYPE::ATTACK, true, desc);
	SetBattleColliderObject("Hand_Laser", BATTLE_COLTYPE::TRIGGER, true, desc);
}

void CSacrifice::DeactiveLaser()
{
	auto pLaser = Get_Component<CObjectContainer>()->Find_ObjectByName("Sacrifice_Laser");
	static_cast<CSacrifice_Laser*>(pLaser)->DeactiveLaser();

	FinishBattleColliderObject("Hand_Laser");
}

void CSacrifice::ActiveEyeLaser()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	auto pLaser0 = pObjectContainer->Find_ObjectByName("Sacrifice_Eye_Laser0");
	static_cast<CSacrifice_Laser*>(pLaser0)->ActiveLaser(2);

	auto pLaser1 = pObjectContainer->Find_ObjectByName("Sacrifice_Eye_Laser1");
	static_cast<CSacrifice_Laser*>(pLaser1)->ActiveLaser(2);

	auto pLaser2 = pObjectContainer->Find_ObjectByName("Sacrifice_Eye_Laser2");
	static_cast<CSacrifice_Laser*>(pLaser2)->ActiveLaser(2);

	HitDesc desc{};

	SetBattleColliderObject("Eye_Laser0", BATTLE_COLTYPE::ATTACK, true, desc);
	SetBattleColliderObject("Eye_Laser0", BATTLE_COLTYPE::TRIGGER, true, desc);

	SetBattleColliderObject("Eye_Laser1", BATTLE_COLTYPE::ATTACK, true, desc);
	SetBattleColliderObject("Eye_Laser1", BATTLE_COLTYPE::TRIGGER, true, desc);

	SetBattleColliderObject("Eye_Laser2", BATTLE_COLTYPE::ATTACK, true, desc);
	SetBattleColliderObject("Eye_Laser2", BATTLE_COLTYPE::TRIGGER, true, desc);
}

void CSacrifice::DeactiveEyeLaser()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	auto pLaser0 = pObjectContainer->Find_ObjectByName("Sacrifice_Eye_Laser0");
	static_cast<CSacrifice_Laser*>(pLaser0)->DeactiveLaser();

	auto pLaser1 = pObjectContainer->Find_ObjectByName("Sacrifice_Eye_Laser1");
	static_cast<CSacrifice_Laser*>(pLaser1)->DeactiveLaser();

	auto pLaser2 = pObjectContainer->Find_ObjectByName("Sacrifice_Eye_Laser2");
	static_cast<CSacrifice_Laser*>(pLaser2)->DeactiveLaser();

	FinishBattleColliderObject("Eye_Laser0");
	FinishBattleColliderObject("Eye_Laser1");
	FinishBattleColliderObject("Eye_Laser2");
}
void CSacrifice::Set_DissolveState(DISSOLVE_STATE state, _float duration)
{
	m_eDissolveState = state;
	m_fDissolveDuration = duration;
	m_fDissolveElapsedTime = 0.f;
	m_fDissolveProgress = 0.f;
}

void CSacrifice::Update_Dissolve(_float dt)
{
	if (m_fDissolveElapsedTime < m_fDissolveDuration)
	{
		m_fDissolveElapsedTime += dt;
		_float t = m_fDissolveElapsedTime / m_fDissolveDuration;

		switch (m_eDissolveState)
		{
		case Client::CSacrifice::DISSOLVE_STATE::DISAPPEAR:
		{
			m_fDissolveProgress = t;
		}break;
		case Client::CSacrifice::DISSOLVE_STATE::APPEAR:
		{
			m_fDissolveProgress = 1.f - t;
		}break;
		case Client::CSacrifice::DISSOLVE_STATE::NONE:
			break;
		default:
			break;
		}
	}
	else
	{
		if (DISSOLVE_STATE::DISAPPEAR == m_eDissolveState)
			m_fDissolveProgress = 1.01f;
		else
			m_fDissolveProgress = 0.f;
	}
}

void CSacrifice::Create_Children()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();
	auto pAnimator = Get_Component<CAnimator3D>();

	Create_AttackSign("Bip001_Head");

	/* Hand */
	{
		auto pHand = Builder::Create_Object({ "Zero_Level","Proto_GameObject_SacrificeHand" })
			.Build("Sacrifice_Hand");
		pHand->Set_Alive(false);
		m_iHandID = pObjectContainer->Add_Child(pHand, false);
	}

	/* Hand Laser */
	{
		auto pLaser = Builder::Create_Object({ "Zero_Level","Proto_GameObject_SacrificeLaser" })
			.Build("Sacrifice_Laser");
		pObjectContainer->Add_Child(pLaser, false);
		pLaser->Get_Component<CBoneFollower>()->Link_Bone(pAnimator, "Ctr_Eye6_05");
	}

	/* Eye Laser */
	{
		_smatrix offsetMatrix = _smatrix::Identity;
		offsetMatrix.Translation(_vector3(-0.8f, 0.2f, 0.f));
	
		for (_uint i = 0; i < 3; ++i)
		{
			string instanceTag = "Sacrifice_Eye_Laser" + to_string(i);
	
			auto pLaser = Builder::Create_Object({ "Zero_Level","Proto_GameObject_SacrificeLaser" })
				.Build(instanceTag);
	
			pObjectContainer->Add_Child(pLaser, false);
			pLaser->Get_Component<CBoneFollower>()->Set_Offset(offsetMatrix);
		}
	
		pObjectContainer->Find_ObjectByName("Sacrifice_Eye_Laser0")->Get_Component<CBoneFollower>()->Link_Bone(pAnimator, "Ctr_WpnEye_01_1");
		pObjectContainer->Find_ObjectByName("Sacrifice_Eye_Laser1")->Get_Component<CBoneFollower>()->Link_Bone(pAnimator, "Ctr_WpnEye_02_1");
		pObjectContainer->Find_ObjectByName("Sacrifice_Eye_Laser2")->Get_Component<CBoneFollower>()->Link_Bone(pAnimator, "Ctr_WpnEye_03_1");
	}

	/* Spark Effect */
	{
		auto pSpark = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_spark.json")
			.Build("Sacrifice_Spark");
		pObjectContainer->Add_Child(pSpark, false);
		auto pSparkBoneFollower = pSpark->Add_Component<CBoneFollower>();
		pSparkBoneFollower->Initialize(nullptr);
		pSparkBoneFollower->Link_Bone(pAnimator, "Ctr_Limbs_03");
	}
}

HRESULT CSacrifice::Create_Colliders()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();
	auto pAnimator = Get_Component<CAnimator3D>();

	/* Right Arm */
	{
		BATTLE_COLLIDER_DESC RightArmDesc{};
	
		RightArmDesc.tagName = "Right_Arm";
		RightArmDesc.isAttachBone = true;
		RightArmDesc.tagBone = "Skn_R_Hand";
		RightArmDesc.pOwnerAnimator3D = pAnimator;
		RightArmDesc.vAttackSize = _float3{1.f,1.f,1.f};
		RightArmDesc.vTriggerSize = _float3{ 3.f,2.f,3.f };
	
		if (FAILED(AttachBattleColliderObject(&RightArmDesc)))
			return E_FAIL;
	}
	
	/* Sword */
	{
		BATTLE_COLLIDER_DESC SwordDesc{};
	
		SwordDesc.tagName = "Sword";
		SwordDesc.isAttachBone = true;
		SwordDesc.tagBone = "Ctr_Wpn_02";
		SwordDesc.vCenter = _float3{ 0.7f,0.f,0.f };
		SwordDesc.pOwnerAnimator3D = pAnimator;
		SwordDesc.vAttackSize = _float3{ 2.f,2.f,2.f };
		SwordDesc.vTriggerSize = _float3{ 3.f,2.f,3.f };
	
		if (FAILED(AttachBattleColliderObject(&SwordDesc)))
			return E_FAIL;
	}
	
	/* Axe */
	{
		BATTLE_COLLIDER_DESC AxeDesc{};
	
		AxeDesc.tagName = "Axe";
		AxeDesc.isAttachBone = true;
		AxeDesc.tagBone = "Ctr_Wpn_03";
		AxeDesc.pOwnerAnimator3D = pAnimator;
		AxeDesc.vCenter = _float3{ 1.f,0.f,0.f };
		AxeDesc.vAttackSize = _float3{ 2.f,2.f,2.f };
		AxeDesc.vTriggerSize = _float3{ 3.f,2.f,3.f };
	
		if (FAILED(AttachBattleColliderObject(&AxeDesc)))
			return E_FAIL;
	}

	/* Whip */
	{
		BATTLE_COLLIDER_DESC WhipDesc{};

		WhipDesc.tagName = "Whip";
		WhipDesc.isAttachBone = true;
		WhipDesc.tagBone = "Skn_Wpn1_06";
		WhipDesc.pOwnerAnimator3D = pAnimator;
		WhipDesc.vAttackSize = _float3{ 2.f,2.f,2.f };
		WhipDesc.vTriggerSize = _float3{ 5.f,5.f,5.f };

		if (FAILED(AttachBattleColliderObject(&WhipDesc)))
			return E_FAIL;
	}

	/* Hand Laser */
	{
		BATTLE_COLLIDER_DESC HandLaserDesc{};

		HandLaserDesc.tagName = "Hand_Laser";
		HandLaserDesc.isAttachBone = true;
		HandLaserDesc.tagBone = "Ctr_Eye6_05";
		HandLaserDesc.pOwnerAnimator3D = pAnimator;
		HandLaserDesc.eAttackColliderType = COLLIDER_TYPE::BOX;
		HandLaserDesc.eTriggerColliderType = COLLIDER_TYPE::BOX;
		HandLaserDesc.vCenter = _float3{ 16.f,0.f,0.f };
		HandLaserDesc.vAttackSize = _float3{ 32.f,2.f,2.f };
		HandLaserDesc.vTriggerSize = _float3{ 32.f,4.f,4.f };

		if (FAILED(AttachBattleColliderObject(&HandLaserDesc)))
			return E_FAIL;
	}

	/* Eye Laser0 */
	{
		BATTLE_COLLIDER_DESC HandLaserDesc{};

		HandLaserDesc.tagName = "Eye_Laser0";
		HandLaserDesc.isAttachBone = true;
		HandLaserDesc.tagBone = "Ctr_WpnEye_01_1";
		HandLaserDesc.pOwnerAnimator3D = pAnimator;
		HandLaserDesc.eAttackColliderType = COLLIDER_TYPE::BOX;
		HandLaserDesc.eTriggerColliderType = COLLIDER_TYPE::BOX;
		HandLaserDesc.vCenter = _float3{ -16.f,0.f,0.f };
		HandLaserDesc.vAttackSize = _float3{ 32.f,2.f,2.f };
		HandLaserDesc.vTriggerSize = _float3{ 32.f,4.f,4.f };

		if (FAILED(AttachBattleColliderObject(&HandLaserDesc)))
			return E_FAIL;
	}
	
	/* Eye Laser1 */
	{
		BATTLE_COLLIDER_DESC HandLaserDesc{};

		HandLaserDesc.tagName = "Eye_Laser1";
		HandLaserDesc.isAttachBone = true;
		HandLaserDesc.tagBone = "Ctr_WpnEye_02_1";
		HandLaserDesc.pOwnerAnimator3D = pAnimator;
		HandLaserDesc.eAttackColliderType = COLLIDER_TYPE::BOX;
		HandLaserDesc.eTriggerColliderType = COLLIDER_TYPE::BOX;
		HandLaserDesc.vCenter = _float3{ -16.f,0.f,0.f };
		HandLaserDesc.vAttackSize = _float3{ 32.f,2.f,2.f };
		HandLaserDesc.vTriggerSize = _float3{ 32.f,4.f,4.f };

		if (FAILED(AttachBattleColliderObject(&HandLaserDesc)))
			return E_FAIL;
	}
	
	/* Eye Laser2 */
	{
		BATTLE_COLLIDER_DESC HandLaserDesc{};

		HandLaserDesc.tagName = "Eye_Laser2";
		HandLaserDesc.isAttachBone = true;
		HandLaserDesc.tagBone = "Ctr_WpnEye_03_1";
		HandLaserDesc.pOwnerAnimator3D = pAnimator;
		HandLaserDesc.eAttackColliderType = COLLIDER_TYPE::BOX;
		HandLaserDesc.eTriggerColliderType = COLLIDER_TYPE::BOX;
		HandLaserDesc.vCenter = _float3{ -16.f,0.f,0.f };
		HandLaserDesc.vAttackSize = _float3{ 32.f,2.f,2.f };
		HandLaserDesc.vTriggerSize = _float3{ 32.f,4.f,4.f };

		if (FAILED(AttachBattleColliderObject(&HandLaserDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSacrifice::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CSacrifice>::Create();
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

HRESULT CSacrifice::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CSacrificeState_Idle::Create());
	m_pStateMachine->Register_State("Walk", CSacrificeState_Walk::Create());
	m_pStateMachine->Register_State("Attack",CSacrificeState_Attack::Create());
	m_pStateMachine->Register_State("Born",CSacrificeState_Born::Create());
	m_pStateMachine->Register_State("Hit",CSacrificeState_Hit::Create());
	m_pStateMachine->Register_State("Evade", CSacrificeState_Evade::Create());
	m_pStateMachine->Register_State("Death", CSacrificeState_Death::Create());
	m_pStateMachine->Register_State("ChangePhase", CSacrificeState_ChangePhase::Create());
	m_pStateMachine->Register_State("Parry", CSacrificeState_Parry::Create());

	return S_OK;
}

HRESULT CSacrifice::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CSacrifice>::CONDITION_ANIMATION_END);

	/* From Idle */
	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CSacrifice>::CONDITION_TRIGGER, "Idle_To_Attack");
	m_pStateMachine->Register_Transition("Idle", "Walk",
		CStateMachine<CSacrifice>::CONDITION_TRIGGER, "Idle_To_Walk");

	/* From Death */
	m_pStateMachine->Register_Transition("Death", "ChangePhase",
		CStateMachine<CSacrifice>::CONDITION_ANIMATION_END);

	/* From Change Phase */
	m_pStateMachine->Register_Transition("ChangePhase", "Idle",
		CStateMachine<CSacrifice>::CONDITION_ANIMATION_END);

	/* From Parry */
	m_pStateMachine->Register_Transition("Parry", "Idle",
		CStateMachine<CSacrifice>::CONDITION_ANIMATION_END);

	return S_OK;
}

void CSacrifice::Update_States(_float dt)
{
	m_fIdleDuration = m_IsOverDriveCharged ? 2.f : 0.2f;

	if (m_RequestIdle)
	{
		m_pStateMachine->Change_State("Idle");
		m_pStateMachine->Reset_Trigger("Idle_To_Attack");
		m_pStateMachine->Reset_Trigger("Idle_To_Walk");
		m_RequestIdle = false;
	}

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Tap('P'))
		m_pStateMachine->Change_State("Death");

	if (PHASE::PHASE2 == m_eCurrPhase && CGameInstance::GetInstance()->Get_InputDev()->Key_Tap('O'))
		m_IsOverDrive = true;

	/* Idle */
	if ("Idle" == m_pStateMachine->Get_CurrentStateName())
	{
		m_fIdleElasedTime += dt;
		if (m_fIdleElasedTime >= m_fIdleDuration)
		{
			_uint iRandIndex = Helper::Get_Random_Int(0, 4);
			if (m_IsOverDrive)
				iRandIndex = 1;
		
			if (0 == iRandIndex)
				m_pStateMachine->Set_Trigger("Idle_To_Walk");
			else
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
		
			m_fIdleElasedTime = 0.f;
		}
	}

}
