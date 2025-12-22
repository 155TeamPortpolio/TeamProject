#include "pch.h"
#include "MapParseLevel.h"

#include "GameInstance.h"
#include "EditCamera.h"
#include "EditModel.h"
#include "Camera.h"
#include "FreeCam.h"

CMapParseLevel::CMapParseLevel(const string& LevelKey)
	: CLevel{ LevelKey },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMapParseLevel::Initialize()
{
	return S_OK;
}

HRESULT CMapParseLevel::Awake()
{

	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	CGameObject* Camera = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_EditCamera" }).Camera({ (float)g_iWinSizeX / (float)g_iWinSizeY }).Position({ 0.f, 0.f, -5.f }).Build("Default_Camera");
	pObjMgr->Add_Object(Camera, { "MapParse_Level", "Camera_Layer" });

	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());
	m_pGameInstance->Get_CameraMgr()->Set_ShadowCam(Camera->Get_Component<CCamera>());

    return S_OK;
}

void CMapParseLevel::Update()
{
	
}

HRESULT CMapParseLevel::Render()
{
	SetWindowText(g_hWnd, TEXT("데모 레벨입니다."));
	return S_OK;
}

void CMapParseLevel::PreLoad_Level()
{
}

CMapParseLevel* CMapParseLevel::Create(const string& LevelKey)
{
    CMapParseLevel* instance = new CMapParseLevel(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Map level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CMapParseLevel::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
