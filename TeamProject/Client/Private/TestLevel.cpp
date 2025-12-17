#include "pch.h"
#include "TestLevel.h"
#include "GameInstance.h"

#include "FreeCam.h"
#include "TestObject.h"
#include "TestMap.h"

#include "Camera.h"

CTestLevel::CTestLevel(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CTestLevel::Initialize()
{
	return S_OK;
}

HRESULT CTestLevel::Awake()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();

	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_FreeCam", CFreeCam::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestModel", CTestObject::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestMap", CTestMap::Create());


	Ready_Map();		// 프레임 이슈로 잠시 사용 X
	Ready_Camera();
	Ready_Object();
	return S_OK;
}

void CTestLevel::Update()
{
}

void CTestLevel::Ready_Camera()
{
	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	CGameObject* Camera =
		Builder::Create_Object({ "Test_Level" ,"Proto_GameObject_FreeCam" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0.f, 3.f, -3.f })
		.Build("Free_Cam");

	pObjMgr->Add_Object(Camera, { "Test_Level","Camera_Layer" });
	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());
}

void CTestLevel::Ready_Map()
{
	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();

	COLLIDER_DESC MapColDesc = {};
	MapColDesc.bCooking = true;
	MapColDesc.strModelKey = "Concert_Ground_FloorTile_01.model";

	CGameObject* Map =
		Builder::Create_Object({ "Test_Level" ,"Proto_GameObject_TestMap" })
		.Position({ 0.f, 0.f, 0.f })
		.Collider(MapColDesc)
		.Build("TestMap");

	pObjMgr->Add_Object(Map, { "Test_Level","Map_Layer" });
	
}

void CTestLevel::Ready_Object()
{
	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	
	CCT_DESC ObjCCTDesc = {};
	CGameObject* Obj =
		Builder::Create_Object({ "Test_Level" ,"Proto_GameObject_TestModel" })
		.Position({ 0.f, 10.f, 0.f })
		.CharacterController(ObjCCTDesc)
		.Build("TestObj");

	pObjMgr->Add_Object(Obj, { "Test_Level","Object_Layer" });
}

HRESULT CTestLevel::Render()
{
	SetWindowText(g_hWnd, TEXT("Welcome To TestLevel"));
	return S_OK;
}

void CTestLevel::PreLoad_Level()
{
}

CTestLevel* CTestLevel::Create(const string& LevelKey)
{
	CTestLevel* instance = new CTestLevel(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Test level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CTestLevel::Free()
{
	m_pGameInstance->DestroyInstance();
}
