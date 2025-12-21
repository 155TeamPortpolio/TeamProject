#include "pch.h"
#include "ModelEditLevel.h"

#include "GameInstance.h"
#include "EditCamera.h""
#include "FreeCam.h"
#include "Camera.h"
#include "EditorSystem.h"

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
    IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
    pProto->Add_ProtoType("ModelEdit_Level", "Proto_GameObject_EditCamera", CFreeCam::Create());

    IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
    CGameObject* Camera = Builder::Create_Object({ "ModelEdit_Level", "Proto_GameObject_EditCamera" }).Camera({ (float)g_iWinSizeX / (float)g_iWinSizeY }).Position({ 0.f, 0.f, -5.f }).Build("Default_Camera");
    pObjMgr->Add_Object(Camera, { "ModelEdit_Level", "Camera_Layer" });
    m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());
    m_pGameInstance->Get_CameraMgr()->Set_ShadowCam(Camera->Get_Component<CCamera>());
	m_pSystem = CEditorSystem::GetInstance();
	if (m_pSystem)
		m_pSystem->Initialize();
    return S_OK;
}

void CModelEditLevel::Update()
{
	m_pSystem->Update();
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
	Safe_Release(m_pSystem);
}
