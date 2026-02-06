#include "pch.h"
#include "FirstLevel.h"
#include "FreeCam.h"
#include "Grid.h"
#include "Corin.h"
#include "JaneDoe.h"

#include "CamPanel.h"
#include "Unagi.h"
// Gacha
#include "GachaProps.h"
#include "GachaFootStage.h"

HRESULT CFirstLevel::Awake()
{
	PROTO->Add_ProtoType(G_GlobalLevelKey, "Proto_FreeCam",         CFreeCam::Create());
	PROTO->Add_ProtoType(G_GlobalLevelKey, "Proto_Grid",            CGrid::Create());
	PROTO->Add_ProtoType(G_GlobalLevelKey, "Proto_Corin",           Corin::Create());
	PROTO->Add_ProtoType(G_GlobalLevelKey, "Proto_JaneDoe",         JaneDoe::Create());
	PROTO->Add_ProtoType(G_GlobalLevelKey, "Proto_Miyabi",          Unagi::Create());
	PROTO->Add_ProtoType(G_GlobalLevelKey, "Proto_Gacha",           CGachaProps::Create());
	PROTO->Add_ProtoType(G_GlobalLevelKey, "Proto_Gacha_FootStage", CGachaFootStage::Create());

	auto freeCam = Builder::Create_Object({G_GlobalLevelKey, "Proto_FreeCam"})
		.Camera(aspect)
		.Position({0.f, 1.4f, -4.f})
		.Build("FreeCam");

	freeCam->Get_Component<CTransform>()->LookAt(Vector3(0.f, 3.f, -1.6f));

	auto demoGrid = Builder::Create_Object({G_GlobalLevelKey, "Proto_Grid"})
		.Scale({50.f, 1.f, 50.f})
		.Build("Grid");

	auto corin     = Builder::Create_Object({G_GlobalLevelKey, "Proto_Corin"  }).Build("Corin");
	auto janeDoe   = Builder::Create_Object({G_GlobalLevelKey, "Proto_JaneDoe"})
		.Position(Vector3(0.f, 0.15f, -1.6f))
		.Rotate(Vector3(0.f, XMConvertToRadians(180.f), 0.f))
		.Build("JaneDoe");

	auto miyabi    = Builder::Create_Object({G_GlobalLevelKey, "Proto_Miyabi" })
		.Position(Vector3(0.f, 0.3f, -1.6f))
		.Rotate(Vector3(0.f, XMConvertToRadians(180.f), 0.f))
		.Build("Miyabi");

	auto gacha     = Builder::Create_Object({G_GlobalLevelKey, "Proto_Gacha"  }).Build("Gacha");
	auto footStage = Builder::Create_Object({G_GlobalLevelKey, "Proto_Gacha_FootStage"})
		.Position(Vector3(0.f, 0.3f, -1.4f))
		.Build("FootStage");

	OBJ->Add_Object(freeCam,   {G_GlobalLevelKey, "Camera_Layer"});
	OBJ->Add_Object(corin,     {G_GlobalLevelKey, "Model_Layer" });
	OBJ->Add_Object(janeDoe,   {G_GlobalLevelKey, "Model_Layer" });
	OBJ->Add_Object(miyabi,    {G_GlobalLevelKey, "Model_Layer" });
	OBJ->Add_Object(gacha,     {G_GlobalLevelKey, "Gacha_Layer" });
//	OBJ->Add_Object(footStage, {G_GlobalLevelKey, "Gacha_Layer" });

	//OBJ->Add_Object(demoGrid,  {G_GlobalLevelKey, "Grid_Layer"  });

	CAM->Set_MainCam(freeCam->Get_Component<CCamera>());
		
	auto corinHandle  = corin->Get_Handle();
	auto janeHandle   = janeDoe->Get_Handle();
	auto miyabiHandle = miyabi->Get_Handle();

	auto camPanel = CCamPanel::Create(GUISystem()->Get_Context());
	camPanel->SetCaptureTarget(static_cast<CCamObj*>(freeCam));
	camPanel->SetSpaceRefCandidates({corinHandle, janeHandle, miyabiHandle});
	camPanel->SetSpaceReference(miyabiHandle);
	GUISystem()->Register_Panel(camPanel);

	return S_OK;
}

void CFirstLevel::Update()
{
}

CFirstLevel* CFirstLevel::Create(const string& key)
{
	auto inst = new CFirstLevel(key);
	inst->Initialize();
	return inst;
}