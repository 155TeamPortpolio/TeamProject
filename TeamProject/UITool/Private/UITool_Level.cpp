#include "pch.h"
#include "UITool_Level.h"

#include "GameInstance.h"
#include "IInputService.h"
#include "ILevelService.h"

#include "UITool_Camera.h"
#include "Camera.h"

#include "GUIPanel.h"
#include "CanvasPanel.h"

CUITool_Level::CUITool_Level(const string& LevelKey)
	: CLevel{ LevelKey },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUITool_Level::Initialize()
{
	return S_OK;
}

HRESULT CUITool_Level::Awake()
{
	if (FAILED(Ready_Textures()))
		MSG_BOX("Failed to Ready Textures : UI Tool");

	if(FAILED(Ready_Camera()))
		MSG_BOX("Failed to Ready Camera : UI Tool");

	if (FAILED(Ready_UIObjects()))
		MSG_BOX("Failed to Ready UI Objects : UI Tool");

	if (FAILED(Ready_GUIPanel()))
		MSG_BOX("Failed to Ready GUI Panel : UI Tool");

	return S_OK;
}

void CUITool_Level::Update()
{
}

HRESULT CUITool_Level::Render()
{
	SetWindowText(g_hWnd, TEXT("데모 레벨입니다."));

	return S_OK;
}

void CUITool_Level::PreLoad_Level()
{
}

HRESULT CUITool_Level::Ready_Textures()
{
	if (FAILED(CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath("Logo.png", "../Bin/Resources/UI/Logo.png")))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath("Bangboo.jpg", "../Bin/Resources/UI/Bangboo.jpg")))
		return E_FAIL;

	return S_OK;
}

HRESULT CUITool_Level::Ready_Camera()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();

	if (FAILED(pProto->Add_ProtoType("UITool_Level", "Proto_GameObject_Camera", CUITool_Camera::Create())))
		return E_FAIL;

	CGameObject* Camera = Builder::Create_Object({ "UITool_Level" ,"Proto_GameObject_Camera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0.f, 3.f, -3.f })
		.Build("Main_Camera");

	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();

	pObjMgr->Add_Object(Camera, { "UITool_Level","Camera_Layer" });

	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());

	return S_OK;
}

HRESULT CUITool_Level::Ready_UIObjects()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();

	if (FAILED(pProto->Add_ProtoType("UITool_Level", "Proto_GameObject_CanvasPanel", CCanvasPanel::Create())))
		return E_FAIL;

	return S_OK;
}

HRESULT CUITool_Level::Ready_GUIPanel()
{
	CBasePanel* pPanel = CGUIPanel::Create(CGameInstance::GetInstance()->Get_GUISystem()->Get_Context());
	if (!pPanel)
		return E_FAIL;

	CGameInstance::GetInstance()->Get_GUISystem()->Register_Panel(pPanel);

	return S_OK;
}

CUITool_Level* CUITool_Level::Create(const string& LevelKey)
{
	CUITool_Level* instance = new CUITool_Level(LevelKey);

	if (FAILED(instance->Initialize())) 
	{
		MSG_BOX("Failed to Create : CUITool_Level");
		Safe_Release(instance);
	}

	return instance;
}

void CUITool_Level::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}