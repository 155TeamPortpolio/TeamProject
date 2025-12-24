#include "pch.h"
#include "ModelEditLevel.h"

#include "GameInstance.h"
#include "EditCamera.h""
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
    IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
    CGameObject* Camera = Builder::Create_Object({G_GlobalLevelKey, "Proto_GameObject_EditCamera" }).Camera({ (float)g_iWinSizeX / (float)g_iWinSizeY }).Position({ 0.f, 0.f, -5.f }).Build("Default_Camera");
    
    m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());
    m_pGameInstance->Get_CameraMgr()->Set_ShadowCam(Camera->Get_Component<CCamera>());
	auto Model = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_EditModel" }).Build("Parse Obj");
	m_pGameInstance->Get_ObjectMgr()->Add_Object(Model, { "ModelEdit_Level","Model_Layer"});
	pObjMgr->Add_Object(Camera, { "ModelEdit_Level", "Camera_Layer" });
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
