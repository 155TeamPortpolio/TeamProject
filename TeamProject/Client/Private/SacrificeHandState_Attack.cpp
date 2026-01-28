#include "pch.h"
#include "SacrificeHandState_Attack.h"
#include "GameInstance.h"
#include "SacrificeHand.h"
#include "Sacrifice.h"
#include "Child.h"
#include "EffectContainer.h"
#include "BattleSystem.h"

#include "CamDirector.h"

void CSacrificeHandState_Attack::Enter(CSacrificeHand* pOwner)
{
	if (!m_pSubStateMachine)
	{
		m_pSubStateMachine = CStateMachine<CSacrificeHand>::Create();

		Register_States();
		__super::Enter(pOwner);
	}

	BuildPattern(pOwner);
}

void CSacrificeHandState_Attack::Update(CSacrificeHand* pOwner, _float dt)
{
	__super::Update(pOwner, dt);

	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
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
		pOwner->Idle();
}

void CSacrificeHandState_Attack::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack::Register_States()
{
	/* Phase1 Attack */
	m_pSubStateMachine->Register_State("Attack01_Phase1", CSacrificeHandState_Attack_01_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack02_Phase1", CSacrificeHandState_Attack_02_Phase1::Create());
	m_pSubStateMachine->Register_State("Attack03_Phase1", CSacrificeHandState_Attack_03_Phase1::Create());

	/* Phase2 Attack */
	m_pSubStateMachine->Register_State("Attack04_Phase2", CSacrificeHandState_Attack_04_Phase2::Create());

	/* OverDrive Start */
	m_pSubStateMachine->Register_State("OverDrive_Start", CSacrificeHandState_OverDrive_Release_Start_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Loop", CSacrificeHandState_OverDrive_Release_Loop_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_End", CSacrificeHandState_OverDrive_Release_End_Phase2::Create());

	/* OverDrive Attack */
	m_pSubStateMachine->Register_State("OverDrive_Attack01", CSacrificeHandState_OverDrive_Release_Attack01_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Attack02", CSacrificeHandState_OverDrive_Release_Attack02_Phase2::Create());
	m_pSubStateMachine->Register_State("OverDrive_Attack03", CSacrificeHandState_OverDrive_Release_Attack03_Phase2::Create());
}

void CSacrificeHandState_Attack::BuildPattern(CSacrificeHand* pOwner)
{
	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	blackBoard.stateQueue.clear();

	switch (blackBoard.eCurrPattern)
	{
	case Client::CSacrificeHand::PATTERN::PHASE1:
	{
		blackBoard.stateQueue.push_back("Attack01_Phase1");
		blackBoard.stateQueue.push_back("Attack02_Phase1");
		blackBoard.stateQueue.push_back("Attack03_Phase1");
	}break;
	case Client::CSacrificeHand::PATTERN::PHASE2:
	{
		blackBoard.stateQueue.push_back("Attack04_Phase2");
	}break;
	case Client::CSacrificeHand::PATTERN::OVER_DRIVE_START:
	{
		blackBoard.stateQueue.push_back("OverDrive_Start");
		blackBoard.stateQueue.push_back("OverDrive_Loop");
	}break;
	case Client::CSacrificeHand::PATTERN::OVER_DRIVE_ATTACK01:
	{
		blackBoard.stateQueue.push_back("OverDrive_Attack01");
	}break;
	case Client::CSacrificeHand::PATTERN::OVER_DRIVE_ATTACK02:
	{
		blackBoard.stateQueue.push_back("OverDrive_Attack02");
	}break;
	case Client::CSacrificeHand::PATTERN::OVER_DRIVE_ATTACK03:
	{
		blackBoard.stateQueue.push_back("OverDrive_Attack03");
	}break;
	default:
		break;
	}

	if (!blackBoard.stateQueue.empty())
		blackBoard.isRequestNext = true;
}

void CSacrificeHandState_Attack_01_Phase1::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("SacrificeBringerHand_Ani_P1_Attack_10").Loop(false).Speed(1.2f).Apply();

	HitDesc hitDesc{};
	pOwner->SetAutoPlayBattleCollider("Hand", 0.f, 0.6f, hitDesc);

	pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::APPEAR, 0.1f);
	pOwner->SetVisable(true);
}

void CSacrificeHandState_Attack_01_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fAnimProgress >= 0.2)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	pOwner->Update_Dissolve(dt);
	Update_Effects(pOwner);
}

void CSacrificeHandState_Attack_01_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_01_Phase1::Update_Effects(CSacrificeHand* pOwner)
{
	if (IsCrossAnimProgress(0.15f))
	{
		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_smoke_trail.json")
			.Build("HandSmokeTrail");

		static_cast<CEffectContainer*>(effect)->AttachBone(pOwner->Get_Component<CAnimator3D>(), "Eye01_A1");

		ObjectManager()->Add_Object(effect, { "Zero_Level","Effect_Layer" });
	}
}

void CSacrificeHandState_Attack_02_Phase1::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringerHand_Ani_P1_Attack_11").Loop(false).Speed(1.2f).Apply();

	HitDesc hitDesc{};
	pOwner->SetAutoPlayBattleCollider("Hand", 0.f, 0.6f, hitDesc);
}

void CSacrificeHandState_Attack_02_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (IsCrossAnimProgress(0.2f))
		pOwner->SetVisable(false);

	if (m_fAnimProgress >= 0.3)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (IsCrossAnimProgress(0.15f))
		pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::DISAPPEAR, 0.1f);

	pOwner->Update_Dissolve(dt);
	Update_Effects(pOwner);
}

void CSacrificeHandState_Attack_02_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_02_Phase1::Update_Effects(CSacrificeHand* pOwner)
{
	if (IsCrossAnimProgress(0.12f))
	{
		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_smoke_trail.json")
			.Build("HandSmokeTrail");

		static_cast<CEffectContainer*>(effect)->AttachBone(pOwner->Get_Component<CAnimator3D>(), "Eye01_A1");

		ObjectManager()->Add_Object(effect, { "Zero_Level","Effect_Layer" });
	}
}

void CSacrificeHandState_Attack_03_Phase1::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("SacrificeBringerHand_Ani_P1_Attack_12").Loop(false).Speed(1.2f).Apply();

	auto pTrasform = pOwner->Get_Component<CTransform>();
	CSacrifice* pParent = static_cast<CSacrifice*>(pOwner->Get_Component<CChild>()->Get_Parent());

	_vector3 vCurrPosition = pTrasform->Get_WorldPos();
	_vector3 vTargetPosition = pParent->GetTargetingInfo().vTargetPos;
	_vector3 vDir = vTargetPosition - vCurrPosition;
	vDir.y = 0.f;
	vDir.Normalize();

	pTrasform->Set_Look(vDir);
	pTrasform->Set_Pos(vTargetPosition - vDir * 16.f);


	HitDesc hitDesc{};
	pOwner->SetAutoPlayBattleCollider("Hand", 0.f, 0.6f, hitDesc);

	pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::NONE, 0.f);
	m_IsActiveHand = false;
}

void CSacrificeHandState_Attack_03_Phase1::Update(CSacrificeHand* pOwner, _float dt)
{
	ATTACK_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (!m_IsActiveHand && m_fAnimProgress >= 0.1f)
	{
		pOwner->SetVisable(true);
		m_IsActiveHand = true;
	}

	if (m_fAnimProgress >= 0.6)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (IsCrossAnimProgress(0.55f))
		pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::DISAPPEAR, 0.1f);

	pOwner->Update_Dissolve(dt);
	Update_Effects(pOwner);
}

void CSacrificeHandState_Attack_03_Phase1::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_Attack_03_Phase1::Update_Effects(CSacrificeHand* pOwner)
{
	if (IsCrossAnimProgress(0.1f))
	{
		_vector3 vWorldPosition = pOwner->Get_WorldPos();

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_ground_up.json")
			.Position(vWorldPosition)
			.Build("HandGroundUp");

		ObjectManager()->Add_Object(effect, { "Zero_Level","Effect_Layer" });
	}

	/* Hit Ground Smoke */
	if (IsCrossAnimProgress(0.41f))
	{
		auto pAnimator = pOwner->Get_Component<CAnimator3D>();
		auto pTransform = pOwner->Get_Component<CTransform>();

		_vector3 vBonePosition = pAnimator->Get_BonePosition(CAnimator3D::BoneSpace::COMBINED, "Eye01_A1");
		vBonePosition = _vector3::Transform(vBonePosition, pTransform->Get_WorldMatrix());
		//vBonePosition.y -= 0.2f;
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("hit_ground_smoke_strong.json")
			.Position(vBonePosition)
			.Build("Smoke");

		ObjectManager()->Add_Object(pEffect, { "Zero_Level","Effect_Layer"});

		CameraManager()->AddImpact(ENUM(CamShakeType::ExplosionBig), ENUM(CamZoomType::ExplosionBig), 1.6f);
	}
}

void CSacrificeHandState_Attack_04_Phase2::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("SacrificeBringerHand_Ani_Attack_04").Loop(false).Speed(1.2f).Apply();

	HitDesc hitDesc{};
	pOwner->SetAutoPlayBattleCollider("Hand", 0.f, 0.6f, hitDesc);
	pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::NONE, 0.f);
	pOwner->SetVisable(true);
}

void CSacrificeHandState_Attack_04_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (m_fAnimProgress >= 0.25f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;

		pOwner->SetVisable(false);
	}
}

void CSacrificeHandState_Attack_04_Phase2::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_OverDrive_Release_Start_Phase2::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_SacrificeBringerHand_Ani_P2_OverDrive_Charge_Start_New").Loop(false).Speed(1.2f).Apply();

	pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::NONE, 0.f);
	pOwner->SetVisable(true);
}

void CSacrificeHandState_OverDrive_Release_Start_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	Update_Effects(pOwner);
}

void CSacrificeHandState_OverDrive_Release_Start_Phase2::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_OverDrive_Release_Start_Phase2::Update_Effects(CSacrificeHand* pOwner)
{
	if (IsCrossAnimProgress(0.18f))
	{
		_vector3 vWorldPosition = pOwner->Get_Component<CTransform>()->Get_WorldPos();
		_vector3 vLook = pOwner->Get_Component<CTransform>()->Dir(STATE::LOOK);
		vWorldPosition.y += 0.1f;

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_overdrive_charge.json")
			.Position(vWorldPosition)
			.Build("Sacrifice_Hand_Overdrive_Charge");

		effect->Get_Component<CTransform>()->Set_Look(vLook);
		ObjectManager()->Add_Object(effect, { "Zero_Level","Enemy_Effect_Layer" });
	}
}

void CSacrificeHandState_OverDrive_Release_Loop_Phase2::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Change_Animation("Monster_SacrificeBringerHand_Ani_P2_OverDrive_Charge_Loop").Loop(true).Speed(1.2f).Apply();

	pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::NONE, 0.f);
}

void CSacrificeHandState_OverDrive_Release_Loop_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();

	if (m_fStateTime >= 3.5f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (CSacrificeHand::DISSOLVE_STATE::NONE == pOwner->Get_DissolveState() && m_fStateTime >= 3.2f)
		pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::DISAPPEAR, 0.1f);

	pOwner->Update_Dissolve(dt);
}

void CSacrificeHandState_OverDrive_Release_Loop_Phase2::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_OverDrive_Release_End_Phase2::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_SacrificeBringerHand_Ani_P2_OverDrive_Release_Start").Loop(false).Speed(1.2f).Apply();

}

void CSacrificeHandState_OverDrive_Release_End_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (pAnimator->isCurrentAnimEnd(0))
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (IsCrossAnimProgress(0.7f))
		pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::DISAPPEAR, 0.2f);

	pOwner->Update_Dissolve(dt);
}

void CSacrificeHandState_OverDrive_Release_End_Phase2::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_OverDrive_Release_Attack01_Phase2::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack01").Loop(false).Speed(1.2f).Apply();

	HitDesc desc{};
	pOwner->SetAutoPlayBattleCollider("Hand_Sword", 0.f, 0.7f, desc);
	pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::APPEAR, 0.1f);
	pOwner->SetVisable(true);
	pOwner->Active_SwordRimLight();
}

void CSacrificeHandState_OverDrive_Release_Attack01_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (m_fAnimProgress >= 0.9f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (IsCrossAnimProgress(0.85f))
		pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::DISAPPEAR, 0.1f);

	pOwner->Update_Dissolve(dt);

	Update_Effects(pOwner);
}

void CSacrificeHandState_OverDrive_Release_Attack01_Phase2::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_OverDrive_Release_Attack01_Phase2::Update_Effects(CSacrificeHand* pOwner)
{
	/* Slash1 */
	if (IsCrossAnimProgress(0.3f))
	{
		auto pAnimator = pOwner->Get_Component<CAnimator3D>();

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_overdrive_attack1_1.json")
			.Build("Sacrifice_Hand_Attack1");

		_smatrix offsetMatrix = _smatrix::Identity;
		offsetMatrix.Translation(_vector3(20.f, -1.f, 0.f));
		effect->AttachBone(pAnimator, "Ctr_HSword", offsetMatrix);
		ObjectManager()->Add_Object(effect, { "Zero_Level","Enemy_Effect_Layer" });
	}

	/* Slash2 */
	if (IsCrossAnimProgress(0.57f))
	{
		auto pAnimator = pOwner->Get_Component<CAnimator3D>();

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_overdrive_attack1_2.json")
			.Build("Sacrifice_Hand_Attack1");

		_smatrix offsetMatrix = _smatrix::Identity;
		offsetMatrix.Translation(_vector3(20.f, -1.f, 0.f));
		effect->AttachBone(pAnimator, "Ctr_HSword", offsetMatrix);
		ObjectManager()->Add_Object(effect, { "Zero_Level","Enemy_Effect_Layer" });
	}
}

void CSacrificeHandState_OverDrive_Release_Attack02_Phase2::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack02").Loop(false).Speed(1.2f).Apply();

	HitDesc desc{};
	pOwner->SetAutoPlayBattleCollider("Hand_Sword", 0.f, 0.9f, desc);
	pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::APPEAR, 0.1f);
	pOwner->SetVisable(true);
}

void CSacrificeHandState_OverDrive_Release_Attack02_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (m_fAnimProgress >= 0.9f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;
	}

	if (m_fAnimProgress < 0.3f)
		Rotate_ToTarget(pOwner, dt);

	if (IsCrossAnimProgress(0.85f))
		pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::DISAPPEAR, 0.1f);

	pOwner->Update_Dissolve(dt);
	Update_Effects(pOwner);
}

void CSacrificeHandState_OverDrive_Release_Attack02_Phase2::Exit(CSacrificeHand* pOwner)
{
}

void CSacrificeHandState_OverDrive_Release_Attack02_Phase2::Rotate_ToTarget(CSacrificeHand* pOwner, _float dt)
{
	auto pTransform = pOwner->Get_Component<CTransform>();

	auto& infos = BattleSystem()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::PLAYER);
	_vector3 vTargetPosition(0.f, 0.f, 0.f);
	_vector3 vCurrPosition = pTransform->Get_WorldPos();
	_vector3 vTargetDir(0.f, 0.f, 1.f);
	_vector3 vCurrDir = pTransform->Dir(STATE::LOOK);

	auto playerHandle = BattleSystem()->GetCurCharacterHandle();
	vTargetPosition = playerHandle.Get()->Get_Component<CTransform>()->Get_WorldPos();

	vTargetDir = vTargetPosition - vCurrPosition;
	vTargetDir.y = 0.f;
	vTargetDir.Normalize();

	_vector3 vDir = _vector3::Lerp(vCurrDir, vTargetDir, dt * 30.f);
	pTransform->Set_Look(vDir);
}

void CSacrificeHandState_OverDrive_Release_Attack02_Phase2::Update_Effects(CSacrificeHand* pOwner)
{
	if (IsCrossAnimProgress(0.01f))
	{
		auto pAnimator = pOwner->Get_Component<CAnimator3D>();

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_overdrive_attack2_light.json")
			.Build("Sacrifice_Hand_Overdrive_Attack2_Light");

		effect->AttachBone(pAnimator, "Ctr_HSword");

		ObjectManager()->Add_Object(effect, { "Zero_Level","Enemy_Effect_Layer" });
	}

	/* Hand Up, Down */
	if (IsCrossAnimProgress(0.25f))
	{
		auto pTransform = pOwner->Get_Component<CTransform>();

		_vector3 vWorldPosition = _vector3::Transform(_vector3(0.f, 16.f, 6.f), pTransform->Get_WorldMatrix());
		auto effectUp = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_overdrive_attack2_up.json")
			.Position(vWorldPosition)
			.Build("Sacrifice_Hand_Overdrive_Attack2_Up");

		vWorldPosition = pTransform->Get_WorldPos();
		auto effectDown = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_overdrive_attack2_down.json")
			.Position(vWorldPosition)
			.Build("Sacrifice_Hand_Overdrive_Attack2_Down");

		ObjectManager()->Add_Object(effectUp, { "Zero_Level","Enemy_Effect_Layer" });
		ObjectManager()->Add_Object(effectDown, { "Zero_Level","Enemy_Effect_Layer" });
	}

	/* Hit Ground Flare */
	if (IsCrossAnimProgress(0.76f))
	{
		auto pTransform = pOwner->Get_Component<CTransform>();

		_vector3 vWorldPosition = _vector3::Transform(_vector3(0.f, 0.f, 38.4f), pTransform->Get_WorldMatrix());
		_vector3 vLook = pTransform->Dir(STATE::LOOK);

		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_overdrive_attack2_flare.json")
			.Build("Sacrifice_Hand_Overdrive_Attack2_Flare");

		auto effectTransform = effect->Get_Component<CTransform>();
		effectTransform->Set_WorldPos(vWorldPosition);
		effectTransform->Set_Look(vLook);
		ObjectManager()->Add_Object(effect, { "Zero_Level","Enemy_Effect_Layer" });
	}
}

void CSacrificeHandState_OverDrive_Release_Attack03_Phase2::Enter(CSacrificeHand* pOwner)
{
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();
	pAnimator->Set_Animation("Monster_SacrificeBringer_Ani_P2_OverDrive_Release_Attack03").Loop(false).Speed(1.2f).Apply();

	pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::APPEAR, 0.1f);
	pOwner->SetVisable(true);
}

void CSacrificeHandState_OverDrive_Release_Attack03_Phase2::Update(CSacrificeHand* pOwner, _float dt)
{
	CSacrificeHand::SACRIFICE_HAND_BLACK_BOARD& blackBoard = pOwner->GetBlackBoard();
	auto pAnimator = pOwner->Get_Component<CAnimator3D>();

	if (m_fAnimProgress >= 0.9f)
	{
		blackBoard.isChainOpen = true;
		if (!blackBoard.stateQueue.empty())
			blackBoard.isRequestNext = true;

		pOwner->Deactive_SwordRimLight();
	}

	if (IsCrossAnimProgress(0.85f))
		pOwner->Set_DissolveState(CSacrificeHand::DISSOLVE_STATE::DISAPPEAR, 0.1f);

	pOwner->Update_Dissolve(dt);
}

void CSacrificeHandState_OverDrive_Release_Attack03_Phase2::Exit(CSacrificeHand* pOwner)
{
}
