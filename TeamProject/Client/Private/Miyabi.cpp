#include "pch.h"
#include "Miyabi.h"
#include "GameInstance.h"

#include "StateMachine.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

#include "MiyabiState_Idle.h"
#include "MiyabiState_Walk.h"

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

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size02_Unagi.model",
		"../Bin/Resources/Model/skeletal/Miyabi/Avatar_Female_Size02_Unagi.model");
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size02_Unagi.mat",
		"../Bin/Resources/Model/skeletal/Miyabi/Avatar_Female_Size02_Unagi.mat");
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size02_Unagi_Meta.json",
		"../Bin/Resources/Model/skeletal/Miyabi/Anim/Avatar_Female_Size02_Unagi_Meta.json");

	Get_Component<CModel>()->Link_Model("Test_Level", "Avatar_Female_Size02_Unagi.model");
	Get_Component<CMaterial>()->Link_Material("Test_Level", "Avatar_Female_Size02_Unagi.mat");


	return S_OK;
}

HRESULT CMiyabi::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CMiyabi::Awake()
{
	Get_Component<CAnimator3D>()->LinkAnimate_Model("Test_Level", "Avatar_Female_Size02_Unagi.model");
	Get_Component<CAnimator3D>()->Link_MetaData("Test_Level", "Avatar_Female_Size02_Unagi_Meta.json");
	Get_Component<CAnimator3D>()->Set_NoTransform(21);
	Get_Component<CAnimator3D>()->Set_Animation("Avatar_Female_Size02_Unagi_Ani_Idle")
		.Loop(true)
		.Apply();
	Get_Component<CCharacterController>()->Set_GravityEnabled(true);
}

void CMiyabi::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CMiyabi::Update(_float dt)
{
	Update_Input(dt);

	if (m_pStateMachine)
		m_pStateMachine->Update(dt);

	__super::Update(dt);
}

void CMiyabi::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

void CMiyabi::Render_GUI()
{
	__super::Render_GUI();
	// StateMachine µð¹ö±ë Á¤º¸
	if (m_pStateMachine)
	{
		ImGui::Separator();
		ImGui::Text("Current State: %s", m_pStateMachine->Get_CurrentStateName().c_str());
		ImGui::Text("State Time: %.2f", m_pStateMachine->Get_StateTime());
	}
	_bool isLayer = Get_Layer();
	ImGui::Checkbox("InLayer", &isLayer);
}

void CMiyabi::Update_Input(_float dt)
{
	__super::Update_Input(dt);

	m_pStateMachine->Set_Bool("IsMove", m_bIsMove);
	m_pStateMachine->Set_Bool("IsGround", m_bIsGround);

	if (m_bIsJump)
		m_pStateMachine->Set_Trigger("Jump");

	if (m_pStateMachine) m_pStateMachine->Update(dt);
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

	m_pStateMachine->Set_DefaultState("Idle");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CMiyabi::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CMiyabiState_Idle::Create());
	m_pStateMachine->Register_State("Walk", CMiyabiState_Walk::Create());
	//m_pStateMachine->Register_State("Jump", CMiyabiState_Jump::Create());

	return S_OK;
}

HRESULT CMiyabi::Initialize_Transitions()
{
	// Idle <-> Walk
	m_pStateMachine->Register_Transition("Idle", "Walk",
		CStateMachine<CMiyabi>::CONDITION_BOOL_TRUE, "IsMove");

	m_pStateMachine->Register_Transition("Walk", "Idle",
		CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "IsMove");

	// Jump 
	m_pStateMachine->Register_Transition("Idle", "Jump",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Jump");
	m_pStateMachine->Register_Transition("Walk", "Jump",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Jump");
	m_pStateMachine->Register_Transition("Jump", "Idle",
		CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

	return S_OK;
}

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
