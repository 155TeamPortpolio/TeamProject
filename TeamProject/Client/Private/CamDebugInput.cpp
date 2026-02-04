#include "pch.h"
#include "CamDebugInput.h"
// Engine
#include "GameInstance.h"
#include "CamDirector.h"
#include "Animator3D.h"
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
        if (levelKey == "Gacha_Level")
        {
            auto obj = Builder::Create_UIObject({G_GlobalLevelKey, "Proto_GameObject_Gangta"}).Build("Gangta");
            UIManager()->Add_UIObject(obj, LevelManager()->Get_NowLevelKey());
            static_cast<CUI_Gangta*>(obj)->UI_Active({});
        }
        else if (levelKey == "Test_Level")  CamDirector()->RequestSequence(CamSeqType::BattleIntro);

        //else if (levelKey == "Test_Level" && CamDirector()->GetCharacterName() == CHARACTER::Miyabi)
        //{
        //    CamDirector()->RequestSequence(CamSeqType::ZeroIntro);
        //    CamDirector()->GetCharacter()->Get_Component<CAnimator3D>()->Set_Animation("Avatar_Female_Size02_Unagi_Ani_QuestStart").Apply();
        //}

        else if (levelKey == "Zero_Level")  CamDirector()->RequestSequence(CamSeqType::ZeroIntro);
    }

    if (InputDevice()->Key_Tap(VK_F4))
    {
       // if (levelKey == "Gacha_Level") CamDirector()->RequestSequence("Gacha/Spin");
        if (levelKey == "Gacha_Level" || levelKey == "Zero_Level")
        {
            auto obj = Builder::Create_UIObject({G_GlobalLevelKey, "Proto_GameObject_Seoriyeol"}).Build("Seoriyeol");
            UIManager()->Add_UIObject(obj, LevelManager()->Get_NowLevelKey());
            static_cast<CUI_Seoriyeol*>(obj)->UI_Active({});
        }
    }

    if (CamDirector()->IsFinished(CamEventType::SpinFinished) || CamDirector()->IsFinished(CamEventType::SpinHalfFinished))
    {
        CameraManager()->SetZoomType(ENUM(CamZoomType::GachaShake), 2.f);
        
        // 1차 시도
        CameraManager()->AddShakeAxisWave(CamShakeAxis::Roll,  2.5f,   4.0f, 0.8f, 0.1f,  EaseType::InQuad, EaseType::OutQuad);
        CameraManager()->AddShakeAxisWave(CamShakeAxis::Yaw,   1.25f,  3.0f, 0.6f, 0.1f,  EaseType::InQuad, EaseType::OutQuad);
        CameraManager()->AddShakeAxisWave(CamShakeAxis::Pitch, 1.f,    2.5f, 0.4f, 0.1f,  EaseType::InQuad, EaseType::OutQuad);

        // 2차 시도
        //CameraManager()->AddShakeAxisWave(CamShakeAxis::Roll,  5.f,   4.0f, 1.f, 0.1f,  EaseType::InCubic, EaseType::OutCirc);
        //CameraManager()->AddShakeAxisWave(CamShakeAxis::Yaw,   2.5f,  3.0f, 1.f, 0.1f,  EaseType::InCubic, EaseType::OutCirc);
        //CameraManager()->AddShakeAxisWave(CamShakeAxis::Pitch, 1.25f, 2.0f, 1.f, 0.1f,  EaseType::InCubic, EaseType::OutCirc);

        // 3차 시도
        //CameraManager()->AddShakeAxisWave(CamShakeAxis::Roll,  2.2f, 4.0f, 0.8f, 0.2f, EaseType::OutSine, EaseType::OutQuad);
        //CameraManager()->AddShakeAxisWave(CamShakeAxis::Yaw,   1.1f, 3.0f, 0.8f, 0.2f, EaseType::OutSine, EaseType::OutQuad);
        //CameraManager()->AddShakeAxisWave(CamShakeAxis::Pitch, 0.6f, 2.5f, 0.8f, 0.2f, EaseType::OutSine, EaseType::OutQuad);
        
        
    }
}