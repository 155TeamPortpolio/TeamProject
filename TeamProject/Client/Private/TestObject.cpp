#include "pch.h"
#include "TestObject.h"
#include "SkeletalModel.h"
#include "Animator3D.h"
#include "Material.h"
#include "CharacterController.h"
#include "Camera.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "RigidBody.h"

#include "StateMachine.h"
#include "TestState_Idle.h"
#include "TestState_Walk.h"
#include "TestState_Jump.h"
#include "TestState_Dash.h"

#define CAM CGameInstance::GetInstance()->Get_CameraMgr()


CTestObject::CTestObject()
{
}

CTestObject::CTestObject(const CTestObject& rhs)
	:CGameObject(rhs) 
{
}

HRESULT CTestObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CAnimator3D>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();
	//Add_Component<CRigidBody>();
	//Add_Component<CCollider>();
	Add_Component<CCharacterController>();

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();

	/*파일명과 키값은 일치*/
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).model",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).model");
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).mat",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).mat");
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_Meta.json",
		"../../DemoResource/new/Anim/Bangboo_Sharkboo_Meta.json");

	Get_Component<CModel>()->Link_Model("Test_Level", "Bangboo_Sharkboo_NPC (merge).model");
	Get_Component<CMaterial>()->Link_Material("Test_Level", "Bangboo_Sharkboo_NPC (merge).mat");

	return S_OK;
}

HRESULT CTestObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	Initialize_State();

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

HRESULT CTestObject::Initialize_State()
{
	m_pStateMachine = CTestStateMachine::Create();

	// States 등록
	m_pStateMachine->Register_State("Idle", CTestState_Idle::Create());
	m_pStateMachine->Register_State("Walk", CTestState_Walk::Create());
	m_pStateMachine->Register_State("Jump", CTestState_Jump::Create());
	m_pStateMachine->Register_State("Dash", CTestState_Dash::Create());

	// Transition 설정
	// Idle <-> Walk
	m_pStateMachine->Register_Transition("Idle", "Walk",
		CTestStateMachine::CONDITION_BOOL_TRUE, "IsMoving");
	m_pStateMachine->Register_Transition("Walk", "Idle",
		CTestStateMachine::CONDITION_BOOL_FALSE, "IsMoving");

	// AnyState -> Jump
	m_pStateMachine->Register_AnyStateTransition("Jump",
		CTestStateMachine::CONDITION_TRIGGER, "Jump");

	// AnyState -> Dash
	m_pStateMachine->Register_AnyStateTransition("Dash",
		CTestStateMachine::CONDITION_TRIGGER, "Dash");

	// Jump -> Idle (착지하면 무조건 Idle로)
	m_pStateMachine->Register_Transition("Jump", "Idle",
		CTestStateMachine::CONDITION_BOOL_TRUE, "IsGrounded");

	// Dash -> Idle (대쉬 완료되면 무조건 Idle로)
	m_pStateMachine->Register_Transition("Dash", "Idle",
		CTestStateMachine::CONDITION_BOOL_TRUE, "DashFinished");


	// 기본 상태 설정
	m_pStateMachine->Set_DefaultState("Idle");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

void CTestObject::Awake()
{
	Get_Component<CAnimator3D>()->LinkAnimate_Model("Test_Level", "Bangboo_Sharkboo_NPC (merge).model");
	Get_Component<CAnimator3D>()->Link_MetaData("Test_Level", "Bangboo_Sharkboo_Meta.json");
	Get_Component<CAnimator3D>()->Set_Animation(0, 3);
	Get_Component<CAnimator3D>()->Set_NoTransform(7); // << SharkBoo는 7번본이움직임

	Get_Component<CCharacterController>()->Set_GravityEnabled(true);
	Get_Component<CCharacterController>()->Set_Position({0.f, 1.f, 0.f});
}

void CTestObject::Priority_Update(_float dt)
{
}

void CTestObject::Update(_float dt)
{
	// Process Input
	auto input = CGameInstance::GetInstance()->Get_InputDev();
	m_vInputDir = _vector3(0.f, 0.f, 0.f);
	if (input->Key_Down(VK_UP)) m_vInputDir.z += 1.f;
	if (input->Key_Down(VK_DOWN)) m_vInputDir.z -= 1.f;
	if (input->Key_Down(VK_RIGHT)) m_vInputDir.x += 1.f;
	if (input->Key_Down(VK_LEFT)) m_vInputDir.x -= 1.f;

	m_bJump = input->Key_Down('J');
	_bool bDash = input->Key_Down(VK_SHIFT);

	Get_Component<CAnimator3D>()->Update_Animation(dt);
	if (m_pStateMachine) m_pStateMachine->Update(dt);

	// Process Parameter
	auto pCCT = Get_Component<CCharacterController>();
	if (pCCT)
	{
		m_pStateMachine->Set_Bool("IsGrounded", pCCT->Is_Grounded());
		m_pStateMachine->Set_Bool("IsMoving", m_vInputDir.Length() > 0.01f);
	}

	if (m_bJump)
	{
		m_pStateMachine->Set_Trigger("Jump");
		m_bJump = false;
	}

	if (bDash)
	{
		m_pStateMachine->Set_Trigger("Dash");
	}

	auto controller = Get_Component<CCharacterController>();
	if (controller)
	{
		controller->Update(dt);

		// 디버그용 레이캐스트 (F키)
		auto input = CGameInstance::GetInstance()->Get_InputDev();
		if (input->Key_Hold('F'))
		{
			PHYSICS_RAY_HIT hit;
			_vector vLook = m_pTransform->Dir(STATE::LOOK);
			controller->Shoot_Ray(vLook, 100.f, hit);
		}
		else
		{
			controller->Clear_DebugRay();
		}
	}
}

void CTestObject::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CTestObject::OnCollisionEnter()
{

}

void CTestObject::OnCollisionStay()
{
}

void CTestObject::OnCollisionExit()
{
}

void CTestObject::Render_GUI()
{
	__super::Render_GUI();

	// StateMachine 디버깅 정보
	if (m_pStateMachine)
	{
		ImGui::Separator();
		ImGui::Text("Current State: %s", m_pStateMachine->Get_CurrentStateName().c_str());
		ImGui::Text("State Time: %.2f", m_pStateMachine->Get_StateTime());
	}

	if (ImGui::Button("Add")) {
		CGameObject* DemoModel = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoModel" })
			.Position({ 0,0,0 })
			.Build("Demo_Model");
		Get_Component<CObjectContainer>()->Add_Child(DemoModel,false);
	}
	_bool isLayer = Get_Layer();
	ImGui::Checkbox("InLayer",&isLayer);


}

void CTestObject::Rotate_Horizontal(const _vector3& vDirection)
{
	_vector vDir = XMLoadFloat3(&vDirection);
	vDir = XMVectorSetY(vDir, 0.f);

	if (XMVector3Length(vDir).m128_f32[0] < 0.001f)
		return;

	vDir = XMVector3Normalize(vDir);

	_vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vDir));

	_matrix vRotmat = XMMatrixIdentity();
	vRotmat.r[0] = vRight;
	vRotmat.r[1] = vWorldUp;
	vRotmat.r[2] = vDir;

	_vector vQuaternion = XMQuaternionRotationMatrix(vRotmat);
	m_pTransform->Set_Quaternion(vQuaternion);
}

CTestObject* CTestObject::Create()
{
	CTestObject* instance = new CTestObject();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CTestObject");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CTestObject::Clone(INIT_DESC* pArg)
{
	CTestObject* instance = new CTestObject(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CTestObject");
		Safe_Release(instance);
	}

	return instance;
}

void CTestObject::Free()
{
	Safe_Release(m_pStateMachine);
	__super::Free();
}
