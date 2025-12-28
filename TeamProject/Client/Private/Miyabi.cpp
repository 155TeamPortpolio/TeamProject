#include "pch.h"
#include "Miyabi.h"
#include "GameInstance.h"


#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

#include "StateMachine.h"
#include "MiyabiState_Idle.h"
#include "MiyabiState_Move.h"
#include "MiyabiState_Attack.h"

#include "Renderer.h"
#include "SkeletalModel.h"
#include "Shader.h"

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
	Get_Component<CAnimator3D>()->Set_ExtractBoneMovement(21);
	Get_Component<CAnimator3D>()->Set_Animation("Avatar_Female_Size02_Unagi_Ani_Idle")
		.Loop(true)
		.Apply();
	Get_Component<CCharacterController>()->Set_GravityEnabled(true);

	Get_Component<CMaterial>()->Set_RimLightInfo(_float3(0.f, 0.f, 0.0), 0.1f);
	CGameInstance::GetInstance()->Get_RenderSystem()->SetRimLightMode(RIMLIGHT::OUTLINE);
}

void CMiyabi::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CMiyabi::Update(_float dt)
{
	Update_Input(dt);
	Update_States();
	m_pStateMachine->Update(dt);
	__super::Update(dt);
}

void CMiyabi::Late_Update(_float dt)
{
	__super::Late_Update(dt);
	Add_OutLineRender();
}

void CMiyabi::Render_GUI()
{
	__super::Render_GUI();
	// StateMachine 디버깅 정보
	if (m_pStateMachine)
	{
		ImGui::Separator();
		ImGui::Text("StateMachine: %s", m_pStateMachine->Get_CurrentStateName().c_str());

		if (ImGui::Button("Open StateMachine"))
			m_pStateMachine->Set_ShowWindow(true);

		m_pStateMachine->Render_GUI();
	}
}

void CMiyabi::Render_OutLine(ID3D11DeviceContext* pContext, _uint idx)
{
	auto RenderSys = CGameInstance::GetInstance()->Get_RenderSystem()->GetRenderer(RENDERER_TYPE::FORWARD);
	auto Model = Get_Component<CSkeletalModel>();
	auto Material = Get_Component<CMaterial>();

	_int Index = Model->Get_MaterialIndex(idx);
	auto Shader = Material->Get_Shader(Index);
	ID3D11InputLayout* pLayout;
	RenderSys->Get_InputLayout(
		Model,
		Shader,
		idx,
		"OutLine",
		&pLayout
	);

	Get_Component<CMaterial>()->Set_OutLineInfo(_float4(0.27f, 0.27f, 0.27f, 1.0f), 0.001f);

	pContext->IASetInputLayout(pLayout);
	Shader->Apply("OutLine", pContext);
	Model->Draw(pContext, idx);
}

void CMiyabi::Update_Input(_float dt)
{
	__super::Update_Input(dt);

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

void CMiyabi::Update_States()
{
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
	m_pStateMachine->Register_State("Move", CMiyabiState_Move::Create());
	m_pStateMachine->Register_State("Attack", CMiyabiState_Attack::Create());

	return S_OK;
}

HRESULT CMiyabi::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CMiyabi>::CONDITION_BOOL_TRUE, "IsMove");

	m_pStateMachine->Register_Transition("Move", "Idle",
		CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "IsMove");

	m_pStateMachine->Register_AnyStateTransition("Attack",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Attack");

	m_pStateMachine->Register_Transition("Attack", "Idle",
		CStateMachine<CMiyabi>::CONDITION_ANIMATION_END);

	m_pStateMachine->Register_Transition("Attack", "Walk",
		CStateMachine<CMiyabi>::CONDITION_BOOL_TRUE, "IsMove");

	return S_OK;
}

HRESULT CMiyabi::Add_OutLineRender()
{
	auto Model = Get_Component<CSkeletalModel>();
	_uint size = sizeof(_float4x4) * m_pAnimator->Get_BoneMatrices().size();

	for (_int i = 0; i < Model->Get_MeshCount(); ++i)
	{
		vector<_float4x4> BoneMatrices = m_pAnimator->Get_BoneMatrices(i);
		OUTLINE_COMMAND Command =
		{
			Get_Component<CMaterial>()->Get_Shader(Model->Get_MaterialIndex(i)),
			m_pTransform->Get_WorldMatrix_Ptr(),
			BoneMatrices,
			"float4x4[]",
			size ,
			i,
			[this](ID3D11DeviceContext* pContext, _uint index) {Render_OutLine(pContext,index); }
		};
		CGameInstance::GetInstance()->Get_RenderSystem()->Add_OutLineCommand(Command);
	}
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
