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

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

void CTestObject::Awake()
{
	auto animator = Get_Component<CAnimator3D>();
    animator->LinkAnimate_Model("Test_Level", "Bangboo_Sharkboo_NPC (merge).model");
    animator->Link_MetaData("Test_Level", "Bangboo_Sharkboo_Meta.json");
    animator->Change_Animation(3);

	auto controller = Get_Component<CCharacterController>();
	controller->Set_GravityEnabled(true);
	controller->Set_Position({0.f, 0.5f, 0.f});
}

void CTestObject::Priority_Update(_float dt)
{
}

void CTestObject::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);

	auto cam        = CAM->Get_ActiveCam();
	auto camTf      = cam->Get_Owner()->Get_Component<CTransform>();
	auto controller = Get_Component<CCharacterController>();

	_vector3 camLook = camTf->Dir(STATE::LOOK);
	_vector3 camRight = camTf->Dir(STATE::RIGHT);

	camLook.y = 0.f; camRight.y = 0.f;

	_vector3 vMoveDir{};

	auto input = CGameInstance::GetInstance()->Get_InputDev();

	if (input->Key_Down(VK_UP))
		vMoveDir += camLook;
	if (input->Key_Down(VK_DOWN))
		vMoveDir -= camLook;
	if (input->Key_Down(VK_RIGHT))
		vMoveDir += camRight;
	if (input->Key_Down(VK_LEFT))
		vMoveDir -= camRight;

	controller->Move_Direction(vMoveDir, 5.0f);

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('J'))
		controller->Jump(10.f);

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Hold('F'))
	{
		_vector vLook = m_pTransform->Dir(STATE::LOOK);
		controller->Shoot_Ray(vLook, 100.f);
	}
	else
		controller->Clear_DebugRay();

	controller->Update(dt);
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

	if (ImGui::Button("Add")) {
		CGameObject* DemoModel = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoModel" })
			.Position({ 0,0,0 })
			.Build("Demo_Model");
		Get_Component<CObjectContainer>()->Add_Child(DemoModel,false);
	}
	_bool isLayer = Get_Layer();
	ImGui::Checkbox("InLayer",&isLayer);
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
	__super::Free();
}