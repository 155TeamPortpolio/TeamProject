#include "pch.h"
#include "JaneDoeState_Start.h"
#include "JaneDoe.h"

void CJaneDoeState_Start::Enter(CJaneDoe* pOwner)
{
	pOwner->Push_Invincible();
	pOwner->Lock_Move();
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "QuestStart")
		.Apply();
}

void CJaneDoeState_Start::Update(CJaneDoe* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CJaneDoe::ROOTMOTION_MASK::MOVE) |
		ENUM(CJaneDoe::ROOTMOTION_MASK::QUATERNION));
}

void CJaneDoeState_Start::Exit(CJaneDoe* pOwner)
{
	pOwner->Unlock_Move();
	pOwner->Pop_Invincible();
}
