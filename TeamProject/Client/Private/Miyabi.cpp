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
#include "MiyabiState_NormalAttack.h"
#include "MiyabiState_ChargeAttack.h"

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
	//pRcsMgr->Add_ResourcePath("Avatar_Female_Size02_Unagi_Meta.json",
	//	"../Bin/Resources/Model/skeletal/Miyabi/Anim/Avatar_Female_Size02_Unagi_Meta.json");
	pRcsMgr->Add_ResourcePath("Avatar_Female_Size02_Unagi_Meta.json",
		"../Bin/Resources/Model/skeletal/Miyabi/Avatar_Female_Size02_Unagi_Meta.json");

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
	m_pAnimator->LinkAnimate_Model("Test_Level", "Avatar_Female_Size02_Unagi.model");
	m_pAnimator->Link_MetaData("Test_Level", "Avatar_Female_Size02_Unagi_Meta.json");
	//m_pAnimator()->Set_ExtractBoneMovement(21);
	m_pAnimator ->Set_MotionBone(21);
	m_pAnimator->Set_ExtractMotionboneMovement(AXIS::X | AXIS::Z);
	//m_pAnimator->Set_ExtractMotionboneRotation(AXIS::Y);
	m_pAnimator->Set_Animation("Avatar_Female_Size02_Unagi_Ani_Idle")
		.Loop(true)
		.Apply();
	m_pCCT->Set_GravityEnabled(true);
}

void CMiyabi::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CMiyabi::Update(_float dt)
{
	Update_States();
	m_pStateMachine->Update(dt);
	__super::Update(dt);
}

void CMiyabi::Late_Update(_float dt)
{
	__super::Late_Update(dt);
	//Add_OutLineRender();
}

void CMiyabi::Render_GUI()
{
	__super::Render_GUI();
	// StateMachine ����� ����
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

	//Get_Component<CMaterial>()->Set_OutLineInfo(_float4(0.27f, 0.27f, 0.27f, 1.0f), 0.001f);

	pContext->IASetInputLayout(pLayout);
	Shader->Apply("OutLine", pContext);
	Model->Draw(pContext, idx);
}

void CMiyabi::Update_States()
{
	_bool bInMoveEnd = false;
	_bool bInAttackEnd = false;

	// Move End üũ (������ ����)
	if (m_pStateMachine->Get_CurrentStateName() == "Move")
	{
		CMiyabiState_Move* pMove =
			static_cast<CMiyabiState_Move*>(m_pStateMachine->Get_CurrentState());

		if (pMove && pMove->Get_SubStateMachine())
		{
			IHState<CMiyabi>* pMoveType =
				dynamic_cast<IHState<CMiyabi>*>(pMove->Get_SubStateMachine()->Get_CurrentState());

			if (pMoveType && pMoveType->Has_SubStateMachine())
			{
				IBaseState<CMiyabi>* pAnim =
					pMoveType->Get_SubStateMachine()->Get_CurrentState();

				bInMoveEnd = (pAnim && pAnim->Get_Tag() == "End");
			}
		}
	}
	// Attack End üũ
	else if (m_pStateMachine->Get_CurrentStateName() == "Attack")
	{
		CMiyabiState_Attack* pAttack =
			static_cast<CMiyabiState_Attack*>(m_pStateMachine->Get_CurrentState());

		if (pAttack && pAttack->Get_SubStateMachine())
		{
			string strSub = pAttack->Get_SubStateMachine()->Get_CurrentStateName();

			if (strSub == "NormalAttack")
			{
				CMiyabiState_NormalAttack* pNormal =
					static_cast<CMiyabiState_NormalAttack*>(
						pAttack->Get_SubStateMachine()->Get_State("NormalAttack"));

				if (pNormal && pNormal->Get_SubStateMachine())
				{
					IBaseState<CMiyabi>* pNormalSub = pNormal->Get_SubStateMachine()->Get_CurrentState();
					bInAttackEnd = (pNormalSub && pNormalSub->Get_Tag() == "End");
				}
			}
			else if (strSub == "ChargeAttack")
			{
				CMiyabiState_ChargeAttack* pCharge =
					static_cast<CMiyabiState_ChargeAttack*>(
						pAttack->Get_SubStateMachine()->Get_State("ChargeAttack"));

				if (pCharge && pCharge->Get_SubStateMachine())
				{
					IBaseState<CMiyabi>* pChargeSub = pCharge->Get_SubStateMachine()->Get_CurrentState();
					bInAttackEnd = (pChargeSub && pChargeSub->Get_Tag() == "End");
				}
			}
		}
	}

	// �� AttackEnd �Ķ���� ����
	if (bInAttackEnd)
	{
		CMiyabiState_Attack* pAttack =
			static_cast<CMiyabiState_Attack*>(m_pStateMachine->Get_CurrentState());

		if (pAttack)
		{
			// Attack�� AnimProgress�� 1.0�̸� AttackEnd = true
			_bool bAttackFinished = (pAttack->Get_AnimProgress() >= 1.f);
			m_pStateMachine->Set_Bool("AttackEnd", bAttackFinished);
		}
	}
	else
	{
		m_pStateMachine->Set_Bool("AttackEnd", false);
	}

	// End ĵ�� ó�� (������ ����)
	if ((bInMoveEnd || bInAttackEnd) && Is_Input())
	{
		m_pStateMachine->Set_Bool("IsMove", false);

		// Attack End���� �Է� �� ������ AttackEnd = true
		if (bInAttackEnd)
			m_pStateMachine->Set_Bool("AttackEnd", true);
	}
	else
	{
		m_pStateMachine->Set_Bool("IsMove", Is_Move());

		if (m_bIsAttack)
		{
			string strCurrent = m_pStateMachine->Get_CurrentStateName();

			if (strCurrent == "Idle")
			{
				m_pStateMachine->Set_Trigger("Attack");
			}
			else if (strCurrent == "Attack")
			{
				CMiyabiState_Attack* pAttackState =
					static_cast<CMiyabiState_Attack*>(m_pStateMachine->Get_CurrentState());
				if (pAttackState && pAttackState->Get_SubStateMachine())
				{
					if (pAttackState->Get_SubStateMachine()->Get_CurrentStateName() == "NormalAttack")
					{
						CMiyabiState_NormalAttack* pNormal =
							static_cast<CMiyabiState_NormalAttack*>(
								pAttackState->Get_SubStateMachine()->Get_State("NormalAttack"));

						if (pNormal && pNormal->Get_SubStateMachine())
							pNormal->Get_SubStateMachine()->Set_Trigger("NextCombo");
					}
				}
			}
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
	// Idle <-> Move
	m_pStateMachine->Register_Transition("Idle", "Move",
		CStateMachine<CMiyabi>::CONDITION_BOOL_TRUE, "IsMove");

	m_pStateMachine->Register_Transition("Move", "Idle",
		CStateMachine<CMiyabi>::CONDITION_BOOL_FALSE, "IsMove");

	// Idle -> Attack
	m_pStateMachine->Register_AnyStateTransition("Attack",
		CStateMachine<CMiyabi>::CONDITION_TRIGGER, "Attack");

	// Attack -> Idle
	m_pStateMachine->Register_Transition("Attack", "Idle",
		CStateMachine<CMiyabi>::CONDITION_BOOL_TRUE, "AttackEnd");

	return S_OK;
}

HRESULT CMiyabi::Add_OutLineRender()
{
	auto Model = Get_Component<CSkeletalModel>();
	//_uint size = sizeof(_float4x4) * m_pAnimator->Get_CombinedBoneMatrices().size();
	_uint size = sizeof(_float4x4) * m_pAnimator->Get_BoneMatrices(CAnimator3D::BoneSpace::COMBINED).size();

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
