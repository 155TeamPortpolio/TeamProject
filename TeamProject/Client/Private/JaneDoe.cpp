#include "pch.h"
#include "JaneDoe.h"
#include "GameInstance.h"
#include "DataBase.h"
#include "EffectContainer.h"

#include "Material.h"
#include "MaterialInstance.h"

#include "Animator3D.h"
#include "CharacterController.h"
#include "ObjectContainer.h"

#include "StateMachine.h"
#include "JaneDoeState_Idle.h"
#include "JaneDoeState_Move.h"
#include "JaneDoeState_Attack.h"
#include "JaneDoeState_SwitchIn.h"
#include "JaneDoeState_SwitchOut.h"
#include "JaneDoeState_NormalAttack.h"
#include "JaneDoeState_Hit.h"
#include "JaneDoeState_Evade.h"

CJaneDoe::CJaneDoe()
{
}

CJaneDoe::CJaneDoe(const CJaneDoe& rhs)
	:CCharacter(rhs)
{
}

void CJaneDoe::Process_Passion(_float fPassionGauge)
{
	m_fPassionGauge = fPassionGauge;
}

void CJaneDoe::Process_PassionSkill(_bool bAvailable)
{
	m_bPassionSkillAvailable = bAvailable;
}

HRESULT CJaneDoe::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("JaneDoeModel.model",
		"../Bin/Resources/Model/skeletal/JaneDoe/JaneDoeModel.model");
	pRcsMgr->Add_ResourcePath("JaneDoeModel.mat",
		"../Bin/Resources/Model/skeletal/JaneDoe/JaneDoeModel.mat");
	pRcsMgr->Add_ResourcePath("JaneDoe_Meta.json",
		"../Bin/Resources/Model/skeletal/JaneDoe/JaneDoe_Meta.json");

	Get_Component<CModel>()->Link_Model(G_GlobalLevelKey, "JaneDoeModel.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "JaneDoeModel.mat");

	/* 이펙트 리소스 임시 로드 */
	{
		/* Asset */
		ResourceManager()->Add_ResourcePath("janedoe_normal1_slash.json", "../Bin/Resources/Effect/Data/JaneDoe/janedoe_normal1_slash.json");

		/* Texture */
		ResourceManager()->Add_ResourcePath("Eff_MeleeTrail_078_YZ_05.png", "../Bin/Resources/Effect/Texture/Eff_MeleeTrail_078_YZ_05.png");
		ResourceManager()->Add_ResourcePath("Dissolve.png", "../Bin/Resources/Effect/Texture/Dissolve.png");

		/* Model */
		ResourceManager()->Add_ResourcePath("JaneDoe_Slash0.model", "../Bin/Resources/Effect/Model/JaneDoe_Slash0/JaneDoe_Slash0.model");
		ResourceManager()->Add_ResourcePath("JaneDoe_Slash0.mat", "../Bin/Resources/Effect/Model/JaneDoe_Slash0/JaneDoe_Slash0.mat");
	}
	return S_OK;
}

HRESULT CJaneDoe::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	if (FAILED(Initialize_Weapon()))	   
		return E_FAIL;

	if (FAILED(Initialize_Effects()))
		return E_FAIL;

	return S_OK;
}

void CJaneDoe::Awake()
{
	__super::Awake();

	m_pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "JaneDoeModel.model");
	m_pAnimator->Link_MetaData(G_GlobalLevelKey, "JaneDoe_Meta.json");

	//m_pAnimator->Set_MotionBone(262);
	m_pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);

	m_strAnimName = "Avatar_Female_Size03_JaneDoe_Ani_";
	m_strName = "JaneDoe";
	m_eCharacterName = CHARACTER::JaneDoe;
	m_pAnimator->Set_Animation(Get_Name() + "Idle")
		.Loop(true)
		.Apply();
	m_pCCT->Set_GravityEnabled(true);

	Initialize_Stat();

	m_tEnergy.fCurrentEnergy = 120;

}

void CJaneDoe::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CJaneDoe::Update(_float dt)
{
	//Update_Input(dt);
	if (!m_bTest)
	{
		Update_States();
		m_pStateMachine->Update(dt);
	}
	__super::Update(dt);
}

void CJaneDoe::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

void CJaneDoe::Render_GUI()
{
	__super::Render_GUI();
	if (m_pStateMachine)
	{
		ImGui::Separator();
		ImGui::Checkbox("Animation Test", &m_bTest);
		ImGui::Text("StateMachine: %s", m_pStateMachine->Get_CurrentStateName().c_str());

		if (ImGui::Button("Open StateMachine"))
			m_pStateMachine->Set_ShowWindow(true);

		m_pStateMachine->Render_GUI();

	}
}

void CJaneDoe::On_SwitchIn(SWITCH eType)
{
	m_fDissolveProgress = 0.f;
	SetRenderLayer(RENDER_LAYER::Default);

	Set_Switch(eType);
	m_pStateMachine->Set_Trigger("SwitchIn");
}

void CJaneDoe::On_SwitchOut()
{
	m_pStateMachine->Set_Trigger("SwitchOut");
}

void CJaneDoe::On_Ultimate()
{
	__super::On_Ultimate();

	m_pStateMachine->Set_Int("AttackEntryMode", 3);
	m_pStateMachine->Set_Trigger("Attack");
}

void CJaneDoe::On_Special()
{
	if (InputDevice()->Key_Tap('E') == false) return;

	if (m_tEnergy.fCurrentEnergy >= m_tEnergy.fSpecialEnergy)
	{
		m_tEnergy.fCurrentEnergy -= m_tEnergy.fSpecialEnergy;
		UI_ACTION_DESC desc;
		desc.eType = UI_ACTION_TYPE::SPECIAL;
		desc.eState = UI_ACTION_STATE::EXECUTING;
		EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
	}
	m_pStateMachine->Set_Int("AttackEntryMode", 2);
	m_pStateMachine->Set_Trigger("Attack");
}

void CJaneDoe::On_Hit(DAMAGE_TYPE eType)
{
	m_pStateMachine->Set_Int("HitEntryMode", ENUM(eType));
	m_pStateMachine->Set_Trigger("ToHit");
}

HRESULT CJaneDoe::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CJaneDoe>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Idle");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CJaneDoe::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CJaneDoeState_Idle::Create());
	m_pStateMachine->Register_State("Move", CJaneDoeState_Move::Create());
	m_pStateMachine->Register_State("Attack", CJaneDoeState_Attack::Create());
	m_pStateMachine->Register_State("Evade", CJaneDoeState_Evade::Create());
	m_pStateMachine->Register_State("SwitchIn", CJaneDoeState_SwitchIn::Create());	//*SwitchIn*
	m_pStateMachine->Register_State("SwitchOut", CJaneDoeState_SwitchOut::Create());//*SwtichOut*
	m_pStateMachine->Register_State("Hit", CJaneDoeState_Hit::Create());

	return S_OK;
}

HRESULT CJaneDoe::Initialize_Transitions()
{
	// Idle -> Move
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CJaneDoe>::CONDITION_BOOL_TRUE, "IsMove");

	// Move -> Idle
	m_pStateMachine->Register_Transition("Move", "Idle",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

	// Attack
	m_pStateMachine->Register_AnyStateTransition("Attack",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "Attack");

	// Attack -> Idle
	m_pStateMachine->Register_Transition("Attack", "Idle",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

	// Evade
	m_pStateMachine->Register_AnyStateTransition("Evade",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToEvade");

	// Evade -> Move (Dash)
	m_pStateMachine->Register_Transition("Evade", "Move",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToMove");

	// Evade -> Idle (Backstep)
	m_pStateMachine->Register_Transition("Evade", "Idle",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

	// SwitchIn
	m_pStateMachine->Register_AnyStateTransition("SwitchIn",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "SwitchIn");

	m_pStateMachine->Register_Transition("SwitchIn", "Idle",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

	m_pStateMachine->Register_Transition("SwitchIn", "Move",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToMove");

	// SwitchOut
	m_pStateMachine->Register_AnyStateTransition("SwitchOut",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "SwitchOut");

	m_pStateMachine->Register_Transition("SwitchOut", "Idle",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

	m_pStateMachine->Register_AnyStateTransition("Hit",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToHit");

	m_pStateMachine->Register_Transition("Hit", "Idle",
		CStateMachine<CJaneDoe>::CONDITION_TRIGGER, "ToIdle");

	return S_OK;
}

HRESULT CJaneDoe::Initialize_Stat()
{
	auto Desc = CDataBase::GetInstance()->GetPlayerDesc(m_strName);
	m_tEnergy.fSpecialEnergy = Desc.SpecialAttack;

	auto LVDesc = CDataBase::GetInstance()->GetLevelDesc(m_iCurrentLevel);
	m_fMaxHP = LVDesc.MaxHP;
	m_fDefense = LVDesc.Defend;
	m_fAttackPower = LVDesc.Attack;
	
	Set_EvadeMax(3);
	return S_OK;
}

HRESULT CJaneDoe::Initialize_Weapon()
{
	ATTACK_COLLIDER_DESC HandL_WeaponDesc;
	HandL_WeaponDesc.eColliderType = COLLIDER_TYPE::BOX;
	HandL_WeaponDesc.pOwnerAnimator = Get_Component<CAnimator3D>();
	HandL_WeaponDesc.tagBone = "Ctr_L_HandWpn_F";
	HandL_WeaponDesc.tagName = "HandWeapon_L";
	HandL_WeaponDesc.vSize = { 0.3f, 0.1f, 0.1f };
	HandL_WeaponDesc.vCenter = { 0.1f, 0.f, 0.f };
	
	if (FAILED(Attach_AttackCollider(&HandL_WeaponDesc)))
		return E_FAIL;

	ATTACK_COLLIDER_DESC HandR_WeaponDesc;
	HandR_WeaponDesc.eColliderType = COLLIDER_TYPE::BOX;
	HandR_WeaponDesc.pOwnerAnimator = Get_Component<CAnimator3D>();
	HandR_WeaponDesc.tagBone = "Ctr_R_HandWpn_F";
	HandR_WeaponDesc.tagName = "HandWeapon_R";
	HandR_WeaponDesc.vSize = { 0.3f, 0.1f, 0.1f };
	HandR_WeaponDesc.vCenter = { 0.1f, 0.f, 0.f };

	if (FAILED(Attach_AttackCollider(&HandR_WeaponDesc)))
		return E_FAIL;

	ATTACK_COLLIDER_DESC BootsL_WeaponDesc;
	BootsL_WeaponDesc.eColliderType = COLLIDER_TYPE::BOX;
	BootsL_WeaponDesc.pOwnerAnimator = Get_Component<CAnimator3D>();
	BootsL_WeaponDesc.tagBone = "Ctr_L_BootsWpn_01";
	BootsL_WeaponDesc.tagName = "FootWeapon_L";
	BootsL_WeaponDesc.vSize = { 0.3f, 0.1f, 0.1f };
	BootsL_WeaponDesc.vCenter = { 0.3f, 0.f, 0.f };
	if (FAILED(Attach_AttackCollider(&BootsL_WeaponDesc)))
		return E_FAIL;

	ATTACK_COLLIDER_DESC BootsR_WeaponDesc;
	BootsR_WeaponDesc.eColliderType = COLLIDER_TYPE::BOX;
	BootsR_WeaponDesc.pOwnerAnimator = Get_Component<CAnimator3D>();
	BootsR_WeaponDesc.tagBone = "Ctr_R_BootsWpn_01";
	BootsR_WeaponDesc.tagName = "FootWeapon_R";
	BootsR_WeaponDesc.vSize = { 0.3f, 0.1f, 0.1f };
	BootsR_WeaponDesc.vCenter = { 0.3f, 0.f, 0.f };
	if (FAILED(Attach_AttackCollider(&BootsR_WeaponDesc)))
		return E_FAIL;

	Active_AttackCollider("FootWeapon_L", true);
	Active_AttackCollider("FootWeapon_R", true);

	return S_OK;
}

HRESULT CJaneDoe::Initialize_Effects()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	/* Normal Slash0 */
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("janedoe_normal1_slash.json")
			.Build("JaneDoe_Normal_Slash0");

		pObjectContainer->Add_Child(pEffect);
	}

	/* Normal Slash1 */
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("janedoe_normal1_slash.json")
			.Build("JaneDoe_Normal_Slash1");

		pObjectContainer->Add_Child(pEffect);
	}

	return S_OK;
}

void CJaneDoe::Update_States()
{
	m_pStateMachine->Set_Bool("IsMove", Is_Move_Buffer());

	Process_EndState(m_pStateMachine->Get_CurrentStateName());

	if (m_bIsEvade)
		m_pStateMachine->Set_Trigger("ToEvade");

	if (m_bIsAttack)
		Process_AttackInput(m_pStateMachine->Get_CurrentStateName());
}

void CJaneDoe::Process_AttackInput(const string& strCurrentState)
{
	if (strCurrentState == "Idle")
	{	// Idle -> NormalAttack
		m_pStateMachine->Set_Int("AttackEntryMode", 0);
		m_pStateMachine->Set_Trigger("Attack");
	}
	else if (strCurrentState == "Move")
	{	// Move
		CJaneDoeState_Move* pMove = static_cast<CJaneDoeState_Move*>(
			m_pStateMachine->Get_CurrentState());
		if (!pMove || !pMove->Get_SubStateMachine())
			return;

		string strMoveType = pMove->Get_SubStateMachine()->Get_CurrentStateName();

		if (strMoveType == "Walk")	// Walk -> NormalAttack
			m_pStateMachine->Set_Int("AttackEntryMode", 0);
		else if (strMoveType == "Run")
		{
			IHState<CJaneDoe>* pRun = dynamic_cast<IHState<CJaneDoe>*>(
				pMove->Get_SubStateMachine()->Get_CurrentState());
			if (pRun && pRun->Get_SubStateMachine())
			{
				string strRunTag = pRun->Get_SubStateMachine()->Get_CurrentStateName();
				if (strRunTag == "End") m_pStateMachine->Set_Int("AttackEntryMode", 0);
				else m_pStateMachine->Set_Int("AttackEntryMode", 1);
			}
			else return;
		}
		else return;

		m_pStateMachine->Set_Trigger("Attack");
	}
	else if (strCurrentState == "Attack")
	{	// Attack
		CJaneDoeState_Attack* pAttack = static_cast<CJaneDoeState_Attack*>(
			m_pStateMachine->Get_CurrentState());
		if (!pAttack || !pAttack->Get_SubStateMachine())
			return;

		if (pAttack->Get_SubStateMachine()->Get_CurrentStateName() != "NormalAttack")
			return;

		CJaneDoeState_NormalAttack* pNormal = static_cast<CJaneDoeState_NormalAttack*>(
			pAttack->Get_SubStateMachine()->Get_State("NormalAttack"));
		// NormalAttack : Combo
		if (pNormal && pNormal->Get_SubStateMachine())
			pNormal->Get_SubStateMachine()->Set_Trigger("NextCombo");
	}

}

void CJaneDoe::Process_EndState(const string& strCurrentState)
{
	if (strCurrentState == "Move")
	{
		CJaneDoeState_Move* pMove = static_cast<CJaneDoeState_Move*>(
			m_pStateMachine->Get_CurrentState());
		if (!pMove) return;

		IHState<CJaneDoe>* pMoveType = dynamic_cast<IHState<CJaneDoe>*>(
			pMove->Get_SubStateMachine()->Get_CurrentState());
		if (pMoveType && pMoveType->Is_EndState())
		{
			IBaseState<CJaneDoe>* pEnd = pMoveType->Get_SubStateMachine()->Get_CurrentState();
			if (m_bIsAttack || m_bIsEvade) return;
			if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
	else if (strCurrentState == "Attack")
	{
		CJaneDoeState_Attack* pAttack = static_cast<CJaneDoeState_Attack*>(
			m_pStateMachine->Get_CurrentState());
		if (!pAttack) return;

		IHState<CJaneDoe>* pAttackType = dynamic_cast<IHState<CJaneDoe>*>(
			pAttack->Get_SubStateMachine()->Get_CurrentState());
		if (pAttackType && pAttackType->Is_EndState())
		{
			IBaseState<CJaneDoe>* pEnd = pAttackType->Get_SubStateMachine()->Get_CurrentState();
			if (m_bIsEvade) return;
			if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
	else if (strCurrentState == "SwitchIn")
	{
		CJaneDoeState_SwitchIn* pSwitchIn = static_cast<CJaneDoeState_SwitchIn*>(
			m_pStateMachine->Get_CurrentState());
		if (!pSwitchIn) return;

		IHState<CJaneDoe>* pSwitchInType = dynamic_cast<IHState<CJaneDoe>*>(
			pSwitchIn->Get_SubStateMachine()->Get_CurrentState());
		if (pSwitchInType && pSwitchInType->Is_EndState())
		{
			IBaseState<CJaneDoe>* pEnd = pSwitchInType->Get_SubStateMachine()->Get_CurrentState();
			if (m_bIsEvade) return;
			if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
	else if (strCurrentState == "Hit")
	{
		CJaneDoeState_Hit* pHit = static_cast<CJaneDoeState_Hit*>(
			m_pStateMachine->Get_CurrentState());
		if (!pHit || !pHit->Get_SubStateMachine()) return;

		IBaseState<CJaneDoe>* pHitType = pHit->Get_SubStateMachine()->Get_CurrentState();
		if (pHitType && pHitType->Get_AnimProgress() > 0.3f)
		{
			if (m_bIsEvade) return;
			if (Is_Input() || pHitType->Is_AnimEnd())
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
}

CJaneDoe* CJaneDoe::Create()
{
	CJaneDoe* Instance = new CJaneDoe();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CJaneDoe::Clone(INIT_DESC* pArg)
{
	CJaneDoe* Instance = new CJaneDoe(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CJaneDoe::Free()
{
	Safe_Release(m_pStateMachine);
	__super::Free();
}
