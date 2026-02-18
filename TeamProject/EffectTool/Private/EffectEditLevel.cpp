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
#include "ToolModel.h"
#include "ToolMap.h"
#include "Tool_WaterWave.h"

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
	pProto->Add_ProtoType("EffectEdit_Level", "Proto_GameObject_ToolModel", CToolModel::Create());
	pProto->Add_ProtoType("EffectEdit_Level", "Proto_GameObject_ToolMap", CToolMap::Create());
	pProto->Add_ProtoType("EffectEdit_Level", "Proto_GameObject_ToolWaterWave", CWaterWave::Create());

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
		.Position({ 0,-3.f,0.f })
		.Scale({500.f,0.f,500.f})
		.Build("ToolGrid");

	CGameObject* Effect = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_EffectContainer" })
		.Build("EffectContainer");

	CGameObject* Effect2 = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_EffectContainer" })
		.Build("EffectContainer");

	CGameObject* Effect3 = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_EffectContainer" })
		.Build("EffectContainer");

	CGameObject* Effect4 = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_EffectContainer" })
		.Build("EffectContainer");

	CGameObject* Effect5 = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_EffectContainer" })
		.Build("EffectContainer");

	CGameObject* Model = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_ToolModel" })
		.Build("Model");
	
	CGameObject* Map = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_ToolMap" })
		.Build("Map");

	CWaterWave::WaterWaveDesc* waveDesc = new CWaterWave::WaterWaveDesc;
	waveDesc->fFoamAmount = 1.0f;
	waveDesc->fRoughness = 1.0f;
	waveDesc->fTintStrength = 0.f;
	waveDesc->fTsunamiHeight = 50.f;

	waveDesc->vWaterTint = _float3(1.f, 1.f, 1.f);
	waveDesc->fNoiseOffset = _float2(0.f, 0.f);

	waveDesc->fPeakTime = 0.90f;			
	waveDesc->fRiseWidth = 0.85f;			
	waveDesc->fFallWidth = 0.05f;			

	waveDesc->fCurlStartRatio = 0.95f;		
	waveDesc->fCurlDuration = 0.08f;		

	waveDesc->fFadeInEnd = 0.12f;			
	waveDesc->fFadeOutStart = 0.995f;		

	waveDesc->fCurlForward = 45.f;			
	waveDesc->fMaxCurlAngle = 3.8f;			

	CGameObject* WaterWave =
		Builder::Create_Object({ "EffectEdit_Level", "Proto_GameObject_ToolWaterWave" })
		.Add_ObjDesc(waveDesc)
		.Scale({ 0.1f,1.f,6.f })
		.Position({ -55.f,-4,0.f })
		.Build("WaterWave1");

	LIGHT_INIT_DESC LightDesc{};
	LightDesc.eType = LIGHT_TYPE::DIRECTIONAL;
	LightDesc.vDiffuse = _float4{ 1.f,1.f,1.f,1.f };
	LightDesc.vDirection = _float4{ 1.f,-1.f,1.f,0.f };
	CGameObject* Light = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_ToolLight" })
		.Light(LightDesc)
		.Build("Tool_Light");
	  
	pObjMgr->Add_Object(Map, { "EffectEdit_Level","Map_Layer" });
	pObjMgr->Add_Object(Grid, { "EffectEdit_Level","Grid_Layer" });
	pObjMgr->Add_Object(Effect, { "EffectEdit_Level","Edit_Layer" });
	pObjMgr->Add_Object(Effect2, { "EffectEdit_Level","Edit_Layer" });
	pObjMgr->Add_Object(Effect3, { "EffectEdit_Level","Edit_Layer" });
	pObjMgr->Add_Object(Effect4, { "EffectEdit_Level","Edit_Layer" });
	pObjMgr->Add_Object(Effect5, { "EffectEdit_Level","Edit_Layer" });
	pObjMgr->Add_Object(Model, { "EffectEdit_Level","Model_Layer" });
	pObjMgr->Add_Object(Light, { "EffectEdit_Level","Light_Layer" });
	pObjMgr->Add_Object(Camera, { "EffectEdit_Level","Camera_Layer" });
	pObjMgr->Add_Object(WaterWave, { "EffectEdit_Level","Model_Layer" });

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
