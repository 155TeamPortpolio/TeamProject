#include "UITool_Defines.h"
#include "UITool_Level.h"

#include "GameInstance.h"
#include "IInputService.h"
#include "ILevelService.h"

#include "UITool_Camera.h"
#include "Camera.h"
#include "TestUI.h"

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
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	pProto->Add_ProtoType("UITool_Level", "Proto_GameObject_Camera", CUITool_Camera::Create());
	pProto->Add_ProtoType("UITool_Level", "Proto_GameObject_UI", CTestUI::Create());
	
	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	IUI_Service* pUIMgr = m_pGameInstance->Get_UIMgr();

	CGameObject* Camera = Builder::Create_Object({ "UITool_Level" ,"Proto_GameObject_Camera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0.f, 3.f, -3.f })
		.Build("Main_Camera");
	
	CUI_Object* pUIObject = Builder::Create_UIObject({ "UITool_Level" ,"Proto_GameObject_UI" })
		.Scale({ 200.f, 200.f })
		.Set_Anchor(ANCHOR::Left | ANCHOR::Top, {0.f, 0.f})
		.Build("Test_UI");

	pObjMgr->Add_Object(Camera, { "UITool_Level","Camera_Layer" });
	pUIMgr->Add_UIObject(pUIObject, "UITool_Level");
	
	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());

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