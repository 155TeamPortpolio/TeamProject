#include "Animator3DEX.h"

CAnimator3DEX::CAnimator3DEX()
{
}

CAnimator3DEX::CAnimator3DEX(const CAnimator3DEX& rhs)
	: CAnimator3D(rhs)
{
}

CAnimator3DEX* CAnimator3DEX::Create()
{
	CAnimator3DEX* instance = new CAnimator3DEX();
	if (FAILED(instance->Initialize_Prototype())) {
		Safe_Release(instance);
	}
	return instance;
}

CComponent* CAnimator3DEX::Clone()
{
	return new CAnimator3DEX(*this);
}

void CAnimator3DEX::Free()
{
	__super::Free();
}
