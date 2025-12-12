#include "pch.h"
#include "MapToolLevel.h"

#include "GameInstance.h"
#include "IInputService.h"
#include "ILevelService.h"

#include "DefaultCamera.h"
#include "DummyModel.h"
#include "Camera.h"

CMapToolLevel::CMapToolLevel(const string& LevelKey)
	: CLevel{ LevelKey },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMapToolLevel::Initialize()
{
	return S_OK;
}

HRESULT CMapToolLevel::Awake()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	pProto->Add_ProtoType("MapTool_Level", "Proto_GameObject_DefaultCamera", CDefaultCamera::Create());
	pProto->Add_ProtoType("MapTool_Level", "Proto_GameObject_DummyModel", CDummyModel::Create());

	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	IUI_Service* pUIMgr = m_pGameInstance->Get_UIMgr();
	CAMERA_DESC desc = {};

	CGameObject* Camera = Builder::Create_Object({ "MapTool_Level" ,"Proto_GameObject_DefaultCamera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0,3,-3 })
		.Build("Main_Camera");

	CGameObject* DemoModel = Builder::Create_Object({ "MapTool_Level" ,"Proto_GameObject_DummyModel" })
		.Position({ 0,0,0 })
		.Build("Demo_Model");


	pObjMgr->Add_Object(Camera, { "MapTool_Level","Camera_Layer" });
	pObjMgr->Add_Object(DemoModel, { "MapTool_Level","Model_Layer" });

	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());

	return S_OK;
}

void CMapToolLevel::Update()
{
}

HRESULT CMapToolLevel::Render()
{
	SetWindowText(g_hWnd, TEXT("Level : MapTool"));
	return S_OK;
}

void CMapToolLevel::PreLoad_Level()
{
}

CMapToolLevel* CMapToolLevel::Create(const string& LevelKey)
{
	CMapToolLevel* instance = new CMapToolLevel(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("MapTool level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CMapToolLevel::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
