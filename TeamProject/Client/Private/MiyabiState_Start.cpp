#include "pch.h"
#include "MiyabiState_Start.h"
#include "Miyabi.h"

void CMiyabiState_Start::Enter(CMiyabi* pOwner)
{
	pOwner->Push_Invincible();
	pOwner->Lock_Move();
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "QuestStart")
		.Apply();
}

void CMiyabiState_Start::Update(CMiyabi* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CMiyabi::ROOTMOTION_MASK::MOVE) |
		ENUM(CMiyabi::ROOTMOTION_MASK::QUATERNION));
}

void CMiyabiState_Start::Exit(CMiyabi* pOwner)
{
	pOwner->Unlock_Move();
	pOwner->Pop_Invincible();
}
