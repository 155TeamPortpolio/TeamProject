#include "pch.h"
#include "DemoLevel.h"

#include "GameInstance.h"
#include "IInputService.h"
#include "ILevelService.h"

#include "DemoCamera.h"
#include "DemoModel.h"
#include "DemoUI.h"
#include "Camera.h"
#include "InstanceDemo.h"

#include "RigidBody.h"
#include "Collider.h"

CDemoLevel::CDemoLevel(const string& LevelKey)
	: CLevel{ LevelKey },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CDemoLevel::Initialize()
{
	return S_OK;
}

HRESULT CDemoLevel::Awake()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	pProto->Add_ProtoType("Demo_Level", "Proto_GameObject_DemoCamera", CDemoCamera::Create());
	pProto->Add_ProtoType("Demo_Level", "Proto_GameObject_DemoModel", CDemoModel::Create());

	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	IUI_Service* pUIMgr = m_pGameInstance->Get_UIMgr();

	CGameObject* Camera = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoCamera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0,3,-3 })
		.Build("Main_Camera");

	pObjMgr->Add_Object(Camera, { "Demo_Level","Camera_Layer" });

	// Floor
	RIGIDBODY_DESC floorRbDesc = {};
	floorRbDesc.isStatic = true;
	floorRbDesc.isKinematic = false;
	floorRbDesc.fMass = 0.f;

	COLLIDER_DESC floorColDesc = {};
	floorColDesc.eType = COLLIDER_TYPE::BOX;
	floorColDesc.vSize = { 20.f, 1.f, 20.f };
	floorColDesc.vCenter = { 0.f, 0.f, 0.f };
	floorColDesc.isTrigger = false;
	floorColDesc.vRotation = { 0.f, 0.f, 0.f };

	CGameObject* Floor = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoModel" })
		.Position({ 0.f, -2.f, 0.f })
		.Scale({ 20.f, 1.f, 20.f })
		.RigidBody(floorRbDesc)
		.Collider(floorColDesc)
		.Build("Demo_Floor");

	pObjMgr->Add_Object(Floor, { "Demo_Level", "Model_Layer" });
	// Floor end

	// Box
	RIGIDBODY_DESC objRbDesc = {};
	objRbDesc.isStatic = false;
	objRbDesc.isKinematic = false;
	objRbDesc.fMass = 10.0f;

	COLLIDER_DESC objColDesc = {};
	objColDesc.eType = COLLIDER_TYPE::BOX;
	objColDesc.vSize = { 1.f, 1.f, 1.f };
	objColDesc.vCenter = { 0.f, 0.f, 0.f };
	objColDesc.isTrigger = false;
	objColDesc.vRotation = { 0.f, 0.f, 0.f };

	CGameObject* FallingObj = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoModel" })
		.Position({ 0.f, 10.f, 0.f })
		.Scale({ 1.f, 1.f, 1.f })
		.RigidBody(objRbDesc)
		.Collider(objColDesc)
		.Build("Demo_FallingCube");

	pObjMgr->Add_Object(FallingObj, { "Demo_Level", "Model_Layer" });
	// Box end

	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());

	return S_OK;
}

void CDemoLevel::Update()
{
}

HRESULT CDemoLevel::Render()
{
	SetWindowText(g_hWnd, TEXT("데모 레벨입니다."));
	return S_OK;
}

void CDemoLevel::PreLoad_Level()
{
}

CDemoLevel* CDemoLevel::Create(const string& LevelKey)
{
	CDemoLevel* instance = new CDemoLevel(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Demo level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoLevel::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
