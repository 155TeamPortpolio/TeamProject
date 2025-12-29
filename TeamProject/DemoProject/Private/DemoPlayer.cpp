#include "pch.h"
#include "DemoPlayer.h"
#include "GameInstance.h"
#include "ObjectContainer.h"
#include "CharacterController.h"
#include "StaticModel.h"
#include "RigidBody.h"
#include "SkeletalModel.h"
#include "Animator3D.h"
#include "Material.h"

CDemoPlayer::CDemoPlayer()
{
}

CDemoPlayer::CDemoPlayer(const CDemoPlayer& rhs)
	:CGameObject(rhs)
{
}

HRESULT CDemoPlayer::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CObjectContainer>();
	Add_Component<CCharacterController>();
	//Add_Component<CStaticModel>();

	return S_OK;
}

HRESULT CDemoPlayer::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

void CDemoPlayer::Awake()
{
	Add_Component<CSkeletalModel>();
	Add_Component<CAnimator3D>();
	Add_Component<CMaterial>();

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	/*파일명과 키값은 일치*/
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).model",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).model");
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).mat",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).mat");
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_Meta.json",
		"../../DemoResource/new/Anim/Bangboo_Sharkboo_Meta.json");
	Get_Component<CModel>()->Set_RenderType(RENDER_PASS_TYPE::RENDER_3DUI);

	Get_Component<CModel>()->Link_Model("Demo_Level", "Bangboo_Sharkboo_NPC (merge).model");
	Get_Component<CMaterial>()->Link_Material("Demo_Level", "Bangboo_Sharkboo_NPC (merge).mat");
	Get_Component<CAnimator3D>()->LinkAnimate_Model("Demo_Level", "Bangboo_Sharkboo_NPC (merge).model");
	Get_Component<CAnimator3D>()->Link_MetaData("Demo_Level", "Bangboo_Sharkboo_Meta.json");
	Get_Component<CAnimator3D>()->Set_MotionBone(7);
	Get_Component<CAnimator3D>()->Set_RemoveAxisFromMotionBone(AXIS::X |AXIS::Y |AXIS::Z);
	Get_Component<CAnimator3D>()->Set_Animation(0, 16)
		.Loop(true)
		.Apply();

	Get_Component<CCharacterController>()->Set_Position({ 0.f, 5.f, 2.5f });

	Get_Component<CTransform>()->Rotation(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(180));
}

void CDemoPlayer::Priority_Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CDemoPlayer::Update(_float dt)
{
	CCharacterController* pCCT = Get_Component<CCharacterController>();
	_vector3 vMoveDir = XMVectorZero();

	static _bool b = false;
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_UP)) {
		if (b == false) {
			Get_Component<CAnimator3D>()->Change_Animation(0, 6)
				.Speed(0.2)
				.Apply();
			b = true;
		}
		_float3 Root = Get_Component<CAnimator3D>()->Get_RootBoneMoveDelta();
		pCCT->Move_RootMotion(XMLoadFloat3(&Root), XMVectorSet(0, 1, 0, 0), dt);
		//vMoveDir += ;
	}
	else
		b = false;
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_DOWN)) {
		Get_Component<CAnimator3D>()->Change_Animation(0, 6).Apply();
		vMoveDir -= m_pTransform->Dir(STATE::LOOK);
	}
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_RIGHT)) {
		Get_Component<CAnimator3D>()->Change_Animation(0, 6).Apply();
		vMoveDir -= m_pTransform->Dir(STATE::RIGHT);
	}
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_LEFT)) {
		Get_Component<CAnimator3D>()->Change_Animation(0, 6).Apply();
		vMoveDir += m_pTransform->Dir(STATE::RIGHT);
	}

	//pCCT->Move_Direction(vMoveDir, 5.0f, dt);

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('J'))
		pCCT->Jump(10.f);
	

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Hold('F'))
	{
		_vector vLook = m_pTransform->Dir(STATE::LOOK);
		PHYSICS_RAY_HIT hit;
		pCCT->Shoot_Ray(vLook, 100.f, hit);
		if(hit.bHit)
		{
			CRigidBody* pRigid = hit.pHitObject->Get_Component<CRigidBody>();
			if(pRigid) pRigid->Add_Force(vLook * 100);
		}
	}
	else
	{
		pCCT->Clear_DebugRay();
	}
	
	pCCT->Update(dt);
}

void CDemoPlayer::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CDemoPlayer::OnCollisionEnter()
{
	OutputDebugStringA("CCT Collision Enter!\n");
}

void CDemoPlayer::OnCollisionStay()
{
	OutputDebugStringA("CCT Collision Stay!\n");
}

void CDemoPlayer::OnCollisionExit()
{
	OutputDebugStringA("CCT Collision Exit!\n");
}

void CDemoPlayer::Render_GUI()
{
	__super::Render_GUI();

	if (ImGui::Button("Add")) {
		CGameObject* DemoModel = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoPlayer" })
			.Position({ 0,0,0 })
			.Build("Demo_Model");
		Get_Component<CObjectContainer>()->Add_Child(DemoModel, false);
	}
	_bool isLayer = Get_Layer();
	ImGui::Checkbox("InLayer", &isLayer);
}

CDemoPlayer* CDemoPlayer::Create()
{
	CDemoPlayer* instance = new CDemoPlayer();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoPlayer");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDemoPlayer::Clone(INIT_DESC* pArg)
{
	CDemoPlayer* instance = new CDemoPlayer(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoPlayer");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoPlayer::Free()
{
	__super::Free();
}
