#include "pch.h"
#include "FirstLevel.h"
#include "FreeCam.h"
#include "Unagi.h"
#include "Grid.h"

#include "CamPanel.h"

HRESULT CFirstLevel::Awake()
{
	PROTO->Add_ProtoType("First_Level", "Proto_FreeCam", CFreeCam::Create());
	PROTO->Add_ProtoType("First_Level", "Proto_Unagi",   CUnagi::Create());
	PROTO->Add_ProtoType("First_Level", "Proto_Grid",    CGrid::Create());

	CGameObject* freeCam = Builder::Create_Object({ "First_Level", "Proto_FreeCam" })
		.Camera(aspect)
		.Position({ 0.f, 3.f, -5.f })
		.Build("FreeCam");

	CGameObject* demoModel = Builder::Create_Object({ "First_Level", "Proto_Unagi" })
		.Position({})
		.Rotate(Vector3{0.f, XMConvertToRadians(180.f), 0.f})
		.Build("Unagi");

	CGameObject* demoGrid = Builder::Create_Object({ "First_Level", "Proto_Grid" })
		.Position({ 0, 0, 0 })
		.Scale({ 50.f, 1.f, 50.f })
		.Build("Grid");

	OBJ->Add_Object(freeCam,   { "First_Level", "Camera_Layer" });
	OBJ->Add_Object(demoModel, { "First_Level", "Model_Layer"  });
	OBJ->Add_Object(demoGrid,  { "First_Level", "Grid_Layer"   });

	CAM->Set_MainCam(freeCam->Get_Component<CCamera>());

	OBJECT_HANDLE objHandle = demoModel->Get_Handle();
	auto camPanel = CCamPanel::Create(GUI->Get_Context());
	camPanel->SetCaptureTarget(static_cast<CCamObj*>(freeCam));
	camPanel->SetSpaceReference(objHandle);
	GUI->Register_Panel(camPanel);
	return S_OK;
}

CFirstLevel* CFirstLevel::Create(const string& key)
{
	auto inst = new CFirstLevel(key);
	inst->Initialize();
	return inst;
}