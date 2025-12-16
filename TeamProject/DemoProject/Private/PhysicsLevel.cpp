#include "pch.h"
#include "PhysicsLevel.h"

#include "GameInstance.h"
#include "IInputService.h"
#include "ILevelService.h"

#include "DemoCamera.h"
#include "DemoModel.h"
#include "DemoFloor.h"
#include "DemoPlayer.h"
#include "DemoUI.h"
#include "Camera.h"
#include "InstanceDemo.h"


#include "RigidBody.h"
#include "Collider.h"

CPhysicsLevel::CPhysicsLevel(const string& LevelKey)
	: CLevel{ LevelKey },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPhysicsLevel::Initialize()
{
	return S_OK;
}

HRESULT CPhysicsLevel::Awake()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	pProto->Add_ProtoType("Physics_Level", "Proto_GameObject_DemoCamera", CDemoCamera::Create());
	pProto->Add_ProtoType("Physics_Level", "Proto_GameObject_DemoModel", CDemoModel::Create());
	pProto->Add_ProtoType("Physics_Level", "Proto_GameObject_DemoPlayer", CDemoPlayer::Create());
	pProto->Add_ProtoType("Physics_Level", "Proto_GameObject_DemoFloor", CDemoFloor::Create());

	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	IUI_Service* pUIMgr = m_pGameInstance->Get_UIMgr();

	CGameObject* Camera = Builder::Create_Object({ "Physics_Level" ,"Proto_GameObject_DemoCamera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0,3,-3 })
		.Build("Main_Camera");
	pObjMgr->Add_Object(Camera, { "Physics_Level","Camera_Layer" });


	// Player
	CCT_DESC cctDesc = {};
	CGameObject* Player = Builder::Create_Object({ "Physics_Level", "Proto_GameObject_DemoPlayer" })
		.Position({ 5.f, 10.f, 0.f })
		.Scale({ 1.f, 1.f, 1.f })
		.CharacterController(cctDesc)
		.Build("Demo_Player");
	pObjMgr->Add_Object(Player, { "Physics_Level","Player_Layer" });

	// Floor
	COLLIDER_DESC floorColDesc = {};
	floorColDesc.eType = COLLIDER_TYPE::BOX;
	floorColDesc.vSize = { 20.f, 1.f, 20.f };
	floorColDesc.vCenter = { 0.f, 0.f, 0.f };
	floorColDesc.isTrigger = false;
	floorColDesc.vRotation = { 0.f, 0.f, 0.f };

	CGameObject* Floor = Builder::Create_Object({ "Physics_Level" ,"Proto_GameObject_DemoFloor" })
		.Position({ 0.f, -2.f, 0.f })
		.Scale({ 20.f, 1.f, 20.f })
		.Collider(floorColDesc)
		.Build("Demo_Floor");

	pObjMgr->Add_Object(Floor, { "Physics_Level", "Model_Layer" });
	// Floor end

	// Box
	RIGIDBODY_DESC objRbDesc = {};
	objRbDesc.isKinematic = false;
	objRbDesc.fMass = 10.0f;

	COLLIDER_DESC objColDesc = {};
	objColDesc.eType = COLLIDER_TYPE::BOX;
	objColDesc.vSize = { 1.f, 1.f, 1.f };
	objColDesc.vCenter = { 0.f, 0.f, 0.f };
	objColDesc.isTrigger = false;
	objColDesc.vRotation = { 0.f, 0.f, 0.f };

	CGameObject* FallingObj = Builder::Create_Object({ "Physics_Level" ,"Proto_GameObject_DemoModel" })
		.Position({ 0.f, 10.f, 0.f })
		.Scale({ 1.f, 1.f, 1.f })
		.RigidBody(objRbDesc)
		.Collider(objColDesc)
		.Build("Demo_FallingCube");

	pObjMgr->Add_Object(FallingObj, { "Physics_Level", "Model_Layer" });
	// Box end

	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());

	return S_OK;
}

void CPhysicsLevel::Update()
{
}

HRESULT CPhysicsLevel::Render()
{
	SetWindowText(g_hWnd, TEXT("데모 레벨입니다."));
	return S_OK;
}

void CPhysicsLevel::PreLoad_Level()
{
}

CPhysicsLevel* CPhysicsLevel::Create(const string& LevelKey)
{
	CPhysicsLevel* instance = new CPhysicsLevel(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Physics  level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CPhysicsLevel::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
