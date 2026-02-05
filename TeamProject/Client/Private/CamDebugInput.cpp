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

    if (InputDevice()->Key_Tap(VK_F1))
    {
        if (CamDirector()->IsPlaying(CamDirector()->GetCurSeqName())) CamDirector()->AbortSequenceToOrbit(true);
        CameraManager()->Set_MainCam(CamDirector()->GetFreeCamComp(), 0.5f);
    }

    if (InputDevice()->Key_Tap(VK_F2))
    {
        if (CamDirector()->IsPlaying(CamDirector()->GetCurSeqName())) CamDirector()->AbortSequenceToOrbit(true);
        CameraManager()->Set_MainCam(CamDirector()->GetOrbitCamComp(), 0.5f);
    }

    CMonitorGate gate;
    if (!gate.Pass()) return;

    if (InputDevice()->Mouse_Tap(MOUSE_BTN::RB))
    {
        CamDirector()->GetOrbitCam()->ClearLockOn();
    }


    if (InputDevice()->Key_Tap(VK_F3))
    {
        if (levelKey == "Gacha_Level")
        {
            auto obj = Builder::Create_UIObject({G_GlobalLevelKey, "Proto_GameObject_Gangta"}).Build("Gangta");
            UIManager()->Add_UIObject(obj, LevelManager()->Get_NowLevelKey());
            static_cast<CUI_Gangta*>(obj)->UI_Active({});
        }
        else if (levelKey == "Test_Level")  CamDirector()->RequestSequence(CamSeqType::BattleIntro);

        else if (levelKey == "Zero_Level")  CamDirector()->RequestSequence(CamSeqType::ZeroIntro);
    }

    if (InputDevice()->Key_Tap(VK_F4))
    {
        if (levelKey == "Gacha_Level" || levelKey == "Zero_Level")
        {
            auto obj = Builder::Create_UIObject({G_GlobalLevelKey, "Proto_GameObject_Seoriyeol"}).Build("Seoriyeol");
            UIManager()->Add_UIObject(obj, LevelManager()->Get_NowLevelKey());
            static_cast<CUI_Seoriyeol*>(obj)->UI_Active({});
        }
    }
}