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

    if (levelKey == "Gacha_Level")
        UpdateGachaLevel();
}

void CamDebugInput::UpdateGachaLevel()
{
    auto& cam = *CamDirector();
    auto& mgr = *CameraManager();

    if (cam.IsFinished(CamEventType::Miyabi_01_Finished))
        cam.RequestSequence("Gacha/Miyabi_02");

    if (cam.IsFinished(CamEventType::Miyabi_02_Finished))
        cam.RequestSequence("Gacha/Miyabi_03");

    if (cam.IsFinished(CamEventType::Miyabi_03_Finished))
        cam.RequestSequence("Gacha/Miyabi_01");

    if (cam.IsFinished(CamEventType::JaneDoe_01_Finished))
        cam.RequestSequence("Gacha/JaneDoe_02");

    if (cam.IsFinished(CamEventType::JaneDoe_02_Finished))
        cam.RequestSequence("Gacha/JaneDoe_03");

    if (cam.IsFinished(CamEventType::JaneDoe_03_Finished))
        cam.RequestSequence("Gacha/JaneDoe_01");
}