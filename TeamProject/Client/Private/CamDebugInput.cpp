#include "pch.h"
#include "CamDebugInput.h"
// Engine
#include "GameInstance.h"
#include "CamDirector.h"
// Client
#include "UI_Gangta.h"
#include "UI_Seoriyeol.h"

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

    if (InputDevice()->Key_Tap(VK_F3))
    {
        if      (levelKey == "Gacha_Level") CamDirector()->RequestSequence("Gacha/Spin_Half");
        else if (levelKey == "Test_Level")  CamDirector()->RequestSequence(CamSeqType::BattleIntro);
        else if (levelKey == "Zero_Level")  CamDirector()->RequestSequence(CamSeqType::ZeroIntro);
    }

    if (InputDevice()->Key_Tap(VK_F4))
    {
       // if (levelKey == "Gacha_Level") CamDirector()->RequestSequence("Gacha/Spin");
        if (levelKey == "Gacha_Level" || levelKey == "Zero_Level")
        {
            //auto obj = Builder::Create_UIObject({G_GlobalLevelKey, "Proto_GameObject_Gangta"}).Build("Gangta");
            //UIManager()->Add_UIObject(obj, LevelManager()->Get_NowLevelKey());
            //static_cast<CUI_Gangta*>(obj)->UI_Active({});

            auto obj = Builder::Create_UIObject({G_GlobalLevelKey, "Proto_GameObject_Seoriyeol"}).Build("Seoriyeol");
            UIManager()->Add_UIObject(obj, LevelManager()->Get_NowLevelKey());
            static_cast<CUI_Seoriyeol*>(obj)->UI_Active({});
        }
    }

    if (CamDirector()->IsFinished(CamEventType::SpinFinished) || CamDirector()->IsFinished(CamEventType::SpinHalfFinished))
    {
        CameraManager()->SetZoomType(ENUM(CamZoomType::GachaShake), 1.8f);
        //CameraManager()->SetShakeType(ENUM(CamShakeType::EarthquakeShort), 1.2f);
        //CameraManager()->AddShakeAxisWave(0x4, 3.0f, 3.0f, 1.2f, 0.0f, EaseType::None, EaseType::OutCubic);
       // CameraManager()->AddShakeAxisWave(CamShakeAxis::Roll, 3.0f, 3.0f, 1.2f, 0.0f, EaseType::None, EaseType::OutCubic);
        CameraManager()->AddShakeAxisWave(CamShakeAxis::Roll,  3.f,  4.0f, 0.8f, 0.1f, EaseType::InQuad, EaseType::InOutQuad);
        CameraManager()->AddShakeAxisWave(CamShakeAxis::Yaw,   1.4f, 3.0f, 0.6f, 0.1f, EaseType::InQuad, EaseType::InOutQuad);
        CameraManager()->AddShakeAxisWave(CamShakeAxis::Pitch, 1.f,  2.5f, 0.4f, 0.1f, EaseType::InQuad, EaseType::InOutQuad);
    }
}