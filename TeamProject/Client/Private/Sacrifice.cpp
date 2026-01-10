#include "pch.h"
#include "Sacrifice.h"
#include "GameInstance.h"
#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "CharacterController.h"
#include "Helper_Func.h"
#include "SacrificeHand.h"
#include "ObjectContainer.h"
#include "MaterialInstance.h"
#include "BoneFollower.h"
#include "Sacrifice_Laser.h"

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
	pResource->Add_ResourcePath("Monster_SacrificeBringer.model", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Body/Monster_SacrificeBringer.model");
	pResource->Add_ResourcePath("Monster_SacrificeBringer.mat", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Body/Monster_SacrificeBringer.mat");
	pResource->Add_ResourcePath("Monster_SacrificeBringer_Meta.json", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Body/Monster_SacrificeBringer_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Monster_SacrificeBringer.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Monster_SacrificeBringer.mat");

	return S_OK;
}

HRESULT CSacrifice::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pMaterial = Get_Component<CMaterial>();
	auto& materialInstances = pMaterial->Get_MaterialInstances();
	//for (auto& instance : materialInstances)
	//{
	//	instance->Set_Blended(true);
	//	instance->Override_Pass("Blend");
	//}

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Monster_SacrificeBringer.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Monster_SacrificeBringer_Meta.json");
	pAnimator->Set_MotionBone(3); //Bip001
	pAnimator->Resize_Layer(3);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 2);

	auto pCCT = Get_Component<CCharacterController>();

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	auto pModel = Get_Component<CSkeletalModel>();
	m_PartMeshIndices.resize(ENUM(PARTS::END));
	m_PartMeshIndices[ENUM(PARTS::ICE)] = 7;
	m_PartMeshIndices[ENUM(PARTS::WEAPON_AXE)] = 11;
	m_PartMeshIndices[ENUM(PARTS::WEAPON_SWORD)] = 12;
	m_PartMeshIndices[ENUM(PARTS::WEAPON_WHIP)] = 13;
	for (_uint i = 0; i < m_PartMeshIndices.size(); ++i)
		pModel->SetDrawable(m_PartMeshIndices[i], false);

	Get_Component<CMaterial>()->Set_RimLightInfo(_float3(1.f, 0.1f, 0.0), 0.3f);
	CGameInstance::GetInstance()->Get_RenderSystem()->SetRimLightMode(RIMLIGHT::OUTLINE);

	/* Child Object */
	auto pObjectContainer = Get_Component<CObjectContainer>();

	{
		auto pHand = Builder::Create_Object({ "Test_Level","Proto_GameObject_SacrificeHand" })
			.Build("Sacrifice_Hand");
		pHand->Set_Alive(false);
		m_iHandID = pObjectContainer->Add_Child(pHand, false);
	}
	
	{
		auto pAttackSign = Builder::Create_Object({ G_GlobalLevelKey,"Proto_GameObject_AttackSign" })
			.Build("AttackSign");
		pObjectContainer->Add_Child(pAttackSign, false);
		pAttackSign->Get_Component<CBoneFollower>()->Link_Bone(pAnimator, "Bip001 Head");
	}

	{
		auto pLaser = Builder::Create_Object({ "Test_Level","Proto_GameObject_SacrificeLaser" })
			.Build("Laser");
		pObjectContainer->Add_Child(pLaser, false);
		pLaser->Get_Component<CBoneFollower>()->Link_Bone(pAnimator, "LaserBeamInitPoint");
	}

	return S_OK;
}

void CSacrifice::Awake()
{
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
	Get_Component<CObjectContainer>()->UpdateChild(dt);

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Tap('7'))
	{
		//Get_Component<CAnimator3D>()->Change_Animation(1, "SacrificeBringer_Ani_P1_Hit_Shake")
		//	.Loop(false)
		//	.Speed(1.5f)
		//	.LayerBlend(1.f, 1.f, 0.5f, EaseType::None)
		//	.Apply();
		//
		//Get_Component<CAnimator3D>()->Change_Animation(2, "SacrificeBringer_Ani_P1_Hit_Shake")
		//	.Loop(false)
		//	.Speed(1.5f)
		//	.LayerBlend(1.f, 1.f, 1.f, EaseType::None)
		//	.Apply();
	}
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
	pHandTransform->LookAt(vTargetPosition);

	_vector3 vHandLook = pHandTransform->Dir(STATE::LOOK);
	_vector3 vPosition = vTargetPosition - vHandLook * 8.f;
	vPosition.y -= 1.f;

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

void CSacrifice::ActiveLaser()
{
	auto pLaser = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser");
	static_cast<CSacrifice_Laser*>(pLaser)->ActiveLaser();
}

void CSacrifice::DeactiveLaser()
{
	auto pLaser = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser");
	static_cast<CSacrifice_Laser*>(pLaser)->DeactiveLaser();
}

void CSacrifice::Active_AttackSign()
{
	auto pAttackSign = Get_Component<CObjectContainer>()->Find_ObjectByName("AttackSign");
	static_cast<CAttackSign*>(pAttackSign)->Active();
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
