#include "pch.h"
#include "Claymore.h"

#include "Helper_Func.h"
#include "GameInstance.h"
#include "BattleSystem.h"

/* Child */
#include "AttackSign.h"

/* Component */
#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "ObjectContainer.h"
#include "CharacterController.h"
#include "BoneFollower.h"
#include "AudioSource.h"
#include "EventListener.h"

/* States */
#include "StateMachine.h"
#include "Claymore_Attack.h"
#include "Claymore_Born.h"
#include "Claymore_Death.h"
#include "Claymore_Groggy.h"
#include "Claymore_Hit.h"
#include "Claymore_Idle.h"
#include "Claymore_Move.h"
#include "Claymore_Chase.h"
#include "Claymore_Parried.h"

#include "EffectContainer.h"

CClaymore::CClaymore()
	: CEnemyNormal()
{
}

CClaymore::CClaymore(const CClaymore& rhg)
	: CEnemyNormal(rhg)
{
}

HRESULT CClaymore::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();
	Add_Component<CAudioSource>();
	Add_Component<CEventListener>();

	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResourceMgr->Add_ResourcePath("Claymore.mat", "../Bin/Resources/Zero/Enemy/Claymore/Claymore.mat");
	pResourceMgr->Add_ResourcePath("Claymore.model", "../Bin/Resources/Zero/Enemy/Claymore/Claymore.model");
	//pResourceMgr->Add_ResourcePath("Monster_Claymore_Meta.json", "../../Resources/Data/Meta/Zero/Monster_Claymore_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Claymore.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Claymore.mat");

	return S_OK;
}

HRESULT CClaymore::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Claymore.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Monster_Claymore_Meta.json");
	pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
	pAnimator->Resize_Layer(2);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);

	if (FAILED(Ready_Children(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	if (FAILED(Initialize_Effects()))
		return E_FAIL;
	
	Get_Component<CAudioSource>()->SoundFolder(LevelManager()->Get_NowLevelKey() , "../Bin/Resources/Zero/Enemy/Claymore/Sound");
	Get_Component<CAudioSource>()->Slot("claymore_Spawn.wav").Attribute3D(true).Loop(false).Play();

	// 이벤트 : TUTORIAL_DESC
	Get_Component<CEventListener>()->Add_Listener<TUTORIAL_DESC>([&](const TUTORIAL_DESC& desc)
		{ 
			if (desc.eState == TUTORIAL_STATE::COMPLETED)
				m_isAutoPatternPlay = false;

			if (desc.eState != TUTORIAL_STATE::PLAY)
				return;

			SetTutorial(true);
			SetTutorialMode(desc.eType);
		});

	Get_Component<CEventListener>()->Add_Listener<TUTORIAL_DESC>([&](const TUTORIAL_DESC& desc)
		{
			if (desc.eState != TUTORIAL_STATE::COMPLETED)
				m_isAutoPatternPlay = true;
		});

	m_isUseGroggyRimLight = true;

	return S_OK;
}

void CClaymore::Awake()
{
	__super::Awake();
}

void CClaymore::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CClaymore::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);

	__super::Update(dt);

	ManageTutorialMode(dt);
	Update_States(dt);
	m_pStateMachine->Update(dt);
}

void CClaymore::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);

	__super::Late_Update(dt);
}

void CClaymore::Render_GUI()
{
	ImGui::PushID(this);

	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);

	GUI_DebugButton();

	if (ImGui::TreeNode("RimRight##RimRight")) 
	{
		if (ImGui::DragFloat("RimRight intensity##RimRightIntensity", &m_fRimLightPower, 0.1f))
		{
			if (m_fRimLightPower < 0.f)
				m_fRimLightPower = 0.f;
			//m_fRimLightPower = clamp(m_fRimLightPower, 0.f, 10.f);
		}

		_float startColor[3]= { m_vRimLightColor.x,m_vRimLightColor.y,m_vRimLightColor.z};

		if (ImGui::ColorEdit3("Start Color", startColor))
		{
			m_vRimLightColor = _float3(startColor[0], startColor[1], startColor[2]);
		}

		_float fPowerMinMax[2] = { m_tGroggyRimLight.vPower.x, m_tGroggyRimLight.vPower.y };
		if (ImGui::DragFloat2("RimRight Power MinMax##RimRightMinMax", fPowerMinMax, 0.1f))
		{
			if (fPowerMinMax[0] < 0.f)
				fPowerMinMax[0] = 0.f;
			if (fPowerMinMax[1] < fPowerMinMax[0])
				fPowerMinMax[1] = fPowerMinMax[0];
			
			m_tGroggyRimLight.vPower= { fPowerMinMax[0] , fPowerMinMax[1] };
			//m_fRimLightPower = clamp(m_fRimLightPower, 0.f, 10.f);
		}


		ImGui::TreePop();
	}

#pragma region Tutorial
	ImGui::SeparatorText("Tutorial");
	ImGui::Checkbox("Tutorial##SetTutorial", &m_isTutorial);
	const _char* charTutorialLabels[] =
	{
		"Extreme Evade",
		"Extreme Support",
		"Decibel Ultimate",
		"Groggy Combo",
	};

	_int kCount = static_cast<_int>(TUTORIAL_TYPE::END);
	_int cur = static_cast<_int>(m_eCurTutorial);
	if (cur < 0 || cur >= kCount) cur = 0;

	const _char* preview = charTutorialLabels[cur];
	if (ImGui::BeginCombo("Tutorial##SelectTutorialMode", preview))
	{
		for (int n = 0; n < kCount; ++n)
		{
			const bool selected = (cur == n);
			if (ImGui::Selectable(charTutorialLabels[n], selected))
			{
				cur = n;
				SetTutorialMode(static_cast<TUTORIAL_TYPE>(cur));
			}
			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

#pragma endregion

#pragma region Component Inspector
	if (ImGui::TreeNode("Inspector##ThugBulkyInspector")) {
		__super::Render_GUI();
		ImGui::TreePop();
	}
#pragma endregion
	
	{
		ImGui::SeparatorText("Damage Log");
		ImGui::BeginChild("TracePlayer##DamageLog", ImVec2{ 0, childHeight + textLineHeight * 6.f }, true);
		
		_int iPushed = {};
		if (true == m_isAccDamage)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.50f, 0.90f, 1.00f)); 
			iPushed++;
		}
		
		if (ImGui::Button(u8"누적##DamageLogAcc"))
			m_isAccDamage = !m_isAccDamage;
		
		if (iPushed)
			ImGui::PopStyleColor();
		
		ImGui::SameLine(0.f, 10.f);
		
		if (ImGui::Button("Clear##DamageLogClear"))
		{
			m_DamageLog.clear();
			m_fAccDamage = 0.f;
		}

		ImGui::Text(u8"누적 데미지 : %.2f", m_fAccDamage);

		ImGui::Separator();
		_int fShowLimit = {};
		for (auto& fDamage : m_DamageLog)
		{
			if (fShowLimit > 15)
				break;
			ImGui::Text("%.2f", fDamage); ++fShowLimit;
		}

		ImGui::EndChild();
	}

#pragma region Status
	ImGui::SeparatorText("Status");
	auto pCharacter = GetCharacterOnField();
	if (nullptr != pCharacter) {
		ImGui::BeginChild("TracePlayer##ThugAssaulterStatus", ImVec2{ 0, childHeight + textLineHeight * 6.f }, true);

		ImGui::Text(u8"속성 이상 가중치 : %.2f", m_tStatus.fPropertiesValue);
		ImGui::Text("RimLightPower : %.2f", m_fRimLightPower);
		ImGui::Text("AnimName : %s", Get_Component<CAnimator3D>()->Get_CurAnimName().c_str());
		ImGui::Text("SelfDir: %.2f, %.2f, %.2f", m_tTargetingInfo.vDirSelfLook.x, m_tTargetingInfo.vDirSelfLook.y, m_tTargetingInfo.vDirSelfLook.z);
		ImGui::Text("CaptureDir: %.2f, %.2f, %.2f", m_tRotDir.vDirToLookCapture.x, m_tRotDir.vDirToLookCapture.y, m_tRotDir.vDirToLookCapture.z);
		ImGui::Text("HP : %d", (_int)m_tStatus.iNowHP);
		ImGui::Text("Groggy Value : %d", m_tStatus.iGroggyValue);
		ImGui::Text("Groggy StayTime : %d", m_tGroggyManage.fGroggyStayTime);
		ImGui::Text("ComboCount : %d", m_tStatus.iPlayerComboCount);

		ImGui::BeginDisabled(true);
		//ImGui::Checkbox(u8"isLookPlayer", &m_isLookPlayer);
		ImGui::Checkbox("IsGroggy", &m_tStatus.isGroggy);
		ImGui::Checkbox("ForUI.IsGroggyStay", &m_tStatus.isGroggyStay);
		ImGui::Checkbox("IsOnAttack", &m_isOnAttack);
		ImGui::EndDisabled();

		ImGui::EndChild();
	}
#pragma endregion

#pragma region TargetInfo
	Render_GUI_ForTargetInfo();
#pragma endregion

#pragma region CheckState
	if (ImGui::TreeNode("Test State##ThugAssaulterCheckState")) {
		//ImGui::BeginChild("State##ThugBulkyEnforcerStatus", ImVec2{ 0, childHeight }, true);

		if (ImGui::TreeNode("AttackState##ThugAssaulterTestState_Attack"))
		{
			if (ImGui::Button(u8"1. Attack01"))
			{
				m_pStateMachine->Set_Int("AttackPattern", 1);
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
			}
			if (ImGui::Button(u8"2. Attack02"))
			{
				m_pStateMachine->Set_Int("AttackPattern", 2);
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
			}
			if (ImGui::Button(u8"3. Attack03"))
			{
				m_pStateMachine->Set_Int("AttackPattern", 3);
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Death##ThugAssaulterTestDeath"))
		{
			if (ImGui::Button("Death Front"))
				m_pStateMachine->Change_State("Death");

			if (ImGui::Button("Death Back"))
			{
				m_pStateMachine->Set_Bool("DeathBack", true);
				m_pStateMachine->Change_State("Death");
			}

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Groggy&Hit##ThugAssaulterTestGroggy&Hit"))
		{
			if (ImGui::Button("Increase Groggy value 30"))
				m_tStatus.iGroggyValue += 30;

			if (ImGui::Button("Hit"))
				TakeDamage(DAMAGE_TYPE::NORMAL, 20.f);

			if (ImGui::Button("Parried"))
				Parried();

			if (ImGui::Button("Execute"))
				m_tStatus.iNowHP -= m_tStatus.iMaxHP;



			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
#pragma endregion

#pragma region AutoPattern
	ImGui::Checkbox("Auto Pattern", &m_isAutoPatternPlay);
#pragma endregion

	_float fDeathDisappearProgress = m_pStateMachine->Get_Float("DeathDisappearProgress");
	if (ImGui::DragFloat("DeathDisappearProgress", &fDeathDisappearProgress, 0.01f))
		m_pStateMachine->Set_Float("DeathDisappearProgress", fDeathDisappearProgress);


	ImGui::PopID();
}

void CClaymore::OnPooledAcquire(INIT_DESC* pArg)
{
	Initialize(pArg);
}

void CClaymore::OnPooledRelease()
{
}

void CClaymore::Parried()
{
	if ("Attack" != m_pStateMachine->Get_CurrentStateName() || false == m_isParryEnable)
		return;

	__super::Parried();

	m_pStateMachine->Change_State("Parried");
	SetOnAttack(false, ATTACK_SIDE::NONE); 
}

HRESULT CClaymore::Ready_Children(INIT_DESC* pArg)
{
	BATTLE_COLLIDER_DESC WeaponDesc = {};

	WeaponDesc.tagName = "Weapon";
	WeaponDesc.isAttachBone = true;
	WeaponDesc.tagBone = "Bip001_L_Forearm";
	WeaponDesc.pOwnerAnimator3D = Get_Component<CAnimator3D>();
	WeaponDesc.eAttackColliderType = COLLIDER_TYPE::BOX;
	WeaponDesc.vCenter = { 0.8f, 0.f, 0.f };
	WeaponDesc.vAttackSize = { 1.8f, 0.3f, 0.3f };

	if (FAILED(AttachBattleColliderObject(&WeaponDesc)))
		return E_FAIL;

	Create_AttackSign("Bip001_Head");
	Create_UIEnemyStatus("Bip001_Spine2");
	Create_MeshPyramid();

	return S_OK;
}

CClaymore* CClaymore::Create()
{
	CClaymore* instance = new CClaymore();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CClaymore");
	}

	return instance;
}

CGameObject* CClaymore::Clone(INIT_DESC* pArg)
{
	CClaymore* instance = new CClaymore(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CClaymore");
	}

	return instance;
}

void CClaymore::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}


void CClaymore::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName)
{
	__super::TakeDamage(eDamageType, fDamage, charaName);
	
	// DamageLog
	{
		_float fResultDamage = m_tStatus.isGroggy == false ? fDamage : fDamage * 1.5f;
		m_DamageLog.push_front(fResultDamage);

		if (m_isAccDamage)
			m_fAccDamage += fResultDamage;
	}

	if (m_isTutorial)
	{
		m_tStatus.iNowHP = m_tStatus.iMaxHP;

		if (m_eCurTutorial != TUTORIAL_TYPE::GROGGY_COMBO)
			m_tStatus.iGroggyValue = 0.f;
	}

	if (0 >= m_tStatus.iNowHP)
		return;

	if ("Groggy" == m_pStateMachine->Get_CurrentStateName())
	{
		Get_Component<CAnimator3D>()->Set_Animation(1, "Claymore_Ani_Hit_Stay")
			.LayerBlend(1.f, 0.f, 1.f, EaseType::Linear)
			.Loop(false)
			.Apply();
	}
	else if ("Idle" == m_pStateMachine->Get_CurrentStateName())
	{
		m_pStateMachine->Set_Trigger("Idle_To_Hit");
		DIR eDir = GetDIRToPlayer();
		m_pStateMachine->Set_Int("Dir", ENUM(eDir));
	}
	else
	{
		Get_Component<CAnimator3D>()->Set_Animation(1, "Claymore_Ani_Hit_Stay")
			.LayerBlend(1.f, 0.f, 1.f, EaseType::Linear)
			.Loop(false)
			.Apply();
	}
}

void CClaymore::SetTutorialMode(TUTORIAL_TYPE eMode)
{
	if (!m_isTutorial || eMode == TUTORIAL_TYPE::END)
		return;

	m_eCurTutorial = eMode;
	m_isAutoPatternPlay = true;


	if (m_eCurTutorial == TUTORIAL_TYPE::GROGGY_COMBO)
	{
		m_eEnemyClass = ENEMY_CLASS::BOSS;
		m_tStatus.iGroggyValue = 99.f;
	}
	else
	{
		m_eEnemyClass = ENEMY_CLASS::NORMAL;
		m_tStatus.iGroggyValue = 0.f;
	}

	Get_Component<CCharacterController>()->Set_Position(XMVectorSet(-0.18f, 2.f, 1.59f, 1.f));
	m_pTransform->Rotate(_float3(0.f, XMConvertToRadians(180.f), 0.f));
}

/* For.State Machine */
HRESULT CClaymore::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CClaymore>::Create();
	if (nullptr == m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	if (FAILED(Ready_Rules()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Born");
	m_pStateMachine->Initialize(this);

	Get_Component<CAnimator3D>()->Set_Animation("Monster_Claymore_Ani_Born")
		.Apply();

	return S_OK;
}

HRESULT CClaymore::Initialize_States()
{
	m_pStateMachine->Register_State("Born", CClaymore_Born::Create());
	m_pStateMachine->Register_State("Idle", CClaymore_Idle::Create());
	m_pStateMachine->Register_State("Attack", CClaymore_Attack::Create());
	m_pStateMachine->Register_State("Move", CClaymore_Move::Create());
	m_pStateMachine->Register_State("Chase", CClaymore_Chase::Create());
	m_pStateMachine->Register_State("Death", CClaymore_Death::Create());
	m_pStateMachine->Register_State("Groggy", CClaymore_Groggy::Create());
	m_pStateMachine->Register_State("Hit", CClaymore_Hit::Create());
	m_pStateMachine->Register_State("Parried", CClaymore_Parried::Create());

	return S_OK;
}

HRESULT CClaymore::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CClaymore>::CONDITION_ANIMATION_END);
	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CClaymore>::CONDITION_TRIGGER, "Idle_To_Attack");
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CClaymore>::CONDITION_TRIGGER, "Idle_To_Move");
	m_pStateMachine->Register_Transition("Idle", "Chase",
		CStateMachine<CClaymore>::CONDITION_TRIGGER, "Idle_To_Chase");
	m_pStateMachine->Register_Transition("Idle", "Death",
		CStateMachine<CClaymore>::CONDITION_TRIGGER, "Idle_To_Death");
	m_pStateMachine->Register_Transition("Idle", "Groggy",
		CStateMachine<CClaymore>::CONDITION_TRIGGER, "Idle_To_Groggy");
	m_pStateMachine->Register_Transition("Idle", "Hit",
		CStateMachine<CClaymore>::CONDITION_TRIGGER, "Idle_To_Hit");

	return S_OK;
}

HRESULT CClaymore::Initialize_Effects()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	for (_uint i = 0; i < 3; ++i)
	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("claymore_slash0.json")
			.Build("Claymore_Slash0_" + to_string(i));
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}

	return S_OK;
}

HRESULT CClaymore::Ready_Rules()
{
	// x = Idle에서 다음 상태로 넘어가는 쿨타임, y = dt 더한 타이머용
	m_vIdleTime = { 0.2f, 0.f };

	m_tHysteriesis.fEvadeEnter = 2.f;
	m_tHysteriesis.fComboEnter = 2.5f;
	m_tHysteriesis.fComboExit = 3.5f;
	m_tHysteriesis.fChaseEnter = 5.f;
	m_tHysteriesis.fChaseExit = 4.f;

	return S_OK;
}

void CClaymore::Update_States(_float dt)
{
	if (true == m_isIdle) {
		m_pStateMachine->Change_State("Idle");
		m_pStateMachine->Reset_Trigger("Idle_To_Attack");
		m_pStateMachine->Reset_Trigger("Idle_To_Move");
		m_pStateMachine->Reset_Trigger("Idle_To_Chase");
		m_pStateMachine->Reset_Trigger("Idle_To_Death");
		m_pStateMachine->Reset_Trigger("Idle_To_Hit");

		m_isIdle = false;
	}

	CheckDistanceFromPlayer();

	//================================
	ControlState(dt);
	//================================
}

void CClaymore::ControlState(const _float dt)
{
	if (!m_isTutorial)
	{
		if ("Death" != m_pStateMachine->Get_CurrentStateName() &&
			0 >= m_tStatus.iNowHP)
		{
			RequestRemoveOnDeathToBattleSystem();
			m_pStateMachine->Change_State("Death");
		}

		if ("Death" != m_pStateMachine->Get_CurrentStateName() &&
			"Groggy" != m_pStateMachine->Get_CurrentStateName() &&
			true == m_tStatus.isGroggy)
			m_pStateMachine->Change_State("Groggy");
	}

	if (true == m_isAutoPatternPlay &&
		"Idle" == m_pStateMachine->Get_CurrentStateName())
	{
		// 튜토리얼 중 그로기와 궁극기할 땐 상태변경 X
		if (m_isTutorial &&
			(m_eCurTutorial == TUTORIAL_TYPE::GROGGY_COMBO || m_eCurTutorial == TUTORIAL_TYPE::DECIBEL_ULTIMATE))
			return;

		m_vIdleTime.y += dt;

		if (m_vIdleTime.x <= m_vIdleTime.y)
		{
			if (true == m_pStateMachine->Get_Bool("Chase"))
				m_pStateMachine->Set_Trigger("Idle_To_Chase");
			//else if (true == m_pStateMachine->Get_Bool("Death"))
			//	m_pStateMachine->Set_Trigger("Idle_To_Death");
			else if ("Attack" != m_pStateMachine->Get_PrevStateName())
				m_pStateMachine->Set_Trigger("Idle_To_Attack");
			else
				m_pStateMachine->Set_Trigger("Idle_To_Move");


			m_vIdleTime.y = 0.f;
		}
	}
}

void CClaymore::CheckDistanceFromPlayer()
{
	if ("Chase" != m_pStateMachine->Get_CurrentStateName() &&
		m_tTargetingInfo.fDistance >= m_tHysteriesis.fChaseEnter)
		m_pStateMachine->Set_Bool("Chase", true);

	if (true == m_pStateMachine->Get_Bool("Chase") &&
		m_tTargetingInfo.fDistance <= m_tHysteriesis.fChaseExit)
		m_pStateMachine->Set_Bool("Chase", false);
}

void CClaymore::ManageTutorialMode(const _float dt)
{
	if (!m_isTutorial)
		return;

 	if (m_eCurTutorial == TUTORIAL_TYPE::GROGGY_COMBO)
	{
		if (m_isPrevGroggy == true && m_tStatus.isGroggy == false)
			m_isTutorialGroggyCool = true;

		if (m_isTutorialGroggyCool)
		{
			m_vTutorialGroggyTime.y += dt;

			if (m_vTutorialGroggyTime.x <= m_vTutorialGroggyTime.y)
			{
				m_vTutorialGroggyTime.y = 0.f;
				m_tStatus.iGroggyValue = 99.f;
				m_isTutorialGroggyCool = false;
			}
		}

		m_isPrevGroggy = m_tStatus.isGroggy;

	}

}
