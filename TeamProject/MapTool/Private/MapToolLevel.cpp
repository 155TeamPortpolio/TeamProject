#include "pch.h"
#include "MapToolLevel.h"

#include "GameInstance.h"
#include "IResourceService.h"
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
	
	IResourceService* pResMgr = CGameInstance::GetInstance()->Get_ResourceMgr();

	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	IUI_Service* pUIMgr = m_pGameInstance->Get_UIMgr();
	CAMERA_DESC desc = {};

	if (FAILED(CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath("Test.dds", "../Bin/Test.dds")))
		return E_FAIL;

	LIGHT_INIT_DESC DefaultCameraLightDesc = {};
	DefaultCameraLightDesc.eType = LIGHT_TYPE::POINT;
	DefaultCameraLightDesc.fRange = 100.f;
	DefaultCameraLightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	
	CGameObject* Camera = Builder::Create_Object({ "MapTool_Level" ,"Proto_GameObject_DefaultCamera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Light(DefaultCameraLightDesc)
		.Position({ 0,3,-3 })
		.Build("Main_Camera");

	CGameObject* DemoModel = Builder::Create_Object({ "MapTool_Level" ,"Proto_GameObject_DummyModel" })
		.Position({ 0,0,0 })
		.Build("Dummy_Model");
		
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
