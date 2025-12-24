#include "pch.h"
#include "Miyabi.h"
#include "GameInstance.h"

#include "StateMachine.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

#include "MiyabiState_Idle.h"
#include "MiyabiState_Walk.h"
#include "MiyabiState_Attack.h"

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

	Get_Component<CMaterial>()->Set_RimLightInfo(_float3(1.f, 1.f, 1.f), 1.f);
}

void CMiyabi::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CMiyabi::Update(_float dt)
{
	Update_Input(dt);

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
	// StateMachine 디버깅 정보
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
	// Attack 입력 처리
	if (m_bIsAttack)
	{
		string strCurrent = m_pStateMachine->Get_CurrentStateName();
		if (strCurrent == "Attack")
		{
			// Attack 상태면 서브 스테이트머신에 트리거 전달
			CMiyabiState_Attack* pAttackState =
				static_cast<CMiyabiState_Attack*>(m_pStateMachine->Get_CurrentState());
			if (pAttackState && pAttackState->Get_SubStateMachine())
				pAttackState->Get_SubStateMachine()->Set_Trigger("Attack");
		}
		else
		{
			// 다른 상태면 Attack 상태로 전이
			m_pStateMachine->Set_Trigger("Attack");
		}
	}

	// 디버그용 레이캐스트 (F키)
	auto input = CGameInstance::GetInstance()->Get_InputDev();
	if (input->Key_Hold('F'))
	{
		PHYSICS_RAY_HIT hit;
		_vector vLook = m_pTransform->Dir(STATE::LOOK);
		m_pCCT->Shoot_Ray(vLook, 100.f, hit);
	}
	else
	{
		m_pCCT->Clear_DebugRay();
	}

	// 테스트용 점프 (J키)
	if (input->Key_Down('J'))
	{
		m_pCCT->Jump(3.f);
	}
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
	m_pStateMachine->Register_State("Attack", CMiyabiState_Attack::Create());

	return S_OK;
}

HRESULT CMiyabi::Initialize_Transitions()
{
	// Idle <-> Walk
	m_pStateMachine->Register_Transition("Idle", "Walk",
		CStateMachine<CMiyabi>::CONDITION_BOOL_TRUE, "IsMove");

	m_pStateMachine->Register_Transition("Walk", "Idle",
		CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "IsMove");

	m_pStateMachine->Register_AnyStateTransition("Attack",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Attack");

	m_pStateMachine->Register_Transition("Attack", "Idle",
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
