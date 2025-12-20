#include "pch.h"
#include "ModelEditLevel.h"

#include "GameInstance.h"
#include "EditCamera.h"
#include "EditModel.h"
#include "Camera.h"

#include "LightObject.h"

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
	pProto->Add_ProtoType("ModelEdit_Level", "Proto_GameObject_EditCamera", CEditCamera::Create());
	pProto->Add_ProtoType("ModelEdit_Level", "Proto_GameObject_EditModel", CEditModel::Create());
	pProto->Add_ProtoType("ModelEdit_Level", "Proto_GameObject_LightObject", CLightObject::Create());


	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	CAMERA_DESC desc = {};

	CGameObject* Camera = Builder::Create_Object({ "ModelEdit_Level" ,"Proto_GameObject_EditCamera"})
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0.f, 0.f, -5.f })
		.Build("Default_Camera");

	CGameObject* EditModel = Builder::Create_Object({ "ModelEdit_Level" ,"Proto_GameObject_EditModel"})
		.Position({ 0.f,0.f,0.f })
		.Build("Edit_Model");

	pObjMgr->Add_Object(EditModel, { "ModelEdit_Level","Model_Layer"});
	pObjMgr->Add_Object(Camera, { "ModelEdit_Level","Camera_Layer"});


	const _int count = 10;
	const _float radius = 150.f; 
	const _float y = 10.f;    
	const _float twoPi = 6.283185307179586f;

	for (int i = 0; i < count; ++i)
	{
		_float t = static_cast<_float>(i) / static_cast<_float>(count); // 0~1
		_float ang = t * twoPi;

		_float x = cosf(ang) * radius;
		_float z = sinf(ang) * radius;

		string key = "Light_" + std::to_string(i);

		CGameObject* pLight = Builder::Create_Object({ "ModelEdit_Level", "Proto_GameObject_LightObject" })
			.Position({ x, y, z })
			.Build(key.c_str());

		pObjMgr->Add_Object(pLight, { "ModelEdit_Level", "Light_Layer" });
	}

	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());
	m_pGameInstance->Get_CameraMgr()->Set_ShadowCam(Camera->Get_Component<CCamera>());
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
