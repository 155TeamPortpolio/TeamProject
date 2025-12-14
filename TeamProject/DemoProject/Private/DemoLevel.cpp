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
