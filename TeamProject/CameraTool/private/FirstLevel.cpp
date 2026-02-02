#include "pch.h"
#include "FirstLevel.h"
#include "FreeCam.h"
#include "Grid.h"
#include "Corin.h"
#include "JaneDoe.h"
#include "GachaProps.h"
#include "CamPanel.h"

HRESULT CFirstLevel::Awake()
{
	PROTO->Add_ProtoType(G_GlobalLevelKey, "Proto_FreeCam", CFreeCam::Create());
	PROTO->Add_ProtoType(G_GlobalLevelKey, "Proto_Grid",    CGrid::Create());
	PROTO->Add_ProtoType(G_GlobalLevelKey, "Proto_Corin",   Corin::Create());
	PROTO->Add_ProtoType(G_GlobalLevelKey, "Proto_JaneDoe", JaneDoe::Create());

	PROTO->Add_ProtoType(G_GlobalLevelKey, "Proto_Gacha",   CGachaProps::Create());

	auto freeCam = Builder::Create_Object({G_GlobalLevelKey, "Proto_FreeCam"})
		.Camera(aspect)
		.Position({0.f, 2.5f, 2.f})
		.Build("FreeCam");

	auto demoGrid = Builder::Create_Object({G_GlobalLevelKey, "Proto_Grid"})
		.Scale({50.f, 1.f, 50.f})
		.Build("Grid");

	auto corin = Builder::Create_Object({G_GlobalLevelKey, "Proto_Corin"}).Build("Corin");

	auto janeDoe = Builder::Create_Object({G_GlobalLevelKey, "Proto_JaneDoe"}).Build("JaneDoe");

	auto gacha = Builder::Create_Object({G_GlobalLevelKey, "Proto_Gacha"}).Build("Gacha");


	OBJ->Add_Object(freeCam,   {G_GlobalLevelKey, "Camera_Layer"});
	OBJ->Add_Object(corin,     {G_GlobalLevelKey, "Model_Layer" });
	OBJ->Add_Object(janeDoe,   {G_GlobalLevelKey, "Model_Layer" });
	OBJ->Add_Object(gacha,     {G_GlobalLevelKey, "Gacha_Layer"});
	//OBJ->Add_Object(demoGrid,  {G_GlobalLevelKey, "Grid_Layer"});

	CAM->Set_MainCam(freeCam->Get_Component<CCamera>());
		
	OBJECT_HANDLE corinHandle = corin->Get_Handle();
	OBJECT_HANDLE janeHandle = janeDoe->Get_Handle();

	auto camPanel = CCamPanel::Create(GUISystem()->Get_Context());
	camPanel->SetCaptureTarget(static_cast<CCamObj*>(freeCam));
	camPanel->SetSpaceRefCandidates({corinHandle, janeHandle});
	camPanel->SetSpaceReference(janeHandle);
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