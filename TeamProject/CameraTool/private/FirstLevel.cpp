#include "pch.h"
#include "FirstLevel.h"
#include "FreeCam.h"
#include "Unagi.h"
#include "Grid.h"

#include "CamPanel.h"

HRESULT CFirstLevel::Awake()
{
	PROTO->Add_ProtoType("First_Level", "Proto_FreeCam", CFreeCam::Create());
	PROTO->Add_ProtoType("First_Level", "Proto_Unagi", CUnagi::Create());
	PROTO->Add_ProtoType("First_Level", "Proto_Grid", CGrid::Create());

	//PROTO->Add_ProtoType("First_Level", "Proto_Corin", CPlayer::Create());


	CGameObject* freeCam = Builder::Create_Object({"First_Level", "Proto_FreeCam"})
		.Camera(aspect)
		.Position({0.f, 2.5f, 2.f})
		.Build("FreeCam");

	CGameObject* demoModel = Builder::Create_Object({"First_Level", "Proto_Unagi"})
		.Build("Unagi");

	CGameObject* demoGrid = Builder::Create_Object({"First_Level", "Proto_Grid"})
		.Scale({50.f, 1.f, 50.f})
		.Build("Grid");


	/*CGameObject* corin = Builder::Create_Object({"First_Level", "Proto_Corin"})
		.Scale({0.01f, 0.01f, 0.01f})
		.Build("Corin");*/

		//OBJ->Add_Object(corin, {"First_Level", "Model_Layer"});


	OBJ->Add_Object(freeCam,   {"First_Level", "Camera_Layer"});
	OBJ->Add_Object(demoModel, {"First_Level", "Model_Layer"});
	//OBJ->Add_Object(demoGrid,  {"First_Level", "Grid_Layer"});

	CAM->Set_MainCam(freeCam->Get_Component<CCamera>());

	OBJECT_HANDLE objHandle = demoModel->Get_Handle();

	auto camPanel = CCamPanel::Create(GUI->Get_Context());
	camPanel->SetCaptureTarget(static_cast<CCamObj*>(freeCam));
	camPanel->SetSpaceReference(objHandle);
	camPanel->SetAvatarUI(Avatar::JaneDoe);

	auto avatar = static_cast<CUnagi*>(demoModel);
	avatar->ApplyAvatar(Avatar::JaneDoe);

	camPanel->SetOnAvatarChanged([avatar](Avatar p)
		{
			avatar->ApplyAvatar(p);
		});

	GUI->Register_Panel(camPanel);
	return S_OK;
}

void CFirstLevel::Update()
{
	if(InputDevice()->Key_Tap('Q'))
		exit(0);
}

CFirstLevel* CFirstLevel::Create(const string& key)
{
	auto inst = new CFirstLevel(key);
	inst->Initialize();
	return inst;
}