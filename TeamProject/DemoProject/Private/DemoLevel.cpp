#include "pch.h"
#include "DemoLevel.h"

#include "GameInstance.h"
#include "IInputService.h"
#include "ILevelService.h"

#include "DemoCamera.h"
#include "DemoModel.h"
#include "DemoShaderModel.h"
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
	pProto->Add_ProtoType("Demo_Level", "Proto_GameObject_DemoModel", CDemoShaderModel::Create());
	pProto->Add_ProtoType("Demo_Level", "Proto_GameObject_DemoModel", CDemoModel::Create());
	pProto->Add_ProtoType("Demo_Level", "Proto_GameObject_DemoUI", CDemoUI::Create());

	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	IUI_Service* pUIMgr = m_pGameInstance->Get_UIMgr();

	CGameObject* Camera = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoCamera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0,3,-3 })
		.Build("Main_Camera");

	CUI_Object* UI = Builder::Create_UIObject({ "Demo_Level" ,"Proto_GameObject_DemoUI" })
		.Scale({500,500})
		.Offset({0,0,})
		.Build("Demo_UI");

	pUIMgr->Add_UIObject(UI, "Demo_Level");

	pObjMgr->Add_Object(Camera, { "Demo_Level","Camera_Layer" });
	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());

	CGameObject* Camera2 = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoCamera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0,3,-3 })
		.Build("Main_Camera");

	pObjMgr->Add_Object(Camera2, { "Demo_Level","Camera_Layer" });

	 pObjMgr = m_pGameInstance->Get_ObjectMgr();
	 pUIMgr = m_pGameInstance->Get_UIMgr();

	CGameObject* Model = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoModel" })
		.Position({ 0, 0, 0 })
		.Scale({ 1.f, 1.f, 1.f })
		.Build("Demo_Model");

	pObjMgr->Add_Object(Model, { "Demo_Level","Model_Layer" });

	return S_OK;
}

void CDemoLevel::Update()
{
}

HRESULT CDemoLevel::Render()
{
	SetWindowText(g_hWnd, TEXT("���� �����Դϴ�."));
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
