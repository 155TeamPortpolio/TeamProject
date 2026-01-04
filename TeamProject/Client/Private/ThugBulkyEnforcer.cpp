#include "pch.h"
#include "ThugBulkyEnforcer.h"

#include "GameInstance.h"
#include "Material.h"
#include "Animator3D.h"
#include "SkeletalModel.h"
#include "CharacterController.h"
#include "Helper_Func.h"

/* States */
#include "StateMachine.h"
#include "ThugBulkyEnforcer_Idle.h"
#include "ThugBulkyEnforcer_Born.h"
#include "ThugBulkyEnforcer_Attack.h"
#include "ThugBulkyEnforcer_Move.h"



CThugBulkyEnforcer::CThugBulkyEnforcer()
	: CEnemy()
{
}

CThugBulkyEnforcer::CThugBulkyEnforcer(const CThugBulkyEnforcer& rhg)
	: CEnemy(rhg)
{
}

HRESULT CThugBulkyEnforcer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CAnimator3D>();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CCharacterController>();

	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResourceMgr->Add_ResourcePath("Monster_ThugBulkyEnforcer.mat", "../Bin/Resources/Model/skeletal/Enemy/ThugBulkyEnforcer/Monster_ThugBulkyEnforcer.mat");
	pResourceMgr->Add_ResourcePath("Monster_ThugBulkyEnforcer.model", "../Bin/Resources/Model/skeletal/Enemy/ThugBulkyEnforcer/Monster_ThugBulkyEnforcer.model");
	pResourceMgr->Add_ResourcePath("ThugBulkyEnforcer_Meta.json", "../Bin/Resources/Model/skeletal/Enemy/ThugBulkyEnforcer/ThugBulkyEnforcer_Meta.json");

	return S_OK;
}

HRESULT CThugBulkyEnforcer::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Monster_ThugBulkyEnforcer.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Monster_ThugBulkyEnforcer.mat");

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Monster_ThugBulkyEnforcer.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "ThugBulkyEnforcer_Meta.json");
	pAnimator->Set_MotionBone(3);	//Bip001
	pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;
	
	// ÀÓ½Ã È®ÀÎ¿ë
	CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = this;

	return S_OK;
}

void CThugBulkyEnforcer::Awake()
{
}

void CThugBulkyEnforcer::Priority_Update(_float dt)
{
}

void CThugBulkyEnforcer::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);


	Update_States(dt);
	m_pStateMachine->Update(dt);
}

void CThugBulkyEnforcer::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CThugBulkyEnforcer::Render_GUI()
{
	ImGui::PushID(this);

	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 5) + (ImGui::GetStyle().WindowPadding.y * 2);

	if (ImGui::TreeNode("Inspector##ThugBulkyInspector")) {
		__super::Render_GUI();
		ImGui::TreePop();
	}

	ImGui::BeginChild("State##ThugBulkyEnforcerStatus", ImVec2{ 0, childHeight + textLineHeight * 6}, true);
	string TagCurState = "Current State : " + m_pStateMachine->Get_CurrentStateName();
	ImGui::Text(TagCurState.c_str());
	string TagCurChildState = "Current ChildState : " + m_tAttackBlackBoard.currentStateTag;
	ImGui::Text(TagCurChildState.c_str());

	ImGui::SeparatorText("BlackBoard");
	for (size_t i = 0; i < m_tAttackBlackBoard.stateQueue.size(); i++)
	{
		if (i == 0)
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), m_tAttackBlackBoard.stateQueue[i].c_str());
		else
			ImGui::Text(m_tAttackBlackBoard.stateQueue[i].c_str());

	}

	ImGui::EndChild();
	ImGui::Checkbox("Auto Pattern", &m_isAutoPatternPlay);

	if(ImGui::TreeNode("Test State##ThugBulkyEnforcerTestState")) {
		ImGui::BeginChild("State##ThugBulkyEnforcerStatus", ImVec2{ 0, childHeight }, true);

		if (ImGui::Button(u8"1.¿À¸¥ÂÊ À§ºù ÈÄ ¿À¸¥ÂÊ ÈÅ")) {
			m_pStateMachine->Set_Int("AttackPattern", 1);
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		}
		if (ImGui::Button(u8"2.¿À¸¥¼Õ ¾îÆÛ ÈÄ ¿Þ¼Õ ³»·ÁÂï±â")) {
			m_pStateMachine->Set_Int("AttackPattern", 2);
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		}
		if (ImGui::Button(u8"3.ÇÃ¶óÀ× ´ÏÅ± ÈÄ ¾ç¼Õ ³»·ÁÂïÀ¸¸ç ÂøÁö")) {
			m_pStateMachine->Set_Int("AttackPattern", 3);
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		}
		if (ImGui::Button(u8"4.Å©°Ô ¿òÁ÷ÀÌ¸ç ¾ç¼ÕÀ¸·Î ¹Ù´Ú ³»·ÁÂï±â")) {
			m_pStateMachine->Set_Int("AttackPattern", 4);
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		}
		if (ImGui::Button(u8"5.¿À¸¥ÂÊ À§ºù ÈÄ ¿ÞÂÊ À§ºùÇÏ¸é¼­ ¿Þ¼Õ ÈÅ")) {
			m_pStateMachine->Set_Int("AttackPattern", 5);
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		}
		if (ImGui::Button(u8"6.¿ÞÂÊ À§ºù ÈÄ ¿À¸¥¼Õ ¾îÆÛ + ¿Þ¼Õ ³»·ÁÂï±â")) {
			m_pStateMachine->Set_Int("AttackPattern", 6);
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		}


		ImGui::EndChild();
		ImGui::TreePop();
	}


	ImGui::PopID();
}

CThugBulkyEnforcer* CThugBulkyEnforcer::Create()
{
	CThugBulkyEnforcer* instance = new CThugBulkyEnforcer();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CThugBulkyEnforcer");
	}

	return instance;
}

CGameObject* CThugBulkyEnforcer::Clone(INIT_DESC* pArg)
{
	CThugBulkyEnforcer* instance = new CThugBulkyEnforcer(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSacrificeHand");
	}

	return instance;
}

void CThugBulkyEnforcer::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}

/* For.State Machine */
HRESULT CThugBulkyEnforcer::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CThugBulkyEnforcer>::Create();
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

	Get_Component<CAnimator3D>()->Set_Animation("ThugBulkyEnforcer_Ani_Born")
		.Apply();

	return S_OK;
}

HRESULT CThugBulkyEnforcer::Initialize_States()
{
	m_pStateMachine->Register_State("Born", CThugBulkyEnforcer_Born::Create());
	m_pStateMachine->Register_State("Idle", CThugBulkyEnforcer_Idle::Create());
	m_pStateMachine->Register_State("Attack", CThugBulkyEnforcer_Attack::Create());
	m_pStateMachine->Register_State("Move", CThugBulkyEnforcer_Move::Create());


	return S_OK;
}

HRESULT CThugBulkyEnforcer::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Born", "Idle",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_ANIMATION_END);

	m_pStateMachine->Register_Transition("Idle", "Attack",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_TRIGGER, "Idle_To_Attack");

	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CThugBulkyEnforcer>::CONDITION_TRIGGER, "Idle_To_Move");

	return S_OK;
}

HRESULT CThugBulkyEnforcer::Ready_Rules()
{
	m_vIdleTime = { 3.f, 0.f };

	return S_OK;
}

void CThugBulkyEnforcer::Update_States(_float dt)
{
	if (true == m_isIdle) {
		m_pStateMachine->Change_State("Idle");
		m_pStateMachine->Reset_Trigger("Idle_To_Attack");
		m_pStateMachine->Reset_Trigger("Idle_To_Move");

		m_isIdle = false;
	}


	if (true == m_isAutoPatternPlay &&
		"Idle" == m_pStateMachine->Get_CurrentStateName()) {
		
		if (true == m_pStateMachine->Get_Bool("FinishAttack"))
			m_pStateMachine->Set_Trigger("Idle_To_Move");
		else
			m_pStateMachine->Set_Trigger("Idle_To_Attack");
		
	}
}

void CThugBulkyEnforcer::Test_State()
{
	

}
