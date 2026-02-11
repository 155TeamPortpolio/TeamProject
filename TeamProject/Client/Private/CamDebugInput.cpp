#include "pch.h"
#include "CamDebugInput.h"
// Engine
#include "GameInstance.h"
#include "CamDirector.h"
#include "Animator3D.h"
#include "InputMgr.h"
// Client
#include "UI_Gangta.h"
#include "UI_Seoriyeol.h"
#include "BattleSystem.h"

void CamDebugInput::UpdateInput(_float dt)
{
    const auto& levelKey = LevelManager()->Get_NowLevelKey();

    auto& cam = *CamDirector();
    auto& mgr = *CameraManager();

    if (InputDevice()->Key_Tap(VK_F1))
    {
        if (cam.IsPlaying(cam.GetCurSeqName()))
            cam.AbortSequenceToOrbit(true);
        mgr.Set_MainCam(cam.GetFreeCamComp(), 0.5f);
    }

    if (InputDevice()->Key_Tap(VK_F2))
    {
        if (cam.IsPlaying(cam.GetCurSeqName()))
            cam.AbortSequenceToOrbit(true);
        mgr.Set_MainCam(cam.GetOrbitCamComp(), 0.5f);
    }

    if (cam.IsFinished(CamEventType::UltimateFinished))
        cam.RequestSequence("Battle/Back");

    JehyunInput(dt);
}

void CamDebugInput::JehyunInput(_float dt)
{
    CMonitorGate gate;
    if (!gate.Pass()) return;

    auto& cam = *CamDirector();
    auto& mgr = *CameraManager();

    if (InputDevice()->Key_Tap(VK_F3))
    {
        cam.RequestSequence("WipeOutIntro/Back");
    }

    auto time = cam.GetTime();

    if (cam.GetCurSeqName() == "WipeOutIntro/Back" && time > 0.3f)
    {
        mgr.AddShakeAxis(CamShakeAxis::Roll, 1.f, 10.f, 0.025f, 0.f);
        mgr.AddShakeAxis(CamShakeAxis::Yaw, 0.4f, 10.f, 0.025f, 0.f);
    }

    if (cam.IsFinished(CamEventType::WipeOutBackFinished))
        cam.RequestSequence("WipeOut/Right");

    if (cam.IsFinished(CamEventType::WipeOutRightFinished))
        cam.RequestSequence("WipeOut/Front");

    if (cam.IsFinished(CamEventType::WipeOutFrontFinished))
        cam.RequestSequence("WipeOut/Left");
}