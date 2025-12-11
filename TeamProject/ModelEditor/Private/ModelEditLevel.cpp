#include "pch.h"
#include "ModelEditLevel.h"

#include "GameInstance.h"
#include "IInputService.h"
#include "ILevelService.h"

#include "EditCamera.h"
#include "EditModel.h"
#include "Camera.h"

CModelEditLevel::CModelEditLevel(const string& LevelKey)
	: CLevel{ LevelKey },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CModelEditLevel::Initialize()
{
	return S_OK;
}

HRESULT CModelEditLevel::Awake()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	pProto->Add_ProtoType("ModelEdit_Level", "Proto_GameObject_EditCamera", CEditCamera::Create());
	pProto->Add_ProtoType("ModelEdit_Level", "Proto_GameObject_EditModel", CEditModel::Create());

	//		IResourceService* pService = CGameInstance::GetInstance()->Get_ResourceMgr();
	//		pService->Add_ResourcePath("TileCell.png", "../../Resources/TileCell.png");
	//		pService->Add_ResourcePath("VTX_PlaneGrid.hlsl", "../Bin/ShaderFiles/VTX_PlaneGrid.hlsl");

	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	CAMERA_DESC desc = {};

	CGameObject* Camera = Builder::Create_Object({ "ModelEdit_Level" ,"Proto_GameObject_EditCamera"})
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0,3,-3 })
		.Build("Main_Camera");

	CGameObject* DemoModel = Builder::Create_Object({ "ModelEdit_Level" ,"Proto_GameObject_EditModel"})
		.Position({ 0,0,0 })
		.Build("Edit_Model");

	pObjMgr->Add_Object(Camera, { "ModelEdit_Level","Camera_Layer"});
	pObjMgr->Add_Object(DemoModel, { "ModelEdit_Level","Model_Layer"});

	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());

	return S_OK;
}

void CModelEditLevel::Update()
{
}

HRESULT CModelEditLevel::Render()
{
	SetWindowText(g_hWnd, TEXT("데모 레벨입니다."));
	return S_OK;
}

void CModelEditLevel::PreLoad_Level()
{
}

CModelEditLevel* CModelEditLevel::Create(const string& LevelKey)
{
	CModelEditLevel* instance = new CModelEditLevel(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Demo level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CModelEditLevel::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
