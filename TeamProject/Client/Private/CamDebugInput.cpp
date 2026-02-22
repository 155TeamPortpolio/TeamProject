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

#include "ZeroPortal.h"
#include "CamPortalController.h"

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
    

    DebugInput(dt);
}

void CamDebugInput::DebugInput(_float dt)
{
    //CMonitorGate gate;
    //if (!gate.Pass()) return;

    auto& cam = *CamDirector();
    auto& mgr = *CameraManager();

    if (InputDevice()->Key_Tap(VK_F3))
    {
        cam.GetOrbitCam()->Lock_Input();
    }

    if (InputDevice()->Key_Tap(VK_F4))
    {
        cam.GetOrbitCam()->Unlock_Input();
    }
}