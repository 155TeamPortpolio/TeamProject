#include "pch.h"
#include "UITool_Level.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "IInputService.h"
#include "ILevelService.h"

#include "UITool_Camera.h"
#include "Camera.h"

#include "GUIPanel.h"
#include "CanvasPanel.h"
#include "ImageUI.h"
#include "TextUI.h"
#include "ButtonUI.h"
#include "SpriteAnimationUI.h"

vector<string> CUITool_Level::m_strTextureKeys;
vector<const _char*> CUITool_Level::m_szTextureKeys;
vector<string> CUITool_Level::m_strFontKeys;
vector<const _char*> CUITool_Level::m_szFontKeys;

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
	if (FAILED(Ready_Camera()))
		MSG_BOX("Failed to Ready Camera");

	if (FAILED(Ready_GUIPanel()))
		MSG_BOX("Failed to Ready GUI Panel");

	Ready_Textures();

	Ready_Fonts();

	Ready_UIObjects();	 

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
	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	for (const auto& entry : filesystem::recursive_directory_iterator("../Bin/Resources/UI/"))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".png" ||
			entry.is_regular_file() && entry.path().extension() == ".jpg" ||
			entry.is_regular_file() && entry.path().extension() == ".dds")
		{
			filesystem::path filePath = entry.path();	

			if (FAILED(pResourceMgr->Add_ResourcePath(filePath.filename().string(), filePath.string())))
				break;

			if(filePath.filename().string() != "PanelBox.dds")
				m_strTextureKeys.push_back(filePath.filename().string());
		}
	}

	for (const auto& Key : m_strTextureKeys)
		m_szTextureKeys.push_back(Key.c_str());

	return S_OK;
}

HRESULT CUITool_Level::Ready_Fonts()
{
	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	for (const auto& entry : filesystem::recursive_directory_iterator("../Bin/Resources/Font/"))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".spritefont")
		{
			filesystem::path filePath = entry.path();

			if (FAILED(m_pGameInstance->Get_FontSystem()->Add_Font(filePath.filename().string(), Helper::ConvertToWideString(filePath.string()))))
				break;

			m_strFontKeys.push_back(filePath.filename().string());
		}
	}

	for (const auto& Key : m_strFontKeys)
		m_szFontKeys.push_back(Key.c_str());

	return S_OK;
}

HRESULT CUITool_Level::Ready_Camera()
{
	IProtoService* pProto = m_pGameInstance->Get_PrototypeMgr();

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
	IProtoService* pProto = m_pGameInstance->Get_PrototypeMgr();

	pProto->Add_ProtoType("UITool_Level", "Proto_GameObject_CanvasPanel", CCanvasPanel::Create());

	pProto->Add_ProtoType("UITool_Level", "Proto_GameObject_ImageUI", CImageUI::Create());

	pProto->Add_ProtoType("UITool_Level", "Proto_GameObject_TextUI", CTextUI::Create());

	pProto->Add_ProtoType("UITool_Level", "Proto_GameObject_ButtonUI", CButtonUI::Create());

	pProto->Add_ProtoType("UITool_Level", "Proto_GameObject_SpriteAnimationUI", CSpriteAnimationUI::Create());

	return S_OK;
}

HRESULT CUITool_Level::Ready_GUIPanel()
{
	CBasePanel* pPanel = CGUIPanel::Create(m_pGameInstance->Get_GUISystem()->Get_Context());
	if (!pPanel)
		return E_FAIL;

	m_pGameInstance->Get_GUISystem()->Register_Panel(pPanel);

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