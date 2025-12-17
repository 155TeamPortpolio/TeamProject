#include "pch.h"
#include "TestLevel.h"
#include "GameInstance.h"

#include "FreeCamera.h"
#include "TestObject.h"

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

	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_FreeCamera", CFreeCamera::Create());
	pProto->Add_ProtoType("Test_Level", "Proto_GameObject_TestModel", CTestObject::Create());


	Ready_Camera();
	return S_OK;
}

void CTestLevel::Update()
{
}

void CTestLevel::Ready_Camera()
{
	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	CGameObject* Camera =
		Builder::Create_Object({ "Test_Level" ,"Proto_GameObject_FreeCamera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0,3,-3 })
		.Build("Main_Camera");

	pObjMgr->Add_Object(Camera, { "Test_Level","Camera_Layer" });
	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());
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
}
