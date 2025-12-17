#include "Engine_Defines.h"
#include "CameraMgr.h"
#include "Camera.h"
#include "IAudioService.h"
#include "GameInstance.h"
#include "GameObject.h"

CCameraMgr::CCameraMgr()
{
	view          = Matrix::Identity;
	proj          = Matrix::Identity;
	invView       = Matrix::Identity;
	invProj       = Matrix::Identity;

	shadowView    = Matrix::Identity;
	shadowProj    = Matrix::Identity;
	shadowInvView = Matrix::Identity;
	shadowInvProj = Matrix::Identity;

	camPos        = { 0.f, 0.f, 0.f, 1.f };
	shadowCamPos  = { 0.f, 0.f, 0.f, 1.f };
}

void CCameraMgr::Set_MainCam(CCamera* camComp)
{
	if (mainCam)
		Safe_Release(mainCam);

	mainCam = camComp;

	if (mainCam)
		Safe_AddRef(mainCam);
}

void CCameraMgr::Set_ShadowCam(CCamera* camComp)
{
	if (shadowCam)
		Safe_Release(shadowCam);

	shadowCam = camComp;

	Safe_AddRef(shadowCam);
}

void CCameraMgr::Update(_float dt)
{
	if (mainCam)
	{
		view          = mainCam->Get_ViewMatrix();
		proj          = mainCam->Get_ProjMatrix();
		invView       = view.Invert();
		invProj       = proj.Invert();
		camPos        = mainCam->Get_Pos();
	}
	if (shadowCam)
	{
		shadowView    = shadowCam->Get_ViewMatrix();
		shadowProj    = shadowCam->Get_ProjMatrix();
		shadowInvView = shadowView.Invert();
		shadowInvProj = shadowProj.Invert();
		shadowCamPos  = shadowCam->Get_Pos();
	}
}

void CCameraMgr::Free()
{
	Safe_Release(mainCam);
	Safe_Release(shadowCam);
}
