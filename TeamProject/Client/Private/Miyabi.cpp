#include "pch.h"
#include "Miyabi.h"

#include "GameInstance.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"

#include "DataBase.h"
#include "EffectContainer.h"

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "MaterialInstance.h"

#include "SkeletalModel.h"
#include "Animator3D.h"
#include "CharacterController.h"
#include "ObjectContainer.h"

#include "StateMachine.h"
#include "MiyabiState_Start.h"
#include "MiyabiState_Idle.h"
#include "MiyabiState_Move.h"
#include "MiyabiState_Attack.h"
#include "MiyabiState_NormalAttack.h"
#include "MiyabiState_CounterAttack.h"
#include "MiyabiState_AssaultAttack.h"
#include "MiyabiState_SwitchIn.h"
#include "MiyabiState_SwitchInParryAid.h"
#include "MiyabiState_SwitchOut.h"
//#include "MiyabiState_Hit.h"
#include "MiyabiState_Evade.h"

CMiyabi::CMiyabi()
{
}

CMiyabi::CMiyabi(const CMiyabi& rhs)
	: CCharacter(rhs)
{
}

HRESULT CMiyabi::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Get_Component<CModel>()->Link_Model(G_GlobalLevelKey, "Miyabi.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Miyabi.mat");

	return S_OK;
}

HRESULT CMiyabi::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	if (FAILED(Initialize_Weapon()))
		return E_FAIL;

	return S_OK;
}

void CMiyabi::Awake()
{
	__super::Awake();

	m_pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Miyabi.model");
	m_pAnimator->Link_MetaData(G_GlobalLevelKey, "Miyabi_Meta.json");
	m_pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
	m_strAnimName = "Avatar_Female_Size02_Unagi_Ani_";
	m_pAnimator->Set_Animation(Get_Name() + "Idle")
		.Loop(true)
		.Apply();

	m_strName = "Miyabi";
	m_eCharacterName = CHARACTER::Miyabi;

	Initialize_Stat();
	m_fCurrentHP = 312.f;
	m_tEnergy.fCurrentEnergy = 75;
	m_iFrost = 6;

	if (FAILED(Attach_ParryCollider()))
		return;
}

void CMiyabi::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CMiyabi::Update(_float dt)
{
	if (!m_bTest)
	{
		Update_States();
		m_pStateMachine->Update(dt);
	}
	__super::Update(dt);
}

void CMiyabi::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

void CMiyabi::Render_GUI()
{
	ImGui::Separator();
	ImGui::Text("FrostCount : %d", m_iFrost);
	if (ImGui::Button("Max"))
		Increase_Frost(MAX_FROST);
	ImGui::SameLine();
	if (ImGui::Button("Up"))
		Increase_Frost(1);
	ImGui::SameLine();
	if (ImGui::Button("Down"))
		Decrease_Frost(1);
	ImGui::SameLine();
	if (ImGui::Button("Zero"))
		Decrease_Frost(MAX_FROST);

	if (m_pStateMachine)
	{
		ImGui::Separator();
		ImGui::Checkbox("Animation Test", &m_bTest);
		ImGui::Text("StateMachine: %s", m_pStateMachine->Get_CurrentStateName().c_str());

		if (ImGui::Button("Open StateMachine"))
			m_pStateMachine->Set_ShowWindow(true);

		m_pStateMachine->Render_GUI();
	}

	__super::Render_GUI();
}

_bool CMiyabi::Can_Evade()
{
	if (m_pStateMachine->Get_Bool("InDash02"))
		return false;

	return __super::Can_Evade();
}

//void CMiyabi::Render_OutLine(ID3D11DeviceContext* pContext, _uint idx)
//{
//	auto RenderSys = CGameInstance::GetInstance()->Get_RenderSystem()->GetRenderer(RENDERER_TYPE::FORWARD);
//	auto Model = Get_Component<CSkeletalModel>();
//	auto Material = Get_Component<CMaterial>();
//
//	_int Index = Model->Get_MaterialIndex(idx);
//	auto Shader = Material->Get_Shader(Index);
//	ID3D11InputLayout* pLayout;
//	RenderSys->Get_InputLayout(
//		Model,
//		Shader,
//		idx,
//		"OutLine",
//		&pLayout
//	);
//
//	//Get_Component<CMaterial>()->Set_OutLineInfo(_float4(0.27f, 0.27f, 0.27f, 1.0f), 0.001f);
//
//	pContext->IASetInputLayout(pLayout);
//	Shader->Apply("OutLine", pContext);
//	Model->Draw(pContext, idx);
//}

void CMiyabi::Increase_Frost(_uint iFrost)
{
	m_iFrost += iFrost;
	m_iFrost = min(m_iFrost, MAX_FROST);
}

void CMiyabi::Decrease_Frost(_uint iFrost)
{
	m_iFrost -= iFrost;
	m_iFrost = max(m_iFrost, 0.f);
}

void CMiyabi::Reset_State()
{
	m_pStateMachine->Set_Trigger("ResetState");
}

void CMiyabi::On_Start()
{
	m_pStateMachine->Set_Trigger("QuestStart");
}

void CMiyabi::On_SwitchIn(SWITCH eType)
{
	m_fDissolveProgress = 0.f;
	SetRenderLayer(RENDER_LAYER::Default);

	Set_Switch(eType);
	m_pStateMachine->Set_Trigger("SwitchIn");
}

void CMiyabi::On_ChainParry()
{
	m_pStateMachine->Set_Int("IdleEntryMode", 2);
	m_pStateMachine->Set_Trigger("ToIdle");
}

void CMiyabi::On_SwitchOut()
{
	__super::On_SwitchOut();

	if (m_pStateMachine->Get_CurrentStateName() == "Attack")
	{
		m_pStateMachine->Set_Bool("OutReserve", true);
		return;
	}
	else if (m_pStateMachine->Get_CurrentStateName() == "SwitchIn")
	{
		IHState<CMiyabi>* pState = dynamic_cast<IHState<CMiyabi>*>(m_pStateMachine->Get_CurrentState());
		CStateMachine<CMiyabi>* pSub = pState->Get_SubStateMachine();
		if (pSub && pSub->Get_CurrentStateName() != "SwitchInNormal")
		{
			m_pStateMachine->Set_Bool("OutReserve", true);
			return;
		}
	}
	m_pStateMachine->Set_Trigger("SwitchOut");
}

void CMiyabi::On_Ultimate()
{
	IHState<CMiyabi>* pState = dynamic_cast<IHState<CMiyabi>*>(m_pStateMachine->Get_CurrentState());
	if (pState)
	{
		CStateMachine<CMiyabi>* pSub = pState->Get_SubStateMachine();
		if (pSub && pSub->Get_CurrentStateName() == "UltimateAttack")
			return;
	}

	__super::On_Ultimate();
	m_pStateMachine->Set_Int("AttackEntryMode", 3);
	m_pStateMachine->Set_Trigger("Attack");
}

void CMiyabi::On_Special()
{
	if (InputDevice()->Key_Tap('E') == false) return;

	string strCurrentState = m_pStateMachine->Get_CurrentStateName();

	// NormalAttack 중 캔슬해서 ExAttack
	if (strCurrentState == "Attack")
	{
		CMiyabiState_Attack* pAttack = static_cast<CMiyabiState_Attack*>(
			m_pStateMachine->Get_CurrentState());
		if (pAttack && pAttack->Get_SubStateMachine())
		{
			string strAttackType = pAttack->Get_SubStateMachine()->Get_CurrentStateName();

			if (strAttackType == "NormalAttack")
			{
				pAttack->Get_SubStateMachine()->Set_Trigger("ToExAttack");
				return;
			}
			// NormalAttack이 아니면 아무것도 안함 (ExAttack 포함)
			return;
		}
	}

	m_pStateMachine->Set_Int("AttackEntryMode", 2);
	m_pStateMachine->Set_Trigger("Attack");
}

void CMiyabi::On_Hit(DAMAGE_TYPE eType)
{
	m_pStateMachine->Set_Int("HitEntryMode", ENUM(eType));
	m_pStateMachine->Set_Trigger("ToHit");
}

void CMiyabi::OnDamage()
{
}

void CMiyabi::OnPerfectDodge()
{
}

void CMiyabi::OnDefensiveAssist()
{
}

HRESULT CMiyabi::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CMiyabi>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	if (FAILED(Initialize_Effects()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Idle");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CMiyabi::Initialize_States()
{
	m_pStateMachine->Register_State("Start", CMiyabiState_Start::Create());
	m_pStateMachine->Register_State("Idle", CMiyabiState_Idle::Create());
	m_pStateMachine->Register_State("Move", CMiyabiState_Move::Create());
	m_pStateMachine->Register_State("Attack", CMiyabiState_Attack::Create());
	m_pStateMachine->Register_State("Evade", CMiyabiState_Evade::Create());
	m_pStateMachine->Register_State("SwitchIn", CMiyabiState_SwitchIn::Create());
	m_pStateMachine->Register_State("SwitchOut", CMiyabiState_SwitchOut::Create());
	//m_pStateMachine->Register_State("Hit", CMiyabiState_Hit::Create());

	return S_OK;
}

HRESULT CMiyabi::Initialize_Transitions()
{
	// Start
	m_pStateMachine->Register_AnyStateTransition("Start",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "QuestStart");

	m_pStateMachine->Register_Transition("Start", "Idle",
		CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

	// Idle -> Move
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CMiyabi>::CONDITION_BOOL_TRUE, "IsMove");

	// Move -> Idle
	m_pStateMachine->Register_Transition("Move", "Idle",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToIdle");

	m_pStateMachine->Register_AnyStateTransition("Idle",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ResetState");

	// Attack
	m_pStateMachine->Register_AnyStateTransition("Attack",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Attack");

	// Attack -> Idle
	m_pStateMachine->Register_Transition("Attack", "Idle",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToIdle");

	// Evade
	m_pStateMachine->Register_AnyStateTransition("Evade",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToEvade");

	// Evade -> Move (Dash)
	m_pStateMachine->Register_Transition("Evade", "Move",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToMove");

	// Evade -> Idle (Backstep)
	m_pStateMachine->Register_Transition("Evade", "Idle",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToIdle");

	// SwitchIn
	m_pStateMachine->Register_AnyStateTransition("SwitchIn",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "SwitchIn");

	m_pStateMachine->Register_Transition("SwitchIn", "Idle",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToIdle");

	m_pStateMachine->Register_Transition("SwitchIn", "Move",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToMove");

	// SwitchOut
	m_pStateMachine->Register_AnyStateTransition("SwitchOut",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "SwitchOut", 1);

	m_pStateMachine->Register_Transition("SwitchOut", "Idle",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToIdle");

	// Hit
	vector<CStateMachine<CMiyabi>::CONDITION_INFO> HitConditions;
	HitConditions.push_back({ CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToHit" });
	HitConditions.push_back({ CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "Resistance" });
	m_pStateMachine->Register_AnyStateTransition("Hit", HitConditions);

	m_pStateMachine->Register_Transition("Hit", "Idle",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "ToIdle");

	return S_OK;
}

HRESULT CMiyabi::Initialize_Stat()
{
	auto Desc = CDataBase::GetInstance()->GetPlayerDesc(m_strName);
	m_fMaxHP = Desc.MaxHP;
	m_fAttackPower = Desc.Attack;
	m_fDefense = Desc.Defend;
	m_tEnergy.fSpecialEnergy = Desc.SpecialAttack;
	Set_EvadeMax(2);

	// 추가 버프 적용
	string outID;
	RuntimeBucket().String.TryGet(PersistScope::SaveSlot, "RamenID", outID);
	if (!outID.empty())
	{
		auto Ramen = CDataBase::GetInstance()->GetRamenDesc(outID);
		for (auto attribute : Ramen.attributes)
		{
			string attID = attribute.strAttributeID;
			if (attID == "atk")
			{
				m_fAttackPower += attribute.iAttributeValue * 0.01f;
			}
			else if (attID == "max_hp")
			{
				m_fMaxHP += attribute.iAttributeValue;
			}
			else if (attID == "dmg_ice")
			{
				m_fAttackPower += attribute.iAttributeValue * 0.01f;
			}
			else
				continue;
		}
	}

	return S_OK;
}

HRESULT CMiyabi::Initialize_Weapon()
{
	ATTACK_COLLIDER_DESC KatanaDesc;
	KatanaDesc.eColliderType = COLLIDER_TYPE::BOX;
	KatanaDesc.pOwnerAnimator = m_pAnimator;
	KatanaDesc.tagBone = "Bn_katana_burst_eye";
	KatanaDesc.tagName = "KatanaWeapon";
	KatanaDesc.vCenter = { 0.2f,-0.3f,0.015f };
	KatanaDesc.vSize = { 0.3f,1.3f,0.3f };
	KatanaDesc.vRotation = { 0.f,0.f,0.69f };
	if (FAILED(Attach_AttackCollider(&KatanaDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiyabi::Initialize_Effects()
{
	if (FAILED(__super::Initialize_Effects()))
		return E_FAIL;

	auto pObjectContainer = Get_Component<CObjectContainer>();
	auto pAnimator = Get_Component<CAnimator3D>();

	// Sword Fire
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_sword_fire.json")
			.Build("Miyabi_Sword_Fire");
		pObjectContainer->Add_Child(pEffect, false);
		pEffect->AttachBone(pAnimator, "Bn_Weapon");
	}

	// Normal Slash0
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal1_slash.json")
			.Build("Miyabi_Normal0_Slash0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}

	// Normal Slash1
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal2_slash.json")
			.Build("Miyabi_Normal1_Slash0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal2_slash.json")
			.Build("Miyabi_Normal1_Slash1");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}

	// Normal Slash2
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal3_slash.json")
			.Build("Miyabi_Normal2_Slash0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}

	// Normal Slash3
	for (_uint i = 0; i < 9; ++i)
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal4_slash.json")
			.Build("Miyabi_Normal3_Slash" + to_string(i));
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}

	// Normal Sting
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal1_sting.json")
			.Build("Miyabi_Normal0_Sting0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect,false);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal1_sting.json")
			.Build("Miyabi_Normal0_Sting1");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect,false);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal1_sting.json")
			.Build("Miyabi_Normal0_Sting2");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect,false);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal1_sting.json")
			.Build("Miyabi_Normal0_Sting3");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect,false);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal1_sting.json")
			.Build("Miyabi_Normal0_Sting4");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect,false);
	}

	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal2_sting.json")
			.Build("Miyabi_Normal1_Sting0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect, false);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal2_sting.json")
			.Build("Miyabi_Normal1_Sting1");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect, false);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal2_sting.json")
			.Build("Miyabi_Normal1_Sting2");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect, false);
	}
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_normal2_sting.json")
			.Build("Miyabi_Normal1_Sting3");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect, false);
	}

	// Ex Slash
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_ex0_slash.json")
			.Build("Miyabi_Ex0_Slash0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}

	// Ex Sting
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_ex0_sting.json")
			.Build("Miyabi_Ex0_Sting0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect, false);
	}

	// Rush Sting
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("miyabi_rush0_sting.json")
			.Build("Miyabi_Rush0_Sting0");
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect, false);
	}

	return S_OK;
}

void CMiyabi::Update_States()
{
	if (!Is_MainCharacter()) return;
	if (!m_pCCT->Get_CompActive()) return;

	for (const auto& Event : Get_Animator()->Get_EventBus())
	{
		if (Event.Type != CLIP_EVENT_TYPE::NOTIFY) continue;
		if (Event.Tag == "CheckCombo")
		{
			if (m_bReserveCombo)
			{
				m_bReserveCombo = false;
				BattleSystem()->GetBattlePlayer()->Request_ComboAttack();
			}
		}
	}

	m_pStateMachine->Set_Bool("IsMove", Is_Move_Buffer());

	Process_EndState(m_pStateMachine->Get_CurrentStateName());

	if (m_bIsEvade)
		m_pStateMachine->Set_Trigger("ToEvade");

	if (m_bIsAttack)
		Process_AttackInput(m_pStateMachine->Get_CurrentStateName());
}

void CMiyabi::Process_AttackInput(const string& strCurrentState)
{
	if (strCurrentState == "Idle")
	{
		m_pStateMachine->Set_Int("AttackEntryMode", 0);
		m_pStateMachine->Set_Trigger("Attack");
	}
	else if (strCurrentState == "Move")
	{
		CMiyabiState_Move* pMove = static_cast<CMiyabiState_Move*>(
			m_pStateMachine->Get_CurrentState());
		if (!pMove || !pMove->Get_SubStateMachine())
			return;

		string strMoveType = pMove->Get_SubStateMachine()->Get_CurrentStateName();

		if (strMoveType == "Walk")
			m_pStateMachine->Set_Int("AttackEntryMode", 0);
		else if (strMoveType == "Run")
		{
			IHState<CMiyabi>* pRun = dynamic_cast<IHState<CMiyabi>*>(
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
	{
		CMiyabiState_Attack* pAttack = static_cast<CMiyabiState_Attack*>(
			m_pStateMachine->Get_CurrentState());
		if (!pAttack || !pAttack->Get_SubStateMachine())
			return;

		string strAttackType = pAttack->Get_SubStateMachine()->Get_CurrentStateName();

		if (strAttackType == "NormalAttack")
		{
			CMiyabiState_NormalAttack* pNormal = static_cast<CMiyabiState_NormalAttack*>(
				pAttack->Get_SubStateMachine()->Get_State("NormalAttack"));
			if (pNormal && pNormal->Get_SubStateMachine())
				pNormal->Get_SubStateMachine()->Set_Trigger("NextCombo");
		}
		else if (strAttackType == "CounterAttack")
		{
			CMiyabiState_CounterAttack* pCounter = static_cast<CMiyabiState_CounterAttack*>(
				pAttack->Get_SubStateMachine()->Get_CurrentState());
			if (!pCounter || !pCounter->Get_SubStateMachine())
				return;
			if (!pCounter->Is_EndState())
			{
				pAttack->Get_SubStateMachine()->Set_Int("ComboEntryIndex", 3);
				pCounter->Get_SubStateMachine()->Set_Bool("ReserveNormal", true);
			}
		}
		else if (strAttackType == "AssaultAttack")
		{
			CMiyabiState_AssaultAttack* pAssault = static_cast<CMiyabiState_AssaultAttack*>(
				pAttack->Get_SubStateMachine()->Get_CurrentState());
			if (!pAssault || !pAssault->Get_SubStateMachine())
				return;
			pAttack->Get_SubStateMachine()->Set_Int("ComboEntryIndex", 3);
			pAssault->Get_SubStateMachine()->Set_Bool("ReserveNormal", true);
		}
	}
	else if (strCurrentState == "SwitchIn")
	{
		CMiyabiState_SwitchIn* pSwitchIn = static_cast<CMiyabiState_SwitchIn*>(
			m_pStateMachine->Get_CurrentState());
		if (!pSwitchIn || !pSwitchIn->Get_SubStateMachine())
			return;

		string strSwitchType = pSwitchIn->Get_SubStateMachine()->Get_CurrentStateName();
		if (strSwitchType == "SwitchInParryAid")
		{
			CMiyabiState_SwitchInParryAid* pParryAid = static_cast<CMiyabiState_SwitchInParryAid*>(
				pSwitchIn->Get_SubStateMachine()->Get_CurrentState());
			if (!pSwitchIn || !pSwitchIn->Get_SubStateMachine())
				return;
			if (!pParryAid->Is_EndState())
			{
				pParryAid->Get_SubStateMachine()->Set_Bool("ReserveAssaultAid", true);
			}
		}
	}
}

void CMiyabi::Process_EndState(const string& strCurrentState)
{
	if (strCurrentState == "Move")
	{
		CMiyabiState_Move* pMove = static_cast<CMiyabiState_Move*>(
			m_pStateMachine->Get_CurrentState());
		if (!pMove) return;

		IHState<CMiyabi>* pMoveType = dynamic_cast<IHState<CMiyabi>*>(
			pMove->Get_SubStateMachine()->Get_CurrentState());
		if (pMoveType && pMoveType->Is_EndState())
		{
			IBaseState<CMiyabi>* pEnd = pMoveType->Get_SubStateMachine()->Get_CurrentState();
			if (m_bIsAttack || m_bIsEvade) return;
			if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
	else if (strCurrentState == "Attack")
	{
		CMiyabiState_Attack* pAttack = static_cast<CMiyabiState_Attack*>(
			m_pStateMachine->Get_CurrentState());
		if (!pAttack) return;

		IHState<CMiyabi>* pAttackType = dynamic_cast<IHState<CMiyabi>*>(
			pAttack->Get_SubStateMachine()->Get_CurrentState());
		if (pAttackType && pAttackType->Is_EndState())
		{
			IBaseState<CMiyabi>* pEnd = pAttackType->Get_SubStateMachine()->Get_CurrentState();
			if (m_bIsEvade) return;
			if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
	else if (strCurrentState == "SwitchIn")
	{
		CMiyabiState_SwitchIn* pSwitchIn = static_cast<CMiyabiState_SwitchIn*>(
			m_pStateMachine->Get_CurrentState());
		if (!pSwitchIn) return;

		IHState<CMiyabi>* pSwitchInType = dynamic_cast<IHState<CMiyabi>*>(
			pSwitchIn->Get_SubStateMachine()->Get_CurrentState());
		if (pSwitchInType && pSwitchInType->Is_EndState())
		{
			IBaseState<CMiyabi>* pEnd = pSwitchInType->Get_SubStateMachine()->Get_CurrentState();
			if (m_bIsEvade) return;
			if (pEnd && (Is_Input() || pEnd->Is_AnimEnd()))
				m_pStateMachine->Set_Trigger("ToIdle");
		}
	}
	//else if (strCurrentState == "Hit")
	//{
	//	CMiyabiState_Hit* pHit = static_cast<CMiyabiState_Hit*>(
	//		m_pStateMachine->Get_CurrentState());
	//	if (!pHit || !pHit->Get_SubStateMachine()) return;

	//	IBaseState<CMiyabi>* pHitType = pHit->Get_SubStateMachine()->Get_CurrentState();
	//	if (pHitType && pHitType->Get_AnimProgress() > 0.3f)
	//	{
	//		if (m_bIsEvade) return;
	//		if (Is_Input() || pHitType->Is_AnimEnd())
	//			m_pStateMachine->Set_Trigger("ToIdle");
	//	}
	//}
}

//HRESULT CMiyabi::Add_OutLineRender()
//{
//	auto Model = Get_Component<CSkeletalModel>();
//	//_uint size = sizeof(_float4x4) * m_pAnimator->Get_CombinedBoneMatrices().size();
//	_uint size = sizeof(_float4x4) * m_pAnimator->Get_BoneMatrices(CAnimator3D::BoneSpace::COMBINED).size();
//
//	for (_int i = 0; i < Model->Get_MeshCount(); ++i)
//	{
//		vector<_float4x4> BoneMatrices = m_pAnimator->Get_BoneMatrices(i);
//		OUTLINE_COMMAND Command =
//		{
//			Get_Component<CMaterial>()->Get_Shader(Model->Get_MaterialIndex(i)),
//			m_pTransform->Get_WorldMatrix_Ptr(),
//			BoneMatrices,
//			"float4x4[]",
//			size ,
//			i,
//			[this](ID3D11DeviceContext* pContext, _uint index) {Render_OutLine(pContext,index); }
//		};
//		CGameInstance::GetInstance()->Get_RenderSystem()->Add_OutLineCommand(Command);
//	}
//	return S_OK;
//}

CMiyabi* CMiyabi::Create()
{
	CMiyabi* pInstance = new CMiyabi();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CMiyabi::Clone(INIT_DESC* pArg)
{
	CMiyabi* pInstance = new CMiyabi(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CMiyabi::Free()
{
	Safe_Release(m_pStateMachine);
	__super::Free();
}
