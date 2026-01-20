#include "pch.h"
#include "CorinState_Start.h"
#include "Corin.h"

void CCorinState_Start::Enter(CCorin* pOwner)
{
	pOwner->Push_Invincible();
	pOwner->Lock_Move();
	pOwner->Get_Animator()->Change_Animation(pOwner->Get_Name() + "QuestStart")
		.Apply();
}

void CCorinState_Start::Update(CCorin* pOwner, _float dt)
{
	pOwner->Process_RootMotion(dt,
		ENUM(CCorin::ROOTMOTION_MASK::MOVE) |
		ENUM(CCorin::ROOTMOTION_MASK::QUATERNION));
}

void CCorinState_Start::Exit(CCorin* pOwner)
{
	pOwner->Unlock_Move();
	pOwner->Pop_Invincible();
}
