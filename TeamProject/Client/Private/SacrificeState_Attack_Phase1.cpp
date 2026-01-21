#include "pch.h"
#include "SacrificeState_Attack_Phase1.h"
#include "Helper_Func.h"
#include "GameInstance.h"

/* Object */
#include "SacrificeHand.h"
#include "EffectContainer.h"
#include "Sacrifice.h"

/* Component */
#include "CharacterController.h"
#include "ObjectContainer.h"
#include "BoneFollower.h"

void CSacrificeState_Attack_Phase1::Enter(CSacrifice* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrifice>::Create();

		Register_States();
		Register_Transitions();
	}

	BuildPattern(pOwner);

	__super::Enter(pOwner);
}

void CSacrificeState_Attack_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (blackBoard.isRequestNext)
	{
		blackBoard.isRequestNext = false;
		blackBoard.isChainOpen = false;

		if (!blackBoard.stateQueue.empty())
		{
			string nextStateTag = blackBoard.stateQueue.front();
			blackBoard.stateQueue.pop_front();

			blackBoard.currentStateTag = nextStateTag;
			m_pSubStateMachine->Change_State(nextStateTag);
		}
	}

	if (blackBoard.isChainOpen && blackBoard.stateQueue.empty())
	{
		_uint iRandIndex = Helper::Get_Random_Int(0, 1);
		if (0 == iRandIndex)
			pOwner->Idle();
		else
			pOwner->Evade();
	}
}

void CSacrificeState_Attack_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Phase1::Register_States()
{
	m_pSubStateMachine->Register_State("Arm_Recover", CSacrificeState_ArmRecover_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack01_Phase1", CSacrificeState_Attack_01_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack02_Phase1", CSacrificeState_Attack_02_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack03_Phase1", CSacrificeState_Attack_03_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack04_1_Phase1", CSacrificeState_Attack_04_1_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack04_2_Phase1", CSacrificeState_Attack_04_2_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack05_Phase1", CSacrificeState_Attack_05_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack06_Phase1", CSacrificeState_Attack_06_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack07_Phase1", CSacrificeState_Attack_07_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack08_Phase1", CSacrificeState_Attack_08_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack09_Phase1", CSacrificeState_Attack_09_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack10_Phase1", CSacrificeState_Attack_10_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack11_Phase1", CSacrificeState_Attack_11_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack12_Phase1", CSacrificeState_Attack_12_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack_Turn_Phase1", CSacrificeState_Attack_Turn_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack_Roar_Phase1", CSacrificeState_Attack_Roar_Phase1::Create());
}

void CSacrificeState_Attack_Phase1::Register_Transitions()
{
}

void CSacrificeState_Attack_Phase1::BuildPattern(CSacrifice* pOwner)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.iPatternCount++;
	TARGETING_INFO& targetInfo = pOwner->GetTargetingInfo();

	blackBoard.stateQueue.clear();

	if (blackBoard.iPatternCount >= 4)
	{
		blackBoard.iPatternCount = 0;
		blackBoard.stateQueue.push_back("Attack_Roar_Phase1");
		blackBoard.stateQueue.push_back("Attack06_Phase1");
	}
	else
	{
		if (targetInfo.fDistance < 8.f)
		{
			_vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
			_vector3 vTargetDir = targetInfo.vDirToTarget;
			if (vLook.Dot(vTargetDir) < 0.f)
			{
				blackBoard.stateQueue.push_back("Attack_Turn_Phase1");
				blackBoard.stateQueue.push_back("Attack02_Phase1");
			}
			else
			{
				_uint iRandIndex = Helper::Get_Random_Int(0, 2);
				switch (iRandIndex)
				{
				case 0:
				{
					blackBoard.stateQueue.push_back("Attack01_Phase1");
					blackBoard.stateQueue.push_back("Attack02_Phase1");
					blackBoard.stateQueue.push_back("Attack08_Phase1");
				}break;
				case 1:
				{
					/* Hand Pattern */
					blackBoard.stateQueue.push_back("Attack10_Phase1");
					blackBoard.stateQueue.push_back("Attack11_Phase1");
					blackBoard.stateQueue.push_back("Attack12_Phase1");
				}break;
				case 2:
				{
					blackBoard.stateQueue.push_back("Attack01_Phase1");
					blackBoard.stateQueue.push_back("Attack02_Phase1");
				}break;
				default:
					break;
				}
			}
		}
		else
		{
			_uint iRandIndex = Helper::Get_Random_Int(0, 3);
			switch (iRandIndex)
			{
			case 0:
			{
				blackBoard.stateQueue.push_back("Attack07_Phase1");
				blackBoard.stateQueue.push_back("Attack02_Phase1");
				blackBoard.stateQueue.push_back("Attack08_Phase1");
			}break;
			case 1:
			{
				blackBoard.stateQueue.push_back("Attack07_Phase1");
				blackBoard.stateQueue.push_back("Attack02_Phase1");
				blackBoard.stateQueue.push_back("Attack03_Phase1");
				blackBoard.stateQueue.push_back("Arm_Recover");
			}break;
			case 2:
			{
				blackBoard.stateQueue.push_back("Attack04_1_Phase1");
				blackBoard.stateQueue.push_back("Attack02_Phase1");
			}break;
			case 3:
			{
				blackBoard.stateQueue.push_back("Attack05_Phase1");
			}break;
			default:
				break;
			}
		}
	}
	blackBoard.stateQueue.clear();
	blackBoard.stateQueue.push_back("Attack05_Phase1");
	blackBoard.stateQueue.push_back("Attack05_Phase1");
	blackBoard.stateQueue.push_back("Attack05_Phase1");
	blackBoard.stateQueue.push_back("Attack05_Phase1");
	blackBoard.stateQueue.push_back("Attack05_Phase1");
	blackBoard.stateQueue.push_back("Attack_Turn_Phase1");
	
	blackBoard.isRequestNext = true;
}

void CSacrificeState_ArmRecover_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Arm_Recover").Loop(false).Speed(1.f).Apply();

}

void CSacrificeState_ArmRecover_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);

	if (m_fAnimProgress >= 0.4f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
}

void CSacrificeState_ArmRecover_Phase1::Exit(CSacrifice* pOwner)
{
}


void CSacrificeState_Attack_01_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_01").Loop(false).Speed(1.2f).Apply();

	HitDesc hitDesc{};
	pOwner->Active_AttackSign();
	pOwner->SetAutoPlayBattleCollider("Right_Arm", 0.2f, 0.6f, hitDesc);
}

void CSacrificeState_Attack_01_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.3f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
	Update_Effects(pOwner);
}

void CSacrificeState_Attack_01_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_01_Phase1::Update_Effects(CSacrifice* pOwner)
{
	auto pTransform = pOwner->Get_Component<CTransform>();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	/* Smoke Sweep Trail */
	if (IsCrossAnimProgress(0.14f))
	{
		auto effect2 = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_sweep_trail.json")
			.Build("HandSweepTrail");
		effect2->AttachBone(pAnimator, "Skn_Finger3_03");

		ObjectManager()->Add_Object(effect2, { pOwner->Get_Level(),"Effect_Layer" });
	}

	/* Smoke Slash */
	if (IsCrossAnimProgress(0.15f))
	{
		auto pTransform = pOwner->Get_Component<CTransform>();
        auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

        _smatrix worldMatrix = pTransform->Get_WorldMatrix();
        _vector3 vWorldPosition = _vector3::Transform(_vector3(0.f, 0.8f, 0.3f), worldMatrix);

        _quaternion localQuaternion = _quaternion(0.f, 0.f, 0.f, 1.f);
        _quaternion worldQuaternion = pTransform->Get_QuaternionRotate();
        localQuaternion *= worldQuaternion;

        auto effect = pObjectContainer->Find_ObjectByName("Sacrifice_Smoke_Slash2");
        auto pEffectTransform = effect->Get_Component<CTransform>();

        pEffectTransform->Set_WorldPos(vWorldPosition);
        pEffectTransform->Set_WorldQuaternion(localQuaternion);

        static_cast<CEffectContainer*>(effect)->Play();
	}
}

void CSacrificeState_Attack_02_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_02").Loop(false).Speed(1.f).Apply();
	
	HitDesc hitDesc{};
	pOwner->Active_AttackSign();
	pOwner->SetAutoPlayBattleCollider("Sword", 0.f, 0.1f, hitDesc);

	m_IsAttackStart = false;
	m_IsAttackEnd = false;
	m_fAnimProgress = 0.f;
}

void CSacrificeState_Attack_02_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.2f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (!m_IsAttackStart && m_fAnimProgress >= 0.05f)
	{
		m_IsAttackStart = true;
		pOwner->ActiveSword();
	}

	if (!m_IsAttackEnd && m_fAnimProgress >= 0.15f)
	{
		m_IsAttackEnd = true;
		pOwner->DeactiveSword();
	}

	Update_Effects(pOwner);
	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_02_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_02_Phase1::Update_Effects(CSacrifice* pOwner)
{
	auto pTransform = pOwner->Get_Component<CTransform>();

	if (IsCrossAnimProgress(0.05f))
	{
		auto pTransform = pOwner->Get_Component<CTransform>();

		auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

		_smatrix worldMatrix = pTransform->Get_WorldMatrix();
		_vector3 vWorldPosition = _vector3::Transform(_vector3(-0.3f, 2.f, 0.3f), worldMatrix);

		_quaternion localQuaternion = _quaternion(0.02f, 0.01f, 0.36f, 0.93f);
		_quaternion worldQuaternion = pTransform->Get_QuaternionRotate();
		localQuaternion *= worldQuaternion;

		auto effect = pObjectContainer->Find_ObjectByName("Sacrifice_Sword_Slash");
		auto pEffectTransform = effect->Get_Component<CTransform>();

		pEffectTransform->Set_WorldPos(vWorldPosition);
		pEffectTransform->Set_WorldQuaternion(localQuaternion);

		static_cast<CEffectContainer*>(effect)->Play();
	}
}

void CSacrificeState_Attack_03_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_03").Loop(false).Speed(1.2f).Apply();

	HitDesc hitDesc{};
	pOwner->Active_AttackSign();
	pOwner->SetAutoPlayBattleCollider("Right_Arm", 0.1f, 0.2f, hitDesc);
}

void CSacrificeState_Attack_03_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.3f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if(m_fAnimProgress<0.14f)
		pOwner->RotateToTarget(dt, 10.f);

	Update_Effects(pOwner);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_03_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_03_Phase1::Update_Effects(CSacrifice* pOwner)
{
	auto pTransform = pOwner->Get_Component<CTransform>();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	/* Smoke Slash */
	if (IsCrossAnimProgress(0.15f))
	{
		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_smoke_slash.json")
			.Build("SmokeSlash");

		_vector3 vRight = pTransform->Dir(STATE::RIGHT);

		_vector3 vWorldPosition = pTransform->Get_WorldPos();
		vWorldPosition.y += 2.f;
		vWorldPosition += vRight * 0.5f;
		_quaternion worldQuaternion = pTransform->Get_QuaternionRotate();
		_quaternion localQuaternion = _quaternion(0.07f, 0.06f, 0.99f, 0.12f);
		localQuaternion *= worldQuaternion;

		auto pEffectTransform = effect->Get_Component<CTransform>();
		pEffectTransform->Set_Quaternion(localQuaternion);
		pEffectTransform->Set_Pos(vWorldPosition);

		ObjectManager()->Add_Object(effect, { pOwner->Get_Level(),"Effect_Layer" });
	}

	/* Hit ground smoke */
	if (IsCrossAnimProgress(0.16f))
	{
		_vector3 vBonePosition = pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, "Skn_Finger2_03");
		vBonePosition = _vector3::Transform(vBonePosition, pTransform->Get_WorldMatrix());

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("hit_ground_smoke.json")
			.Position(vBonePosition)
			.Build("Smoke");

		auto flare = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hit_ground_flare.json")
			.Position(vBonePosition)
			.Build("Smoke");

		vBonePosition.y += 2.f;
		auto smoke = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hit_ground_flare_smoke.json")
			.Position(vBonePosition)
			.Build("FlareSmoke");

		CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(effect, { pOwner->Get_Level(),"Effect_Layer" });
		CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(flare, { pOwner->Get_Level(),"Effect_Layer" });
		CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(smoke, { pOwner->Get_Level(),"Effect_Layer" });
	}
}


void CSacrificeState_Attack_04_1_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_04_1").Loop(false).Speed(1.2f).Apply();

	HitDesc hitDesc{};
	pOwner->Active_AttackSign();
	pOwner->SetAutoPlayBattleCollider("Right_Arm", 0.2f, 0.5f, hitDesc);
}

void CSacrificeState_Attack_04_1_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.25f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (m_fAnimProgress < 0.1f)
		pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_04_1_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_04_2_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_04_2").Loop(false).Speed(1.f).Apply();
}

void CSacrificeState_Attack_04_2_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_04_2_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_05_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_05").Loop(false).Speed(1.f).Apply();

}

void CSacrificeState_Attack_05_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	Update_Effects(pOwner);

	pOwner->RotateToTarget(dt, 10.f);
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_05_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_05_Phase1::Update_Effects(CSacrifice* pOwner)
{
	if (IsCrossAnimProgress(0.3f))
	{
		auto pAnimator = pOwner->Get_Component<CAnimator3D>();
		auto pTransform = pOwner->Get_Component<CTransform>();

		_vector3 vTargetPosition = pOwner->GetTargetingInfo().vTargetPos;
		_vector3 vBonePosition = pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, "Ctr_Eye6_05");
		vBonePosition = _vector3::Transform(vBonePosition, pTransform->Get_WorldMatrix());

		_vector3 vDir = vTargetPosition - vBonePosition;
		vDir.Normalize();

		auto pOrb = Builder::Create_Object({ "Zero_Level","Proto_GameObject_SacrificeOrb"})
			.Position(vBonePosition)
			.Build("SacrificeOrb");

		pOrb->Get_Component<CTransform>()->Set_Look(vDir);
		ObjectManager()->Add_Object(pOrb, { pOwner->Get_Level(),"Effect_Layer" });
	}

	if (IsCrossAnimProgress(0.35f))
	{
		auto pAnimator = pOwner->Get_Component<CAnimator3D>();
		auto pTransform = pOwner->Get_Component<CTransform>();

		_vector3 vTargetPosition = pOwner->GetTargetingInfo().vTargetPos;
		_vector3 vBonePosition = pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, "Ctr_Eye6_05");
		vBonePosition = _vector3::Transform(vBonePosition, pTransform->Get_WorldMatrix());

		_vector3 vDir = vTargetPosition - vBonePosition;
		vDir.Normalize();

		auto pOrb = Builder::Create_Object({ "Zero_Level","Proto_GameObject_SacrificeOrb" })
			.Position(vBonePosition)
			.Build("SacrificeOrb");

		pOrb->Get_Component<CTransform>()->Set_Look(vDir);
		ObjectManager()->Add_Object(pOrb, { pOwner->Get_Level(),"Effect_Layer" });
	}

	if (IsCrossAnimProgress(0.5f))
		pOwner->ActiveLaser(1);

	if (IsCrossAnimProgress(0.58f))
		pOwner->DeactiveLaser();
}

void CSacrificeState_Attack_06_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_06").Loop(true).Speed(1.f).Apply();
	
	pOwner->Set_HitBlendable(false);
}

void CSacrificeState_Attack_06_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	if (m_fAnimProgress > 0.15f && m_fAnimProgress <= 0.65f)
	{
		auto pTransform = pOwner->Get_Component<CTransform>();
		_quaternion vRot = _quaternion::CreateFromYawPitchRoll(XMConvertToRadians(180.f) * dt, 0.f, 0.f);
		pTransform->Add_Quaternion(vRot);
	}
	
	if (IsCrossAnimProgress(0.18f))
		pOwner->ActiveLaser(0);
	
	if (IsCrossAnimProgress(0.65f))
		pOwner->DeactiveLaser();
	
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.9f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;

		pOwner->Set_HitBlendable(true);
	}
	
	pOwner->MoveByRootMotion(dt);
}

void CSacrificeState_Attack_06_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_07_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_07").Loop(false).Speed(1.2f).Apply();

	HitDesc hitDesc{};
	pOwner->Active_AttackSign();
	pOwner->SetAutoPlayBattleCollider("Sword", 0.f, 0.5f, hitDesc);
}

void CSacrificeState_Attack_07_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.7f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	Update_Effects(pOwner);

	if (IsCrossAnimProgress(0.05f))
		pOwner->ActiveSword();	

	if (IsCrossAnimProgress(0.6f))
		pOwner->DeactiveSword();

	if (m_fAnimProgress < 0.3f)
		pOwner->RotateToTarget(dt, 20.f);
	pOwner->MoveByRootMotion(dt, 1.5f);
}

void CSacrificeState_Attack_07_Phase1::Exit(CSacrifice* pOwner)
{
	static_cast<CSacrifice*>(pOwner)->DeactiveSword();
}

void CSacrificeState_Attack_07_Phase1::Update_Effects(CSacrifice* pOwner)
{
	if (IsCrossAnimProgress(0.3f))
	{
		auto smokeTrail = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_smoke_trail.json")
			.Build("Smoke");
		
		_vector3 vPosition = pOwner->Get_Component<CTransform>()->Get_Pos();
		auto smokeConeTrail = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_smoke_trail_cone.json")
			.Position(vPosition)
			.Build("SmokeCone");

		auto rushTrail = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_rush_trail.json")
			.Build("Rush");
		
		auto pSmokeConeTransform = smokeConeTrail->Get_Component<CTransform>();
		pSmokeConeTransform->Set_Quaternion(pOwner->Get_Component<CTransform>()->Get_QuaternionRotate());
		//pSmokeConeTransform->Set_Pos(vPosition + _vector3(pSmokeConeTransform->Dir(STATE::LOOK) * 4.f));

		auto smokeBoneFollower = smokeTrail->Add_Component<CBoneFollower>();
		smokeBoneFollower->Initialize(nullptr);
		smokeBoneFollower->Link_Bone(pOwner->Get_Component<CAnimator3D>(), "RootNode");

		_smatrix rushTrailOffset = _smatrix::Identity;
		rushTrailOffset = _smatrix::CreateRotationY(XMConvertToRadians(180.f)) * _smatrix::CreateTranslation(_vector3(0.f, 1.f, -4.f));
		static_cast<CEffectContainer*>(rushTrail)->AttachBone(pOwner->Get_Component<CAnimator3D>(), "RootNode", rushTrailOffset);

		ObjectManager()->Add_Object(smokeTrail, { pOwner->Get_Level(),"Effect_Layer" });
		ObjectManager()->Add_Object(smokeConeTrail, { pOwner->Get_Level(),"Effect_Layer" });
		ObjectManager()->Add_Object(rushTrail, { pOwner->Get_Level(),"Effect_Layer" });
	}
}

void CSacrificeState_Attack_08_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_08").Loop(false).Speed(1.f).Apply();

	HitDesc hitDesc{};
	pOwner->Active_AttackSign();
	pOwner->SetAutoPlayBattleCollider("Axe", 0.f, 0.4f, hitDesc);

	m_IsAttackStart = false;
	m_IsAttackEnd = false;
	m_IsJumpStart = false;	

	_vector3 vCurrDir = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
	_vector3 vTargetPos = pOwner->GetTargetingInfo().vTargetPos;
	m_vFirstTargetPosition = vTargetPos - vCurrDir * 2.f;
}

void CSacrificeState_Attack_08_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	Update_Effects(pOwner);
	Update_Weapons(pOwner);
	Update_Move(pOwner, dt);
}

void CSacrificeState_Attack_08_Phase1::Exit(CSacrifice* pOwner)
{
	
}

void CSacrificeState_Attack_08_Phase1::Update_Effects(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	auto pTransform = pOwner->Get_Component<CTransform>();

	/* Jump Start */
	if (IsCrossAnimProgress(0.12f))
	{
		_vector3 vBonePosition = pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, "Skn_Finger2_03");
		vBonePosition = _vector3::Transform(vBonePosition, pTransform->Get_WorldMatrix());
		vBonePosition.y -= 0.2f;

		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("hit_ground_smoke.json")
			.Position(vBonePosition)
			.Build("Smoke");

		auto pRockParticle = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("rock_particle.json")
			.Position(vBonePosition)
			.Build("Rock");

		ObjectManager()->Add_Object(pEffect, { pOwner->Get_Level(),"Effect_Layer" });
		ObjectManager()->Add_Object(pRockParticle, { pOwner->Get_Level(),"Effect_Layer" });
	}

	/* Axe Slash2 */
	if (IsCrossAnimProgress(0.17f))
	{
		auto pTransform = pOwner->Get_Component<CTransform>();

		auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

		_smatrix worldMatrix = pTransform->Get_WorldMatrix();
		_vector3 vWorldPosition = _vector3::Transform(_vector3(0.3f, 2.5f, 0.f), worldMatrix);

		_quaternion localQuaternion = _quaternion(0.9f, 0.27f, -0.35f, 0.03f);
		_quaternion worldQuaternion = pTransform->Get_QuaternionRotate();
		localQuaternion *= worldQuaternion;

		auto effect = pObjectContainer->Find_ObjectByName("Sacrifice_Axe_Slash2");
		auto pEffectTransform = effect->Get_Component<CTransform>();

		pEffectTransform->Set_WorldPos(vWorldPosition);
		pEffectTransform->Set_WorldQuaternion(localQuaternion);

		static_cast<CEffectContainer*>(effect)->Play();
	}

	/* Axe Slash */
	if (IsCrossAnimProgress(0.3f))
	{
		auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

		_smatrix worldMatrix = pTransform->Get_WorldMatrix();
		_vector3 vWorldPosition = _vector3::Transform(_vector3(-0.2f, 5.8f, 3.6f), worldMatrix);

		_quaternion localQuaternion = _quaternion(-0.07f, -0.01f, -0.64f, 0.77f);
		_quaternion worldQuaternion = pTransform->Get_QuaternionRotate();
		localQuaternion *= worldQuaternion;

		auto effect = pObjectContainer->Find_ObjectByName("Sacrifice_Axe_Slash1");
		auto pEffectTransform = effect->Get_Component<CTransform>();

		pEffectTransform->Set_WorldPos(vWorldPosition);
		pEffectTransform->Set_WorldQuaternion(localQuaternion);

		static_cast<CEffectContainer*>(effect)->Play();
	}
	
	if (IsCrossAnimProgress(0.34f))
	{
		auto pAnimator = pOwner->Get_Component<CAnimator3D>();

		_vector3 vBonePosition = pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, "Bip001 L Hand");
		vBonePosition = _vector3::Transform(vBonePosition, pTransform->Get_WorldMatrix());
		//vBonePosition.y -= 0.2f;
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("hit_ground_smoke_strong.json")
			.Position(vBonePosition)
			.Build("Smoke");

		ObjectManager()->Add_Object(pEffect, { pOwner->Get_Level(),"Effect_Layer" });
	}
}

void CSacrificeState_Attack_08_Phase1::Update_Weapons(CSacrifice* pOwner)
{
	if (IsCrossAnimProgress(0.06f))
	{
		m_IsAttackStart = true;
		pOwner->ActiveAxe();
	}

	if (!m_IsAttackEnd && m_fAnimProgress >= 0.6f)
	{
		m_IsAttackEnd = true;
		pOwner->DeactiveAxe();
	}
}

void CSacrificeState_Attack_08_Phase1::Update_Move(CSacrifice* pOwner, _float dt)
{
	/* Move Update */
	if (m_fAnimProgress < 0.3f)
		pOwner->RotateToTarget(dt, 10.f);

	auto pCCT = pOwner->Get_Component<CCharacterController>();
	if (!m_IsJumpStart && m_fAnimProgress >= 0.2f)
	{
		_vector3 vCurrDir = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
		_vector3 vTargetPos = pOwner->GetTargetingInfo().vTargetPos;
		m_vSecondTargetPosition = vTargetPos - vCurrDir * 2.f;
		m_IsJumpStart = true;
	}

	if (m_IsAttackStart)
	{
		if (m_fAnimProgress < 0.2f)
		{
			_vector3 vCurrPosition = pOwner->Get_Component<CTransform>()->Get_Pos();
			_vector3 vNextPosition = _vector3::Lerp(vCurrPosition, m_vFirstTargetPosition, dt * 3.f);
			_vector3 vVelocity = (vNextPosition - vCurrPosition) / dt;
			pCCT->Move_Velocity(vVelocity, dt);
		}
		else if (m_fAnimProgress < 0.5f)
		{
			_vector3 vCurrPosition = pOwner->Get_Component<CTransform>()->Get_Pos();
			_vector3 vNextPosition = _vector3::Lerp(vCurrPosition, m_vSecondTargetPosition, dt * 3.f);
			_vector3 vVelocity = (vNextPosition - vCurrPosition) / dt;
			pCCT->Move_Velocity(vVelocity, dt);
		}
	}
}

void CSacrificeState_Attack_09_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_09").Loop(false).Speed(1.f).Apply();
}

void CSacrificeState_Attack_09_Phase1::Update(CSacrifice* pOwner, _float dt)
{
}

void CSacrificeState_Attack_09_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_10_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_10").Loop(false).Speed(1.2f).Apply();

	pOwner->Phase1Attack();
}

void CSacrificeState_Attack_10_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.2f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	pOwner->RotateToTarget(dt, 10.f);
}

void CSacrificeState_Attack_10_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_11_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_11").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_11_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.3f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	pOwner->RotateToTarget(dt, 10.f);
}

void CSacrificeState_Attack_11_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_12_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_12").Loop(false).Speed(1.2f).Apply();
}

void CSacrificeState_Attack_12_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.5f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	pOwner->RotateToTarget(dt, 10.f);
}

void CSacrificeState_Attack_12_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Turn_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Attack_Turn").Loop(false).Speed(1.2f).Apply();

	HitDesc hitDesc{};
	pOwner->SetAutoPlayBattleCollider("Right_Arm", 0.2f, 0.4f, hitDesc);
}

void CSacrificeState_Attack_Turn_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.6f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}
	
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	auto pTransform = pOwner->Get_Component<CTransform>();

	_quaternion vDeltaQuaternion = pAnimator->Get_RootBoneQuatDelta();
	pTransform->Add_Quaternion(vDeltaQuaternion);

	Update_Effects(pOwner);
}

void CSacrificeState_Attack_Turn_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Turn_Phase1::Update_Effects(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	auto pTransform = pOwner->Get_Component<CTransform>();

	/* Smoke Sweep Trail */
	if (IsCrossAnimProgress(0.31f))
	{
		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_sweep_trail.json")
			.Build("HandSweepTrail");
		effect->AttachBone(pAnimator, "Skn_Finger3_03");

		ObjectManager()->Add_Object(effect, { pOwner->Get_Level(),"Effect_Layer" });
	}

	/* Smoke Slash */
	if (IsCrossAnimProgress(0.32f))
	{
		auto pObjectContainer = pOwner->Get_Component<CObjectContainer>();

		auto effect = pObjectContainer->Find_ObjectByName("Sacrifice_Smoke_Slash2");
		auto pEffectTransform = effect->Get_Component<CTransform>();

		pEffectTransform->Set_Pos(_vector3(0.f, 0.8f, 0.3f));
		pEffectTransform->Set_Quaternion(_quaternion(0.0f, 0.f, 0.f, 1.f));
		static_cast<CEffectContainer*>(effect)->Play();
	}
}

void CSacrificeState_Attack_Roar_Phase1::Enter(CSacrifice* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringer_Ani_P1_Stun_Roar").Loop(false).Speed(1.f).Apply();
}

void CSacrificeState_Attack_Roar_Phase1::Update(CSacrifice* pOwner, _float dt)
{
	CSacrifice::SACRIFICE_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	if (m_fAnimProgress >= 0.8f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
		{
			blackBoard.isRequestNext = true;
			pOwner->Set_DissolveState(CSacrifice::DISSOLVE_STATE::NONE, 0.f);
		}
	}

	if (IsCrossAnimProgress(0.5f))
		pOwner->Set_DissolveState(CSacrifice::DISSOLVE_STATE::DISAPPEAR, 0.3f);

	if (IsCrossAnimProgress(0.7f))
	{
		pOwner->Set_DissolveState(CSacrifice::DISSOLVE_STATE::APPEAR, 0.2f);
		pOwner->Get_Component<CCharacterController>()->Set_Position(_vector3(-2.f, 1.f, 21.f));

	}

	pOwner->Update_Dissolve(dt);
	Update_Effects(pOwner);
}

void CSacrificeState_Attack_Roar_Phase1::Exit(CSacrifice* pOwner)
{
}

void CSacrificeState_Attack_Roar_Phase1::Update_Effects(CSacrifice* pOwner)
{
	auto pTransform = pOwner->Get_Component<CTransform>();

	/* Roar Smoke */
	if (IsCrossAnimProgress(0.31f))
	{
		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_roar_smoke_down.json")
			.Build("Sacrifice_Roar_Smoke_Down");

		_smatrix worldMatrix = pTransform->Get_WorldMatrix();
		_vector3 vWorldPosition = _vector3::Transform(_vector3(0.f, 1.f, 0.f), worldMatrix);

		auto pEffectTransform = effect->Get_Component<CTransform>();
		pEffectTransform->Set_WorldPos(vWorldPosition);

		ObjectManager()->Add_Object(effect, { pOwner->Get_Level(),"Effect_Layer" });
	}
}
