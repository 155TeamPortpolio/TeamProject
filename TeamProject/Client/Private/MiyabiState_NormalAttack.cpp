#include "pch.h"
#include "MiyabiState_NormalAttack.h"
#include "Miyabi.h"
#include "GameInstance.h"
#include "Animator3D.h"

void CMiyabiState_NormalAttack::Enter(CMiyabi* pOwner)
{
    m_iComboIndex = 0;
    m_bComboReserved = false;
    m_bIsEnd = false;
    m_fAnimProgress = 0.f;

    Play_ComboAnimation(pOwner);
}

void CMiyabiState_NormalAttack::Update(CMiyabi* pOwner, _float dt)
{
    // 입력 저장 (1회만, End 중에는 무시)
    if (CGameInstance::GetInstance()->Get_InputDev()->Mouse_Down(MOUSE_BTN::LB) && !m_bComboReserved && !m_bIsEnd)
        m_bComboReserved = true;

    // 애니메이션 끝나면
    if (Is_AnimEnd())
    {
        if (m_bIsEnd)
        {
            // End 애니메이션 끝 -> 상태 종료
            m_fAnimProgress = 1.f;
        }
        else if (m_bComboReserved && m_iComboIndex < 5)
        {
            // 다음 콤보로
            ++m_iComboIndex;
            m_bComboReserved = false;
            m_fAnimProgress = 0.f;
            Play_ComboAnimation(pOwner);
        }
        else
        {
            // End 애니메이션 재생
            m_bIsEnd = true;
            m_fAnimProgress = 0.f;
            Play_EndAnimation(pOwner);
        }
    }
}

void CMiyabiState_NormalAttack::Exit(CMiyabi* pOwner)
{
}

void CMiyabiState_NormalAttack::Play_ComboAnimation(CMiyabi* pOwner)
{
    const string arrAnims[6] =
    {
        "Avatar_Female_Size02_Unagi_Ani_Attack_01",
        "Avatar_Female_Size02_Unagi_Ani_Attack_02",
        "Avatar_Female_Size02_Unagi_Ani_Attack_03",
        "Avatar_Female_Size02_Unagi_Ani_Attack_04_02",
        "Avatar_Female_Size02_Unagi_Ani_Attack_05",
        "Avatar_Female_Size02_Unagi_Ani_Attack_06"
    };

    pOwner->Get_Animator()->Change_Animation(arrAnims[m_iComboIndex]);
}

void CMiyabiState_NormalAttack::Play_EndAnimation(CMiyabi* pOwner)
{
    const string arrEndAnims[6] =
    {
        "Avatar_Female_Size02_Unagi_Ani_Attack_01_End",
        "Avatar_Female_Size02_Unagi_Ani_Attack_02_End",
        "Avatar_Female_Size02_Unagi_Ani_Attack_03_End",
        "Avatar_Female_Size02_Unagi_Ani_Attack_04_02_End",
        "Avatar_Female_Size02_Unagi_Ani_Attack_05_End",
        "Avatar_Female_Size02_Unagi_Ani_Attack_06_End"
    };

    pOwner->Get_Animator()->Change_Animation(arrEndAnims[m_iComboIndex]);
}