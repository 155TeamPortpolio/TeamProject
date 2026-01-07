#include "pch.h"
#include "EffectEditLevel.h"

#include "GameInstance.h"
#include "IInputService.h"
#include "ILevelService.h"

#include "ToolCamera.h"
#include "ToolLight.h"
#include "ToolGrid.h"
#include "Camera.h"
#include "EffectContainer_Edit.h"
#include "SpriteNode_Edit.h"
#include "ParticleNode_Edit.h"
#include "MeshNode_Edit.h"
#include "TrailNode_Edit.h"


CEffectEditLevel::CEffectEditLevel(const string& LevelKey)
	: CLevel{ LevelKey },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffectEditLevel::Initialize()
{
	return S_OK;
}

HRESULT CEffectEditLevel::Awake()
{
	IProtoService* pProto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	IResourceService* pResource = CGameInstance::GetInstance()->Get_ResourceMgr();

	pProto->Add_ProtoType("EffectEdit_Level", "Proto_GameObject_ToolCamera", CToolCamera::Create());
	pProto->Add_ProtoType("EffectEdit_Level", "Proto_GameObject_EffectContainer", CEffectContainer_Edit::Create());
	pProto->Add_ProtoType("EffectEdit_Level", "Proto_GameObject_SpriteNode", CSpriteNode_Edit::Create());
	pProto->Add_ProtoType("EffectEdit_Level", "Proto_GameObject_ParticleNode", CParticleNode_Edit::Create());
	pProto->Add_ProtoType("EffectEdit_Level", "Proto_GameObject_MeshNode", CMeshNode_Edit::Create());
	pProto->Add_ProtoType("EffectEdit_Level", "Proto_GameObject_TrailNode", CTrailNode_Edit::Create());
	pProto->Add_ProtoType("EffectEdit_Level", "Proto_GameObject_ToolLight", CToolLight::Create());
	pProto->Add_ProtoType("EffectEdit_Level", "Proto_GameObject_ToolGrid", CToolGrid::Create());

	//pResource->Add_ResourcePath("test.json", "../Bin/Resource/Data/test.json");
	//pResource->Load_EffectAsset(G_GlobalLevelKey, "test.json");

	//		IResourceService* pService = CGameInstance::GetInstance()->Get_ResourceMgr();
	//		pService->Add_ResourcePath("TileCell.png", "../../Resources/TileCell.png");
	//		pService->Add_ResourcePath("VTX_PlaneGrid.hlsl", "../Bin/ShaderFiles/VTX_PlaneGrid.hlsl");

	IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();
	CAMERA_DESC desc = {};

	CGameObject* Camera = Builder::Create_Object({ "EffectEdit_Level" ,"Proto_GameObject_ToolCamera" })
		.Camera({ (float)g_iWinSizeX / g_iWinSizeY })
		.Position({ 0,3,-3 })
		.Build("Main_Camera");

	CGameObject* Grid = Builder::Create_Object({ "EffectEdit_Level" ,"Proto_GameObject_ToolGrid" })
		.Position({ 0,-10.f,0.f })
		.Scale({500.f,0.f,500.f})
		.Build("ToolGrid");

	CGameObject* Effect = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_EffectContainer" })
		.Build("EffectContainer");

	CGameObject* Effect2 = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_EffectContainer" })
		.Build("EffectContainer");

	LIGHT_INIT_DESC LightDesc{};
	LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
	LightDesc.vDiffuse = _float4{ 1.f,1.f,1.f,1.f };
	LightDesc.vDirection = _float4{ 1.f,-1.f,1.f,0.f };
	CGameObject* Light = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_ToolLight" })
		.Light(LightDesc)
		.Build("Tool_Light");
	  
	pObjMgr->Add_Object(Grid, { "EffectEdit_Level","Grid_Layer" });
	pObjMgr->Add_Object(Effect, { "EffectEdit_Level","Edit_Layer" });
	pObjMgr->Add_Object(Effect2, { "EffectEdit_Level","Edit_Layer" });
	pObjMgr->Add_Object(Light, { "EffectEdit_Level","Light_Layer" });
	pObjMgr->Add_Object(Camera, { "EffectEdit_Level","Camera_Layer" });

	m_pGameInstance->Get_CameraMgr()->Set_MainCam(Camera->Get_Component<CCamera>());

	return S_OK;
}

void CEffectEditLevel::Update()
{
}

HRESULT CEffectEditLevel::Render()
{
	SetWindowText(g_hWnd, TEXT("EffectTool"));
	return S_OK;
}

void CEffectEditLevel::PreLoad_Level()
{
}

CEffectEditLevel* CEffectEditLevel::Create(const string& LevelKey)
{
	CEffectEditLevel* instance = new CEffectEditLevel(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("EffectEdit level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CEffectEditLevel::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
