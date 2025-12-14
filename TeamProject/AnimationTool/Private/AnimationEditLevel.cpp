#include "pch.h"
#include "AnimationEditLevel.h"

#include "GameInstance.h"
#include "IInputService.h"
#include "ILevelService.h"

#include "EditCamera.h"
#include "EditModel.h"
#include "AnimToolPanel.h"
#include "Camera.h"

CAnimationEditLevel::CAnimationEditLevel(const string& LevelKey)
	: CLevel{ LevelKey },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAnimationEditLevel::Initialize()
{
	return S_OK;
}

HRESULT CAnimationEditLevel::Awake()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	pProto->Add_ProtoType("AnimationEdit_Level", "Proto_GameObject_EditCamera", CEditCamera::Create());
	pProto->Add_ProtoType("AnimationEdit_Level", "Proto_GameObject_EditModel", CEditModel::Create());

	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	CAMERA_DESC desc = {};

	CGameObject* Camera = Builder::Create_Object({ "AnimationEdit_Level" ,"Proto_GameObject_EditCamera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0,3,-3 })
		.Build("Main_Camera");

	CGameObject* AnimModel = Builder::Create_Object({ "AnimationEdit_Level" ,"Proto_GameObject_EditModel" })
		.Position({ 0,0,0 })
		.Build("Anim_Model");

	
	pObjMgr->Add_Object(AnimModel, { "AnimationEdit_Level","Model_Layer" });
	pObjMgr->Add_Object(Camera, { "AnimationEdit_Level","Camera_Layer" });

	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());

	IGUIService* guiSvc = CGameInstance::GetInstance()->Get_GUISystem();
	guiSvc->Register_Panel(CAnimToolPanel::Create(guiSvc->Get_Context()));

	return S_OK;
}

void CAnimationEditLevel::Update()
{
}

HRESULT CAnimationEditLevel::Render()
{
	SetWindowText(g_hWnd, TEXT("데모 레벨입니다."));
	return S_OK;
}

void CAnimationEditLevel::PreLoad_Level()
{
}

CAnimationEditLevel* CAnimationEditLevel::Create(const string& LevelKey)
{
	CAnimationEditLevel* instance = new CAnimationEditLevel(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Demo level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CAnimationEditLevel::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
