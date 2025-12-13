#include "pch.h"
#include "UITool_Level.h"

#include "GameInstance.h"
#include "IInputService.h"
#include "ILevelService.h"

#include "UITool_Camera.h"
#include "Camera.h"

#include "GUIPanel.h"
#include "CanvasPanel.h"
#include "ImageUI.h"
#include "TextUI.h"

vector<string> CUITool_Level::m_TextureKeys;
vector<string> CUITool_Level::m_FontKeys;

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

const vector<const _char*> CUITool_Level::Get_TextureKeys()
{
	vector<const _char*> TextureKeys;

	for (const auto& Key : m_TextureKeys)
		TextureKeys.push_back(Key.c_str());

	return TextureKeys;
}

const vector<const _char*> CUITool_Level::Get_FontKeys()
{
	vector<const _char*> FontKeys;

	for (const auto& Key : m_FontKeys)
		FontKeys.push_back(Key.c_str());

	return FontKeys;
}

HRESULT CUITool_Level::Ready_Textures()
{ 
	Add_Texture("Logo.png", "../Bin/Resources/UI/Logo.png");
	Add_Texture("Bangboo.jpg", "../Bin/Resources/UI/Bangboo.jpg");

	if (FAILED(m_pGameInstance->Get_ResourceMgr()->Add_ResourcePath("CanvasPanel.png", "../Bin/Resources/UI/CanvasPanel.png")))
		MSG_BOX("Failed to Ready Textures : CanvasPanel.png");

	return S_OK;
}

HRESULT CUITool_Level::Ready_Fonts()
{
	Add_Font("NotoSansKR_Regular_20", L"../Bin/Resources/Fonts/NotoSansKR_Regular_20.spritefont");
	Add_Font("NotoSansKR_Regular_24", L"../Bin/Resources/Fonts/NotoSansKR_Regular_24.spritefont");
	Add_Font("NotoSansKR_Regular_32", L"../Bin/Resources/Fonts/NotoSansKR_Regular_32.spritefont");

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

HRESULT CUITool_Level::Add_Texture(const string& resourceKey, const string& resourcePath)
{
	if (FAILED(m_pGameInstance->Get_ResourceMgr()->Add_ResourcePath(resourceKey, resourcePath)))
		return E_FAIL;
	m_TextureKeys.push_back(resourceKey);

	return S_OK;
}

HRESULT CUITool_Level::Add_Font(string FontName, const wstring& FontFilePath)
{
	m_pGameInstance->Get_FontSystem()->Add_Font(FontName, FontFilePath);
	m_FontKeys.push_back(FontName);

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