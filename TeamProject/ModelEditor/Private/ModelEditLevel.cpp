#include "pch.h"
#include "ModelEditLevel.h"

#include "GameInstance.h"
#include "EditCamera.h""
#include "Camera.h"
#include "FreeCam.h"
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
	m_Cameras.resize(2, nullptr);

	m_Cameras[0] = Builder::Create_Object({G_GlobalLevelKey, "Proto_GameObject_EditCamera"}).Camera({(float)g_iWinSizeX / (float)g_iWinSizeY}).Position({0.f, 0.f, -5.f}).Build("Default_Camera1");
	m_Cameras[1] = Builder::Create_Object({G_GlobalLevelKey, "Proto_GameObject_EditCamera"}).Camera({(float)g_iWinSizeX / (float)g_iWinSizeY}).Position({0.f, 0.f, -5.f}).Build("Default_Camera2");
    
	auto Model = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_EditModel" }).Build("Parse Obj");
	m_pGameInstance->Get_ObjectMgr()->Add_Object(Model, { "ModelEdit_Level","Model_Layer"});
	pObjMgr->Add_Object(m_Cameras[0], { "ModelEdit_Level", "Camera_Layer" });
	pObjMgr->Add_Object(m_Cameras[1], { "ModelEdit_Level", "Camera_Layer" });


	m_pGameInstance->Get_CameraMgr()->Set_MainCam(m_Cameras[0]->Get_Component<CCamera>());
	m_pGameInstance->Get_CameraMgr()->Set_ShadowCam(m_Cameras[0]->Get_Component<CCamera>());
	dynamic_cast<CFreeCam*>(m_Cameras[nowCam])->IsMain(true);
	return S_OK;
}

void CModelEditLevel::Update()
{
	if (InputDevice()->Key_Tap(VK_CAPITAL)) {
		dynamic_cast<CFreeCam*>(m_Cameras[nowCam])->IsMain(false);
		//.
		nowCam++;
		if (nowCam > 1)
			nowCam = 0;
		m_pGameInstance->Get_CameraMgr()->Set_MainCam(m_Cameras[nowCam]->Get_Component<CCamera>());
		dynamic_cast<CFreeCam*>(m_Cameras[nowCam])->IsMain(true);
	}
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
