#include "pch.h"
#include "DemoLevel.h"

#include "GameInstance.h"
#include "IInputService.h"
#include "ILevelService.h"

#include "DemoCamera.h"
#include "DemoModel.h"
#include "DemoShaderModel.h"
#include "DemoShaderAnimModel.h"
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
	auto* pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).model",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).model");
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).mat",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).mat");
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_Meta.json",
		"../../DemoResource/new/Anim/Bangboo_Sharkboo_Meta.json");

	PreloadKey key{};
	key.type = ResourceType::Texture;
	key.levelKey = "Demo_Level";  
	key.resourceKey = "Bangboo_Sharkboo_NPC (merge).model";   
	key.options.isSRGB = true;
	pRcsMgr->RequestPreload(key);

	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	pProto->Add_ProtoType("Demo_Level", "Proto_GameObject_DemoCamera", CDemoCamera::Create());
	//pProto->Add_ProtoType("Demo_Level", "Proto_GameObject_DemoModel", CDemoShaderModel::Create());
	//pProto->Add_ProtoType("Demo_Level", "Proto_GameObject_DemoAnimModel", CDemoShaderAnimModel::Create());
	//pProto->Add_ProtoType("Demo_Level", "Proto_GameObject_DemoUI", CDemoUI::Create());
	//
	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	//IUI_Service* pUIMgr = m_pGameInstance->Get_UIMgr();
	//
	CGameObject* Camera = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoCamera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0,3,-3 })
		.Build("Main_Camera");
	//
	//CUI_Object* UI = Builder::Create_UIObject({ "Demo_Level" ,"Proto_GameObject_DemoUI" })
	//	.Scale({500,500})
	//	.Offset({0,0,})
	//	.Build("Demo_UI");
	//
	//pUIMgr->Add_UIObject(UI, "Demo_Level");
	//

	pObjMgr->Add_Object(Camera, { "Demo_Level","Camera_Layer" });
	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());
	m_pGameInstance->Get_CameraMgr()->Set_ShadowCam(Camera->Get_Component<CCamera>());
	
	CGameObject* Camera2 = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoCamera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0,3,-3 })
		.Build("Main_Camera");
	//
	//pObjMgr->Add_Object(Camera2, { "Demo_Level","Camera_Layer" });
	//
	// pObjMgr = m_pGameInstance->Get_ObjectMgr();
	// pUIMgr = m_pGameInstance->Get_UIMgr();

	return S_OK;
}

void CDemoLevel::Update()
{
	auto* rm = CGameInstance::GetInstance()->Get_ResourceMgr();
	vector<PreloadCompleted> completed;
	rm->PumpPreloads(completed);

	_uint done = 0, total = 0;
	rm->GetPreloadProgress(done, total);
}

HRESULT CDemoLevel::Render()
{
	SetWindowText(g_hWnd, TEXT(""));
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
